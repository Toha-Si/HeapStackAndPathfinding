#pragma once
#include <renderer.hpp>
//seems like it's deprecated because of imgui
class Input
{
    public:
        //@Suggestion: use command pattern for ui?
        bool isDragging = false;
        bool controlCamera = false;
        double lastMouseX = 0.0, lastMouseY = 0.0;

        void BindCallbacks(GLFWwindow* window);

    private:
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
};