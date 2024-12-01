#pragma once
#include <vector>
#include <graph.hpp>

class PathfindingAlgorithm
{
    public:
        virtual std::vector<int> FindWay(const Graph& graph, int startNodeID, int endNodeID) = 0;
};

class Djkstra : public PathfindingAlgorithm
{
    public:
        std::vector<int> FindWay(const Graph& graph, int startNodeID, int endNodeID) override;
};

class Astar : public PathfindingAlgorithm
{
    public:
        std::vector<int> FindWay(const Graph& graph, int startNodeID, int endNodeID) override;
};

class DFS : public PathfindingAlgorithm
{
    public:
        std::vector<int> FindWay(const Graph& graph, int startNodeID, int endNodeID) override;
};

class BFS : public PathfindingAlgorithm
{
    public:
        std::vector<int> FindWay(const Graph& graph, int startNodeID, int endNodeID) override;
};