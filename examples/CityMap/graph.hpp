#pragma once
#include <unordered_map>
#include <osmium/osm/box.hpp>
#include <vector>

struct Edge
{
    int nodeToID;
    double distance;
};

class Graph
{
    public:
        osmium::Box box;
        std::unordered_map<int, osmium::Location> nodeLocations;
        std::unordered_map<int, std::vector<Edge>> data;

        bool TryGetNearestNodeID(osmium::Location worldCursorPos, int& foundNodeID);

    private:
        double minDistanceToCursor = 10;
};