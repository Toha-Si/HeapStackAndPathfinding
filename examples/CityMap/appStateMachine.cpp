#include <appStateMachine.hpp>

void Application::Start()
{
    current = new SelectionState();
    current->Enter(*this);
}

void Application::Update()
{
    IAppState* _state = current->Update(*this);

    if(_state != NULL)
    {
        current->Exit(*this);
        delete current;
        current = _state;
        current->Enter(*this);
    }

    renderer.Render(window);
}

void Application::BindCallbacks(GLFWwindow*& window)
{
    renderer.BindWindowSizeCallback(window);
    input.BindCallbacks(window);
}

void SelectionState::Enter(Application& app)
{
    if(app.renderer.ui != NULL)
    {
        delete app.renderer.ui;
    }

    app.renderer.ui = new SelectionUI(app);
}

IAppState* SelectionState::Update(Application& app)
{
    if(app.mapReader.FileIsSet() && app.mapReader.BoundIsSet())
    {
        return new LoadingState();
    }

    return NULL;
}

void LoadingState::Enter(Application& app)
{
    app.mapReader.ReadFile();
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
    app.renderer.CreateMap(app.mapReader.graph);
}