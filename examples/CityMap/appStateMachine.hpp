#pragma once

#include <fileMapHandler.hpp>
#include <renderer.hpp>
#include <widgets.hpp>
#include <controller.hpp>

class Application
{
    public:
        GLFWwindow* window;
        OSMDataReader mapReader;
        Renderer renderer;
        Input input;

        bool shouldClose = false;

        Application()
        {
            current = new SelectionState();
            current->Enter(*this);
        }
        void BindCallbacks();
        void Update();

    private:
        IAppState* current;

};

class IAppState
{
    public:
        virtual void Enter(Application& app) = 0;
        virtual IAppState* Update(Application& app) = 0;
        virtual void Exit(Application& app) = 0;
        virtual ~IAppState() {}
};

class SelectionState : public IAppState
{
    public:
        void Enter(Application& app) override;
        IAppState* Update(Application& app) override; 
        void Exit(Application& app) override;
};

class LoadingState : public IAppState
{
    public:
        void Enter(Application& app) override;
        IAppState* Update(Application& app) override; 
        void Exit(Application& app) override;
};

class MapState : public IAppState
{
    public:
        void Enter(Application& app) override;
        IAppState* Update(Application& app) override; 
        void Exit(Application& app) override;
};

class TerminationState : public IAppState
{
    public:
        void Enter(Application& app) override;
        IAppState* Update(Application& app) override; 
        void Exit(Application& app) override;
};