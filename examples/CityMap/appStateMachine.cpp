#include <appStateMachine.hpp>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <inputDataGL.hpp>

Application::Application()
{
    input = new Input();
    renderer = new Renderer();
    current = new SelectionState();
    ui = new SelectionUI(*this);
    current->Enter(*this);
    
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(renderer->windowWidth, renderer->windowHeight, "Pathfinding", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    auto* windowData = new WindowData{renderer, input};
    glfwSetWindowUserPointer(window, windowData);
    renderer->BindCallbacks(window);
    input->BindCallbacks(window);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW\n";
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, renderer->windowWidth, renderer->windowHeight);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, renderer->windowWidth, renderer->windowHeight, 0, -1, 1);
}

IAppState* const Application::GetCurrentState()
{
    return current;
}

void Application::Start()
{

}

void Application::Update()
{
    IAppState* _state = current->Update(*this);

    if(_state != nullptr)
    {
        current->Exit(*this);
        delete current;
        current = _state;
        current->Enter(*this);
    }

    renderer->Render(window);

    if(ui != nullptr)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ui->Show();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Application::BindCallbacks()
{

}

//SELECTION STATE
void SelectionState::Enter(Application& app)
{
    if(app.ui != nullptr)
    {
        delete app.ui;
    }

    app.ui = new SelectionUI(app);
}

IAppState* SelectionState::Update(Application& app)
{
    if(app.shouldLoad && app.mapReader.FileIsSet() && app.mapReader.BoundIsSet())
    {
        return new LoadingState();
    }

    return NULL;
}

void SelectionState::Exit(Application& app)
{

}

//LOADING STATE
void LoadingState::Enter(Application& app)
{
    std::thread readMap(&OSMDataReader::ReadFile, &app.mapReader); //std::async(std::launch::async, &OSMDataReader::ReadFile, &app.mapReader);
    readMap.detach();

    if(app.ui != nullptr)
    {
        delete app.ui;
    }
    
    app.ui = new LoadingUI(app);
}

IAppState* LoadingState::Update(Application& app)
{
    if(app.mapReader.status == FMHStatus::Ready)
    {
        return new MapState();
    }

    return NULL;
}

void LoadingState::Exit(Application& app)
{
    app.renderer->CreateMap(app.mapReader.graph);
}

//MAP STATE
void MapState::Enter(Application& app)
{
    if(app.ui != NULL)
    {
        delete app.ui;
    }
    state = MapNavigationState::Roaming;
    app.ui = new MapUI(app);
}

IAppState* MapState::Update(Application& app)
{
    return NULL;
}

void MapState::Exit(Application& app)
{
    
}

//TERMINATION STATE
void TerminationState::Enter(Application& app)
{

}

IAppState* TerminationState::Update(Application& app)
{
    return NULL;
}

void TerminationState::Exit(Application& app)
{
    
}

