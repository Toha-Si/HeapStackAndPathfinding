#include <graph.hpp>
#include <osmium/geom/haversine.hpp>
#include <limits>
#include <iostream>
// bool Graph::TryGetNearestNodeID(osmium::Location worldCursorPos, int& foundNodeID)
// {
//     double minDistance = std::numeric_limits<double>::infinity();
//     int nearestNodeID = -1;
//     double thresholdDistance = 1000;

//     for (const auto& [nodeID, edges] : data) 
//     {
//         const auto& distance = osmium::geom::haversine::distance(osmium::geom::Coordinates(worldCursorPos), 
//                                                                  osmium::geom::Coordinates(nodeLocations[nodeID]));
//         if (distance < minDistance)
//         {
//             nearestNodeID = nodeID;
//             minDistance = distance;
//         }
//     }

//     if(nearestNodeID == -1 || minDistance > thresholdDistance)
//     {
//        return false;
//     }

//     foundNodeID = nearestNodeID;
//     return true;
// }
bool Graph::TryGetNearestNodeID(osmium::Location point, int& foundNodeID)
{
    if (!point.valid())
    {
        return false;
    }

    Point queryPoint(point.lon(), point.lat());

    std::vector<Value> result;
    rtree.query(bgi::nearest(queryPoint, 1), std::back_inserter(result));

    if (result.empty())
    {
        return false;
    }

    const auto& [nearestPoint, nearestId] = result.front();
    osmium::Location nearestLocation(bg::get<0>(nearestPoint), bg::get<1>(nearestPoint));
    double distance = osmium::geom::haversine::distance(point, nearestLocation);

    if (distance > 100)
    {
        return false;
    }

    foundNodeID = nearestId;

    return true;
}

RTree Graph::BuildRTree() {
    std::vector<Value> values;

    for (const auto& [nodeID, location] : data)
    {
        if (nodeLocations[nodeID].valid())
        {
            values.emplace_back(Point(nodeLocations[nodeID].lon(), nodeLocations[nodeID].lat()), nodeID);
        }
    }

    return RTree(values.begin(), values.end());
}
