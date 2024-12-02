#include <renderer.hpp>
#include <inputDataGL.hpp>
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
    this->vertices = CreateVerticesFrom(map);

    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);    
}

void Renderer::BindCallbacks(GLFWwindow* window)
{
    glfwSetFramebufferSizeCallback(window, Renderer::FramebufferSizeCallback);
    glfwSetScrollCallback(window, Renderer::ScrollCallback);
}

void Renderer::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
    if (!data || !data->renderer)
    {
        return;
    }

    Renderer* rndr = data->renderer;

    rndr->windowWidth = width;
    rndr->windowHeight = height;
    glViewport(0, 0, rndr->windowWidth, rndr->windowHeight);
}

void Renderer::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
    if (!data || !data->renderer)
    {
        return;
    }

    Renderer* rndr = data->renderer;

    double cursorX, cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);
    glm::vec2 csrWorldBefore = rndr->CursorToCamPos(glm::vec2(cursorX, cursorY));

    float zoomFactor = (yoffset > 0) ? 1.1f : 0.9f;
    rndr->cam.SetScale(rndr->cam.scale * zoomFactor);

    glm::vec2 csrWorldAfter = rndr->CursorToCamPos(glm::vec2(cursorX, cursorY));

    // Корректируем смещение, чтобы курсор оставался в том же месте в мировых координатах
    rndr->cam.positionX += (csrWorldBefore.x - csrWorldAfter.x);
    rndr->cam.positionY += (csrWorldBefore.y - csrWorldAfter.y);
}

void Renderer::RenderVertices(GLFWwindow* window)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5f * windowWidth  / cam.scale + cam.positionX, 
             0.5f * windowWidth  / cam.scale + cam.positionX, 
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
    glClear(GL_COLOR_BUFFER_BIT);

    if(vertices.empty()) return;

    RenderVertices(window);
}

osmium::Location Renderer::ScreenToLocation(const glm::vec2& screenLoc)
{
    float normalizedX = (2.0f * screenLoc.x) / windowWidth - 1.0f;
    float normalizedY = 1.0f - (2.0f * screenLoc.y) / windowHeight; // Y инвертирован в OpenGL

    // Переводим нормализованные координаты в мировое пространство
    double lon = normalizedX * (0.5f * windowWidth / cam.scale) + cam.positionX;
    double lat = normalizedY * (0.5f * windowHeight / cam.scale) + cam.positionY;

    return osmium::Location(lon, lat);
}

glm::vec2 Renderer::CursorToCamPos(const glm::vec2& cursorPos)
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

std::vector<float> Renderer::CreateVerticesFrom(Graph& graph)
{
    std::vector<float> vertices;

    for (const auto& [nodeID, edges] : graph.data) 
    {
        osmium::Location nodeFrom = graph.nodeLocations[nodeID];
        //glm::vec2 nodeFrom = LocationToScreen(graph.nodeLocations[nodeID], graph.box);

        for (const auto& edge : edges) 
        {
            osmium::Location nodeTo = graph.nodeLocations[edge.nodeToID];
            //glm::vec2 nodeTo = LocationToScreen(graph.nodeLocations[edge.nodeToID], graph.box);

            // vertices.push_back(nodeFrom.x);
            // vertices.push_back(nodeFrom.y);
            // vertices.push_back(nodeTo.x);
            // vertices.push_back(nodeTo.y);
            vertices.push_back(nodeFrom.lon());
            vertices.push_back(nodeFrom.lat());
            vertices.push_back(nodeTo.lon());
            vertices.push_back(nodeTo.lat());
        }
    }

    return vertices;
}

void Renderer::DrawPoint(float x, float y, float size, float red, float green, float blue)
{
    glPointSize(size);
    glColor3f(red, green, blue);
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
    glColor3f(1, 1, 1); 
}