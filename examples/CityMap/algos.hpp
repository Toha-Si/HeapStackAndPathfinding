#pragma once
#include <vector>
#include <graph.hpp>

struct SearchStep {
    int stepNumber;
    int fromNodeID; 
    int toNodeID;
};

class Path
{
    public:
        Path();
        Path(const Graph& graph, std::vector<int> pointIDs);
        std::vector<int> pointIDs;
        std::vector<SearchStep> searchSteps;

        bool IsValid();
        double GetLength();
        int GetNumberOfPoints();
    private:
        double totalLength;
        void SetPathInfo(const Graph& graph);
};

class PathfindingAlgorithm
{
    public:
        Path path;
        virtual Path FindWay(const Graph& graph, int startNodeID, int endNodeID, bool animated) = 0;
};

class Djkstra : public PathfindingAlgorithm
{
    public:
        Path FindWay(const Graph& graph, int startNodeID, int endNodeID, bool animated) override;
};

class Astar : public PathfindingAlgorithm
{
    public:
        Path FindWay(const Graph& graph, int startNodeID, int endNodeID, bool animated) override;
};

class DFS : public PathfindingAlgorithm
{
    public:
        Path FindWay(const Graph& graph, int startNodeID, int endNodeID, bool animated) override;
};

class BFS : public PathfindingAlgorithm
{
    public:
        Path FindWay(const Graph& graph, int startNodeID, int endNodeID, bool animated) override;
};