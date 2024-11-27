#pragma once
#include <osmium/osm/box.hpp>
#include <vector>
class UI
{
    public:
        virtual void show();
};

class SelectionUI : public UI
{
    public:
        void show() override;
    
    private:
        int selectedFile, selectedBound;
        const char* files[2] = { "south-fed-district-latest.osm.pbf", "Sochi.osm" };
        const char* boundNames[2] = { "BOLSHOY_SOCHI", "SOCHI"};
        const osmium::Box bounds[2] = { osmium::Box{osmium::Location{39.5961, 43.3671}, osmium::Location{40.2869, 43.6967}}, 
                                        osmium::Box{osmium::Location{39.6795, 43.5361}, osmium::Location{39.8131, 43.6507}} };
};

class LoadingUI : public UI
{
    public:
        void show() override;
};

class MapUI : public UI
{
    public:
        void show() override;
};
