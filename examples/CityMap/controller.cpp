#include <controller.hpp>

#include <iostream>
#include <cstdint> 
#include <inputDataGL.hpp>

void Input::BindCallbacks(GLFWwindow* window)
{
    glfwSetMouseButtonCallback(window, Input::MouseButtonCallback);
    glfwSetCursorPosCallback(window, Input::CursorPositionCallback);
}

void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
    if (!data || !data->input)
    {
        return;
    }

    Input* input = data->input;

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
    WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

    if (!data || !data->renderer || !data->input)
    {
        return;
    }

    Input* input = data->input;
    Camera& cam = data->renderer->cam;

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
