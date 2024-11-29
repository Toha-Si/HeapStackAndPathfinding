#include <renderer.hpp>

void Camera::SetPosition(float x, float y)
{
    //@Todo: clamp values so can't go past a bounding box
    positionX = x;
    positionY = y;
}

void Camera::SetScale(float scale)
{
    this->scale = (scale < this->minScale) ? this->minScale : 
                  (scale > this->maxScale) ? this->maxScale : scale;
}

void Renderer::CreateMap(Graph& map)
{
    this->vertices = map.GetVerticesScreenSpace(*this);

    GLuint VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);    
}

void Renderer::BindWindowSizeCallback(GLFWwindow*& window)
{
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    float lastFrameTime = glfwGetTime();
}

void Renderer::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Renderer* rndr = (Renderer*) glfwGetWindowUserPointer(window);
    rndr->windowWidth = width;
    rndr->windowHeight = height;
    glViewport(0, 0, width, height);
}

void Renderer::RenderVertices(GLFWwindow* window)
{
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-0.5f * windowWidth / cam.scale + cam.positionX, 
                 0.5f * windowWidth / cam.scale + cam.positionX, 
                -0.5f * windowHeight / cam.scale + cam.positionY, 
                 0.5f * windowHeight / cam.scale + cam.positionY, 
                -1.0f, 1.0f);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, nullptr);
        glDrawArrays(GL_LINES, 0, vertices.size() / 2);
        glDisableClientState(GL_VERTEX_ARRAY);
}

void Renderer::Render(GLFWwindow* window)
{
    if(!vertices.empty())
    {
        RenderVertices(window);
    }

    ui->show();

    glfwSwapBuffers(window);
    glfwPollEvents();
}
glm::vec2 Renderer::LocationToScreen(const osmium::Location& location, const osmium::Box& bounds)
{
    double u = (bounds.left()  -  location.lon()) / (bounds.left() - bounds.right()) * windowWidth;
    double v = (location.lat() - bounds.bottom()) / (bounds.top() - bounds.bottom()) * windowHeight;

    return glm::vec2(u, v);
}

glm::vec2 Renderer::CursorToWorld(const glm::vec2& cursorPos)
{
    glm::vec2 cursorWorld;
    // Переводим экранные координаты в диапазон [-1, 1]
    float normalizedX = (2.0f * cursorPos.x) / windowWidth - 1.0f;
    float normalizedY = 1.0f - (2.0f * cursorPos.y) / windowHeight; // Y инвертирован в OpenGL

    // Переводим нормализованные координаты в мировое пространство
    cursorWorld.x = normalizedX * (0.5f * windowWidth / cam.scale) + cam.positionX;
    cursorWorld.y = normalizedY * (0.5f * windowHeight / cam.scale) + cam.positionY;

    return cursorWorld;
}
// void Renderer::RenderVertices(GLFWwindow* window, const std::vector<float>& vertices)
// {
//     GLuint VBO;
//     glGenBuffers(1, &VBO);
//
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
//
//     float lastFrameTime = glfwGetTime();
//
//     while (!glfwWindowShouldClose(window))
//     {
//         float currentFrameTime = glfwGetTime();
//         float deltaTime = currentFrameTime - lastFrameTime;
//         lastFrameTime = currentFrameTime;
//
//         glClear(GL_COLOR_BUFFER_BIT);
//
//         glMatrixMode(GL_PROJECTION);
//         glLoadIdentity();
//         glOrtho(-0.5f * WINDOW_WIDTH / zoom + offsetX, 
//                 0.5f * WINDOW_WIDTH  / zoom + offsetX, 
//                -0.5f * WINDOW_HEIGHT / zoom + offsetY, 
//                 0.5f * WINDOW_HEIGHT / zoom + offsetY, 
//                -1.0f, 1.0f);
//
//         glEnableClientState(GL_VERTEX_ARRAY);
//         glVertexPointer(2, GL_FLOAT, 0, nullptr);
//         glDrawArrays(GL_LINES, 0, vertices.size() / 2);
//         glDisableClientState(GL_VERTEX_ARRAY);
//
//         ImGui_ImplOpenGL3_NewFrame();
//         ImGui_ImplGlfw_NewFrame();
//         ImGui::NewFrame();
//
//         ImGui::Begin("UI Controls");
//         ImGui::Text("Graph Visualization");
//         ImGui::SliderFloat("Zoom", &zoom, 0.01f, 100.0f);
//         ImGui::SliderFloat("Offset X", &offsetX, -100.0f, 100.0f);
//         ImGui::SliderFloat("Offset Y", &offsetY, -100.0f, 100.0f);
//
//         if (ImGui::Button("Reset View")) {
//             zoom = 1.0f;
//             offsetX = 0.0f;
//             offsetY = 0.0f;
//         }
//         ImGui::End();
//
//         ImGui::Render();
//         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//         glfwSwapBuffers(window);
//         glfwPollEvents();
// }