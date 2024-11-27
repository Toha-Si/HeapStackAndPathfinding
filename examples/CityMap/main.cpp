#include <appStateMachine.hpp>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void initOpenGL(Application& app, GLFWwindow*& window)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(app.renderer.windowWidth, app.renderer.windowHeight, "Pathfinding", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    app.BindCallbacks();

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW\n";
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, app.renderer.windowWidth, app.renderer.windowHeight);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, app.renderer.windowWidth, app.renderer.windowHeight, 0, -1, 1);
}

int main(int argc, char** argv)
{
    GLFWwindow* window;
    Application app;
    initOpenGL(app, window);

    while (!app.shouldClose)
    {
        app.Update();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}