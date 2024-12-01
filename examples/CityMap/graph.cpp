#include <graph.hpp>
#include <osmium/geom/haversine.hpp>
#include <limits>

bool Graph::TryGetNearestNodeID(osmium::Location worldCursorPos, int& foundNodeID)
{
    double minDistance = std::numeric_limits<double>::infinity();
    int nearestNodeID = -1;

    for (const auto& [nodeID, edges] : data) 
    {
        const auto& distance = osmium::geom::haversine::distance(osmium::geom::Coordinates(worldCursorPos), 
                                                                 osmium::geom::Coordinates(nodeLocations[nodeID]));
        if (distance < minDistance)
        {
            nearestNodeID = nodeID;
            minDistance = distance;
        }
    }

    if(nearestNodeID == -1)
    {
        return false;
    }

    foundNodeID = nearestNodeID;
    return true;
}