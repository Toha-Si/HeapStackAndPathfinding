#pragma once
#include <algos.hpp>
#include <graph.hpp>


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <osmium/osm/box.hpp>
#include <vector>
#include <future>
class Camera
{
    public:
        //@Todo: make this private and add getters
        float positionX, positionY;
        float scale = 1;

        void SetPosition(float x, float y);
        void SetScale(float scale);

    private:
        float minScale = 0.1f; //Should depend on size of a bounding box?
        float maxScale = 1000000;
};

class Renderer
{
    public:
        int windowWidth = 800;
        int windowHeight = 600;
        GLuint VBO;
        GLuint pathVBO;
        GLuint animVBO;

        Camera cam;
        bool isShowingAlgoWork = false;

        void BindCallbacks(GLFWwindow* window);
        void CreateMap(Graph& map);
        void SetPath(Graph& map, Path& path);
        
        void Render(GLFWwindow* window);
        void DrawPoint(osmium::Location& nodePos, float size, float r, float g, float b);
        void PrepareAnimationStep(int stepSize);

        osmium::Location ScreenToLocation(const glm::vec2& location);
        glm::vec2 CursorToCamPos(const glm::vec2& cursorPos);
        ~Renderer();

    private:
        float lastFrameTime; 
        int step = 0;
        std::vector<float> vertices;
        std::vector<float> pathVertices;
        std::vector<float> animVertices;
        std::vector<SearchStep> pathSteps;
        Graph* map;

        std::vector<float> VertBufferFrom(Graph& graph);
        void RenderVertices(GLFWwindow* window, GLuint& vbo, std::vector<float>& verts, const glm::vec3& color, float lineWidth);
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
};