#pragma once

#include <renderer.hpp>
#include <fileMapHandler.hpp>
#include <controller.hpp>
#include <widgets.hpp>
#include <future>

class IAppState;
class UI;
class SelectionUI;
enum class MapNavigationState;

class Application
{
    public:
        GLFWwindow* window = nullptr;
        OSMDataReader mapReader;
        Renderer* renderer = nullptr;
        Input* input = nullptr;
        UI* ui = nullptr;

        Application();
        bool shouldClose, shouldLoad = false;

        void BindCallbacks();
        void Start();
        void Update();
        IAppState* const GetCurrentState();

    private:
        IAppState* current = nullptr;
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
        MapNavigationState state;

        void Enter(Application& app) override;
        IAppState* Update(Application& app) override; 
        void Exit(Application& app) override;
};

enum class MapNavigationState
{
    Roaming,
    WaypointSelection,
    Pause
};

class TerminationState : public IAppState
{
    public:
        void Enter(Application& app) override;
        IAppState* Update(Application& app) override; 
        void Exit(Application& app) override;
};
