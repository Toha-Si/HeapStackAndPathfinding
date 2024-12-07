#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <appStateMachine.hpp>

int main(int argc, char** argv)
{
    Application* app = new Application();
    
    while (!glfwWindowShouldClose(app->window))
    {
        app->Update();
    }

    glDeleteBuffers(1, &app->renderer->VBO);
    glDeleteBuffers(1, &app->renderer->pathVBO);
    glDeleteBuffers(1, &app->renderer->animVBO);
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(app->window);
    glfwTerminate();
    delete app;
}