#include <controller.hpp>

#include <iostream>
#include <cstdint> 
#include <GLFW/glfw3.h>
#include <renderer.hpp>

void Input::BindCallbacks(GLFWwindow* window)
{
    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPositionCallback);
}

void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    Input* input = (Input*) glfwGetWindowUserPointer(window);

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS) 
        {
            input->isDragging = true;
            glfwGetCursorPos(window, &input->lastMouseX, &input->lastMouseY);
        }
        else if (action == GLFW_RELEASE)
        {
            input->isDragging = false;
        }
    }
}

void Input::CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    Input* input = (Input*) glfwGetWindowUserPointer(window);
    Renderer* rnd = (Renderer*) glfwGetWindowUserPointer(window);
    Camera& cam = rnd->cam;

    if (input->isDragging)
    {
        double dx = xpos - input->lastMouseX;
        double dy = ypos - input->lastMouseY;

        cam.positionX -= dx / cam.scale;
        cam.positionY += dy / cam.scale;

        input->lastMouseX = xpos;
        input->lastMouseY = ypos;
    }
}
//@Todo: move to renderer class?
void Input::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    Renderer* rnd = (Renderer*) glfwGetWindowUserPointer(window);
    Camera& cam = rnd->cam;

    double cursorX, cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);
    glm::vec2 csrWorldBefore = rnd->CursorToWorld(glm::vec2(cursorX, cursorY));

    float zoomFactor = (yoffset > 0) ? 1.1f : 0.9f;
    cam.SetScale(cam.scale * zoomFactor);

    glm::vec2 csrWorldAfter = rnd->CursorToWorld(glm::vec2(cursorX, cursorY));

    // Корректируем смещение, чтобы курсор оставался в том же месте в мировых координатах
    cam.positionX += (csrWorldBefore.x - csrWorldAfter.x);
    cam.positionY += (csrWorldBefore.y - csrWorldAfter.y);
}
