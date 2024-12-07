#include <renderer.hpp>
#include <inputDataGL.hpp>
#include <iostream>

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
    this->map = &map;

    this->vertices = VertBufferFrom(map);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &pathVBO);
    glGenBuffers(1, &animVBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, animVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::SetPath(Graph& map, Path& way)
{
    std::vector<int> path = way.pointIDs;
    this->pathSteps = way.searchSteps;

    if(!pathVertices.empty())
    {
        pathVertices.clear();
    }

    for (size_t i = 0; i < path.size() - 1; ++i)
    {
        const osmium::Location& nodeFrom = map.nodeLocations.at(path[i]);
        const osmium::Location& nodeTo = map.nodeLocations.at(path[i + 1]);
        pathVertices.push_back(nodeFrom.lon());
        pathVertices.push_back(nodeFrom.lat());
        pathVertices.push_back(nodeTo.lon());
        pathVertices.push_back(nodeTo.lat());
    }

    glBindBuffer(GL_ARRAY_BUFFER, pathVBO);
    glBufferData(GL_ARRAY_BUFFER, pathVertices.size() * sizeof(float), pathVertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

void Renderer::RenderVertices(GLFWwindow* window, GLuint& vbo, std::vector<float>& verts, const glm::vec3& color, float lineWidth)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glColor3f(color.r, color.g, color.b);
    glLineWidth(lineWidth);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5f * windowWidth  / cam.scale + cam.positionX, 
             0.5f * windowWidth  / cam.scale + cam.positionX, 
            -0.5f * windowHeight / cam.scale + cam.positionY, 
             0.5f * windowHeight / cam.scale + cam.positionY, 
            -1.0f, 1.0f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);
    glDrawArrays(GL_LINES, 0, verts.size() / 2);
    glDisableClientState(GL_VERTEX_ARRAY);

    glLineWidth(1.0f);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::Render(GLFWwindow* window)
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    if(!vertices.empty())
    {
        RenderVertices(window, VBO, vertices, {0.6f, 0.6f, 0.6f}, 1.0f);
    }

    if(isShowingAlgoWork)
    {
        PrepareAnimationStep(1000);
        RenderVertices(window, animVBO, animVertices, {0.0f, 0.0f, 1.0f}, 1.0f);
    }
    else
    {
        animVertices = std::vector<float>();
        step = 0;
    }

    if(!pathVertices.empty())
    {
        RenderVertices(window, pathVBO, pathVertices, {1.0f, 0.0f, 0.0f}, 3.0f);
    }

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
    cursorWorld.x = normalizedX * (0.5f * windowWidth  / cam.scale) + cam.positionX;
    cursorWorld.y = normalizedY * (0.5f * windowHeight / cam.scale) + cam.positionY;

    return cursorWorld;
}

std::vector<float> Renderer::VertBufferFrom(Graph& graph)
{
    std::vector<float> vertices;

    for (const auto& [nodeID, edges] : graph.data) 
    {
        osmium::Location nodeFrom = graph.nodeLocations[nodeID];

        for (const auto& edge : edges) 
        {
            osmium::Location nodeTo = graph.nodeLocations[edge.nodeToID];
            vertices.push_back(nodeFrom.lon());
            vertices.push_back(nodeFrom.lat());
            vertices.push_back(nodeTo.lon());
            vertices.push_back(nodeTo.lat());
        }
    }

    return vertices;
}

void Renderer::DrawPoint(osmium::Location& nodePos, float size, float red, float green, float blue)
{
    glPointSize(size);
    glColor3f(red, green, blue);
    glBegin(GL_POINTS);
    glVertex2f(nodePos.lon(), nodePos.lat());
    glEnd();
    glColor3f(1, 1, 1); 
}

void Renderer::PrepareAnimationStep(int stepSize)
{
    if(map == nullptr) return;

    if (step >= pathSteps.size() - 1) return;

    int maxStep = (step + stepSize >= pathSteps.size() - 1 ) ? 
                            pathSteps.size() - 1 : 
                            step + stepSize;

    for(size_t i = step; i < maxStep; ++i)
    {
        osmium::Location& nodeFrom = map->nodeLocations[pathSteps.at(i).fromNodeID]; 
        osmium::Location& nodeTo = map->nodeLocations[pathSteps.at(i).toNodeID]; 

        animVertices.push_back(nodeFrom.lon());
        animVertices.push_back(nodeFrom.lat());
        animVertices.push_back(nodeTo.lon());
        animVertices.push_back(nodeTo.lat());
    }

    size_t offset = (animVertices.size() - 4*(maxStep - step)) * sizeof(float);

    glBindBuffer(GL_ARRAY_BUFFER, animVBO);
    glBufferSubData(GL_ARRAY_BUFFER, offset, 4 * (maxStep - step) * sizeof(float), animVertices.data() + animVertices.size() - 4 * (maxStep - step));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    step += maxStep - step;
}

Renderer::~Renderer()
{
    //delete pathSteps;
    //delete map;
}