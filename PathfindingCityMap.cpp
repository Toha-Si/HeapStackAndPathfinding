#include <iostream>
#include <cstdint> 
#include <unordered_map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <osmium/io/any_input.hpp>
#include <osmium/util/file.hpp>
#include <osmium/osm/box.hpp>
#include <osmium/visitor.hpp>
#include <osmium/handler.hpp>
#include <osmium/util/memory.hpp>
#include <osmium/geom/haversine.hpp>
#include <osmium/util/progress_bar.hpp>

struct Edge
{
    int nodeToID;
    double distance;
};

std::unordered_map<int, osmium::Location> nodeLocations;
std::unordered_map<int, std::vector<Edge>> graph;

struct NodeHandler : public osmium::handler::Handler
{
    public:
        void node(const osmium::Node& node)
        {
            if (node.location()) 
            {
                nodeLocations[node.id()] = node.location();
            }
        }
};

struct RoadHandler : public osmium::handler::Handler
{
    const osmium::Box BOX_BOLSHOY_SOCHI = osmium::Box
    {
        osmium::Location{39.5961, 43.3671},
        osmium::Location{40.2869, 43.6967}
    };

    const osmium::Box BOX_SOCHI = osmium::Box
    {
        osmium::Location{39.6795, 43.5361},
        osmium::Location{39.8131, 43.6507}
    };

    void way(const osmium::Way& way)
    {
        const char* highway = way.tags()["highway"];

        if (!highway)
        {
            return;
        }

        bool intersectsBox = false;

        for (const auto& nodeRef : way.nodes())
        {
            auto it = nodeLocations.find(nodeRef.ref());

            if (it != nodeLocations.end()) 
            {
                osmium::Location loc = it->second;

                if (BOX_BOLSHOY_SOCHI.contains(loc)) 
                {
                    intersectsBox = true;
                }
            }
        }

        if (!intersectsBox)
        {
            return;
        }

        const char* oneway = way.tags()["oneway"];
        bool isOneway = false;
        bool isReversed = false;

        if (oneway) 
        {
            if (std::strcmp(oneway, "yes") == 0 || std::strcmp(oneway, "1") == 0) 
            {
                isOneway = true;
            } 
            else if (std::strcmp(oneway, "-1") == 0) 
            {
                isOneway = true;
                isReversed = true;
            }
        }

        const auto& nodes = way.nodes();

        for (size_t i = 0; i < nodes.size() - 1; ++i)
        {
            int fromID = nodes[i].ref();
            int toID = nodes[i+1].ref();

            if (nodeLocations.count(fromID) && nodeLocations.count(toID))
            {
                const auto& distance = osmium::geom::haversine::distance(osmium::geom::Coordinates(nodeLocations[fromID]), 
                                                                         osmium::geom::Coordinates(nodeLocations[ toID ]));
                if (!isOneway)
                {
                    graph[fromID].push_back({toID, distance});
                    graph[toID].push_back({fromID, distance});
                }
                else
                {
                    if(isReversed)
                    {
                        graph[toID].push_back({fromID, distance});
                    }
                    else
                    {
                        graph[fromID].push_back({toID, distance});
                    }
                }
            }
        }
    }
};

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
float zoom = 1.0f;
float offsetX = 0.0f;
float offsetY = 0.0f;
bool isDragging = false;
double lastMouseX = 0.0, lastMouseY = 0.0;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS) 
        {
            isDragging = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        }
        else if (action == GLFW_RELEASE)
        {
            isDragging = false;
        }
    }
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (isDragging)
    {
        double dx = xpos - lastMouseX;
        double dy = ypos - lastMouseY;

        offsetX -= dx / zoom;
        offsetY += dy / zoom;

        lastMouseX = xpos;
        lastMouseY = ypos;
    }
}

void cursorToWorld(GLFWwindow* window, double cursorX, double cursorY, float& worldX, float& worldY) {
    // Переводим экранные координаты в диапазон [-1, 1]
    float normalizedX = (2.0f * cursorX) / WINDOW_WIDTH - 1.0f;
    float normalizedY = 1.0f - (2.0f * cursorY) / WINDOW_HEIGHT; // Y инвертирован в OpenGL

    // Переводим нормализованные координаты в мировое пространство
    worldX = normalizedX * (WINDOW_WIDTH / 2.0f / zoom) + offsetX;
    worldY = normalizedY * (WINDOW_HEIGHT / 2.0f / zoom) + offsetY;
}

//Зум относительно фиксированной точки
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    double cursorX, cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);

    // Преобразуем позицию курсора в мировые координаты до изменения зума
    float worldXBefore, worldYBefore;
    cursorToWorld(window, cursorX, cursorY, worldXBefore, worldYBefore);

    float oldZoom = zoom;
    zoom *= (yoffset > 0) ? 1.1f : 0.9f;
    zoom = std::max(0.01f, std::min(zoom, 100.0f));

    // Преобразуем позицию курсора в мировые координаты после изменения зума
    float worldXAfter, worldYAfter;
    cursorToWorld(window, cursorX, cursorY, worldXAfter, worldYAfter);

    // Корректируем смещение, чтобы курсор оставался в том же месте в мировых координатах
    offsetX += (worldXBefore - worldXAfter);
    offsetY += (worldYBefore - worldYAfter);
}

//Для поддержки изменения размера окна
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
    glViewport(0, 0, width, height);
}

glm::vec2 locToScreen(const osmium::Location &location, const osmium::Box &bounds)
{
    double u = (bounds.left() -  location.lon()) / (bounds.left() - bounds.right()) * WINDOW_WIDTH;
    double v = (location.lat() - bounds.bottom()) / (bounds.top() - bounds.bottom()) * WINDOW_HEIGHT;

    return glm::vec2(u, v);
}

std::vector<float> prepareGraphVertices(const osmium::Box &bounds) 
{
    std::vector<float> vertices;

    for (const auto& [nodeID, edges] : graph) 
    {

        glm::vec2 nodeFrom = locToScreen(nodeLocations[nodeID], bounds);

        for (const auto& edge : edges) 
        {
            glm::vec2 nodeTo = locToScreen(nodeLocations[edge.nodeToID], bounds);

            vertices.push_back(nodeFrom.x);
            vertices.push_back(nodeFrom.y);
            vertices.push_back(nodeTo.x);
            vertices.push_back(nodeTo.y);
        }
    }

    return vertices;
}

void initOpenGL(GLFWwindow*& window) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Pathfinding Sochi Example", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
}

void renderGraph(GLFWwindow* window, const std::vector<float>& vertices) {
    GLuint VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    float lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-0.5f * WINDOW_WIDTH / zoom + offsetX, 
                0.5f * WINDOW_WIDTH  / zoom + offsetX, 
               -0.5f * WINDOW_HEIGHT / zoom + offsetY, 
                0.5f * WINDOW_HEIGHT / zoom + offsetY, 
               -1.0f, 1.0f);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, nullptr);
        glDrawArrays(GL_LINES, 0, vertices.size() / 2);
        glDisableClientState(GL_VERTEX_ARRAY);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
}

int main(int argc, char** argv)
{
    if (argc != 2) 
    {
        std::cerr << "Usage: " << argv[0] << " OSMFILE\n";
        return 1;
    }

    try
    {
        const osmium::io::File inputFile{argv[1]};

        osmium::io::Reader nodeReader{inputFile, osmium::osm_entity_bits::node};
        NodeHandler nodeHandler;

        std::cout << "\nStarted writing nodes\n";

        osmium::apply(nodeReader, nodeHandler);

        std::cout << "\nCompleted writing nodes\n";

        RoadHandler roadHandler;
        osmium::io::Reader roadReader{inputFile, osmium::osm_entity_bits::way};

        std::cout << "\nConstructing graph\n";

        osmium::apply(roadReader, roadHandler);

        std::cout << "\nCompleted constructing graph\n";

        const osmium::MemoryUsage memory;

        std::cout << "\nMemory used: " << memory.peak() << " MBytes\n";

        std::vector<float> vertices = prepareGraphVertices(roadHandler.BOX_SOCHI);

        GLFWwindow* window;
        initOpenGL(window);


        renderGraph(window, vertices);

        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }
    catch(const std::exception e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
}