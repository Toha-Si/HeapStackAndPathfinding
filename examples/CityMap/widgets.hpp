#pragma once
#include <osmium/osm/box.hpp>
#include <vector>
#include <appStateMachine.hpp>

class UI
{
    public:
        UI(Application& application) : app(application)
        {
            
        }

        virtual void show();

    protected:
        Application& app;
};

class SelectionUI : public UI
{
    public:
        SelectionUI(Application& application) : UI(application) 
        {
            
        }

        void show() override;

    private:
        int selectedFile, selectedBound;
        //@Todo: Create a list of cities to choose
        //@Todo: better create a struct that will hold file name and its bounding boxes
        //@Todo: add an option to create a custom boundary
        const char* files[2]        = { "south-fed-district-latest.osm.pbf", "Sochi.osm" };
        const char* boundNames[2]   = { "BOLSHOY_SOCHI", "SOCHI"};
        const osmium::Box bounds[2] = { osmium::Box{osmium::Location{39.5961, 43.3671}, osmium::Location{40.2869, 43.6967}}, 
                                        osmium::Box{osmium::Location{39.6795, 43.5361}, osmium::Location{39.8131, 43.6507}} };
};

class LoadingUI : public UI
{
    public:
        void show() override;

    private:
        void DrawLoadingAnimation(const char* label, float radius, int num_segments, float thickness, float speed);
        char* StatusToString(FMHStatus s);
};

class MapUI : public UI
{
    public:
        void show() override;

    private:
        void DrawWaypointSelection();
        void DrawWayInfo();
        void DrawNodeInfo();
        void DrawNavigationWindow();
};
