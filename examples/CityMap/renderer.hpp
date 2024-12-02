#pragma once
#include <graph.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <osmium/osm/box.hpp>
#include <vector>

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
        Camera cam;

        void CreateMap(Graph& map);
        void BindCallbacks(GLFWwindow* window);
        void Render(GLFWwindow* window);
        void DrawPoint(float x, float y, float size, float r, float g, float b);
        osmium::Location ScreenToLocation(const glm::vec2& location);
        glm::vec2 CursorToCamPos(const glm::vec2& cursorPos);

    private:
        float lastFrameTime; 
        std::vector<float> vertices;

        std::vector<float> CreateVerticesFrom(Graph& graph);
        void RenderVertices(GLFWwindow* window);
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
};
