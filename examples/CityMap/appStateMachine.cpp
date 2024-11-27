#include <appStateMachine.hpp>

void Application::Update()
{
    IAppState* _state = current->Update(*this);

    if(_state != NULL)
    {
        delete current;
        current = _state;

        current->Enter(*this);
    }

    renderer.Render(window);
}

IAppState* SelectionState::Update(Application& app)
{
    return NULL;
}

IAppState* LoadingState::Update(Application& app)
{
    if(app.mapReader.status == FMHStatus::Ready)
    {
        this->Exit(app);
        return new MapState();
    }

    return NULL;
}