#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <osmium/osm/box.hpp>
#include <graph.hpp>
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
        float minScale = 0.01f; //Should depent on size of a bounding box?
        float maxScale = 100;
};

class Renderer
{
    public:
        int windowWidth = 800;
        int windowHeight = 600;
        Camera cam;
        UI* ui;

        void CreateMap(Graph& map);
        void BindWindowSizeCallback(GLFWwindow*& window);
        void Render(GLFWwindow* window);
        glm::vec2 LocationToScreen(const osmium::Location& location, const osmium::Box& bounds);
        glm::vec2 CursorToWorld(const glm::vec2& cursorPos);

    private:
        float lastFrameTime; 
        std::vector<float> vertices;

        void RenderVertices(GLFWwindow* window);
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};

