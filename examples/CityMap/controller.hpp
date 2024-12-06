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
        double mouseX = 0.0, mouseY = 0.0;

        void BindCallbacks(GLFWwindow* window);
    private:
        int holdForFrames = 0;
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
};