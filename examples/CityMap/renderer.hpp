#pragma once

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
        float minScale = 0.01f; //Should depent on size of a bounding box?
        float maxScale = 100;
};

class Renderer
{
    public:
        int windowWidth = 800;
        int windowHeight = 600;
        Camera cam;

        void BindWindowSizeCallback(GLFWwindow*& window);
        void Render(GLFWwindow* window);
        void RenderVertices(GLFWwindow* window, const std::vector<float>& vertices); //@Todo: rethink how to render stuff

        glm::vec2 LocationToScreen(const osmium::Location& location, const osmium::Box& bounds);
        glm::vec2 CursorToWorld(const glm::vec2& cursorPos);

    private:
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};

