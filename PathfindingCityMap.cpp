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

                if (BOX_SOCHI.contains(loc)) 
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

    private:
        bool checkIntersection(const osmium::Box &box)
        {
            return false;
        }
};

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int MAP_OFFSET_FROM_BOUND = 1;

glm::vec2 loc2Screen(const osmium::Location &location, const osmium::Box &bounds)
{
    double u = (location.lon() - bounds.right()) / (bounds.left() - bounds.right()) * WINDOW_WIDTH;
    double v = (bounds.top()   - location.lat()) / (bounds.top() - bounds.bottom()) * WINDOW_HEIGHT;

    return glm::vec2(u, v);
}

std::vector<float> prepareGraphVertices(const osmium::Box &bounds) 
{
    std::vector<float> vertices;

    for (const auto& [nodeID, edges] : graph) 
    {

        glm::vec2 nodeFrom = loc2Screen(nodeLocations[nodeID], bounds);

        for (const auto& edge : edges) 
        {
            glm::vec2 nodeTo = loc2Screen(nodeLocations[edge.nodeToID], bounds);

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
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
}

// Отрисовка графа
void renderGraph(GLFWwindow* window, const std::vector<float>& vertices) {
    GLuint VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

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
        osmium::apply(nodeReader, nodeHandler);

        RoadHandler roadHandler;
        osmium::io::Reader roadReader{inputFile, osmium::osm_entity_bits::way};
        osmium::apply(roadReader, roadHandler);

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