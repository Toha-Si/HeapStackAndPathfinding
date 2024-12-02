#pragma once
#include <unordered_map>
#include <osmium/osm/box.hpp>
#include <vector>
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

// Тип точки и R-дерева
using Point = bg::model::point<double, 2, bg::cs::cartesian>;
using Value = std::pair<Point, int>;
using RTree = bgi::rtree<Value, bgi::quadratic<16>>;

struct Edge
{
    int nodeToID;
    double distance;
};

class Graph
{
    public:
        //@Todo: remove duplication of the box?
        osmium::Box box;
        std::unordered_map<int, osmium::Location> nodeLocations;
        std::unordered_map<int, std::vector<Edge>> data;
        RTree rtree;

        bool TryGetNearestNodeID(osmium::Location point, int& foundNodeID);
        RTree BuildRTree();
};