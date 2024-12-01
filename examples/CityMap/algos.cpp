#include <algos.hpp>
#include <queue>
#include <limits>

std::vector<int> Djkstra::FindWay(const Graph& graph, int startNodeID, int endNodeID)
{
    std::unordered_map<int, double> distances;  // Расстояния от startNodeID до каждой вершины
    std::unordered_map<int, int> predecessors;

    for (const auto& pair : graph.data)
    {
        distances[pair.first] = std::numeric_limits<double>::infinity();
    }

    distances[startNodeID] = 0.0;

    // Приоритетная очередь: {расстояние, ID вершины}
    std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<>> pq;
    pq.push({0.0, startNodeID});

    while (!pq.empty())
    {
        auto [currentDistance, currentNodeID] = pq.top();
        pq.pop();

        if (currentNodeID == endNodeID)
        {
            break;
        }

        for (const Edge& edge : graph.data.at(currentNodeID))
        {
            int nextNodeID = edge.nodeToID;
            double newDistance = currentDistance + edge.distance;

            // Если нашли более короткий путь к соседу
            if (newDistance < distances[nextNodeID])
            {
                distances[nextNodeID] = newDistance;
                predecessors[nextNodeID] = currentNodeID;
                pq.push({newDistance, nextNodeID});
            }
        }
    }

    std::vector<int> path;

    if (distances[endNodeID] == std::numeric_limits<double>::infinity())
    {
        // Путь не найден
        return path;
    }

    for (int at = endNodeID; at != startNodeID; at = predecessors[at])
    {
        path.push_back(at);
    }

    path.push_back(startNodeID);
    std::reverse(path.begin(), path.end());

    return path;
 }

std::vector<int> Astar::FindWay(const Graph& graph, int startNodeID, int endNodeID)
{
    std::vector<int> path;

    return path;
}

std::vector<int> DFS::FindWay(const Graph& graph, int startNodeID, int endNodeID)
{
    std::vector<int> path;

    return path;
}

std::vector<int> BFS::FindWay(const Graph& graph, int startNodeID, int endNodeID)
{
    std::vector<int> path;

    return path;
}