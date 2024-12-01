#pragma once
#include <osmium/osm/box.hpp>
#include <vector>
#include <appStateMachine.hpp>
#include <algos.hpp>
#include <string>

class Application;
class MapState;

class UI
{
    public:
        UI(Application& application);
        virtual void Show() = 0;
        virtual ~UI();
    protected:
        Application& app;
};

class SelectionUI : public UI
{
    public:
        SelectionUI(Application& application);
        void Show() override;
    private:
        int selectedFile, selectedBound = 0;
        //@Todo: better create a struct that will hold file name and its bounding boxes
        //@Todo: add an option to create a custom boundary
        char* files[2]              = { "south-fed-district-latest.osm.pbf", "Sochi.osm" };
        const char* boundNames[2]   = { "BOLSHOY_SOCHI", "SOCHI"};
        const osmium::Box bounds[2] = { osmium::Box{osmium::Location{39.5961, 43.3671}, osmium::Location{40.2869, 43.6967}}, 
                                        osmium::Box{osmium::Location{39.6795, 43.5361}, osmium::Location{39.8131, 43.6507}} };
};

class LoadingUI : public UI
{
    public:
        LoadingUI(Application& application);
        void Show() override;
    private:
        void DrawLoadingAnimation(const char* label, float radius, int num_segments, float thickness, float speed);
        std::string StatusToString(FMHStatus s);
};

class MapUI : public UI
{
    public:
        MapUI(Application& application);
        void Show() override;
        void SetWaypoint(int ID);
        void ResetWaypoints();
    private:
        //@Note: would refactor if it becomes too tiresome to extend
        int startNodeID, endNodeID;
        bool startNodeSet, endNodeSet;
        PathfindingAlgorithm* searchAlgo;

        void DrawWayInfo(MapState* map, bool isSelecting);
        void DrawNodeInfo(int ID);
        void DrawAppNavWindow();
};
