#include <algos.hpp>
#include <queue>
#include <limits>
#include <osmium/geom/haversine.hpp>

Path::Path()
{

}

Path::Path(const Graph& graph, std::vector<int> pointIDs)
{
    this->pointIDs = pointIDs;
    SetPathInfo(graph);
}

void Path::SetPathInfo(const Graph& graph)
{
    double totalLength = 0.0;

    for (size_t i = 0; i < pointIDs.size() - 1; ++i) {
        int currentNode = pointIDs[i];
        int nextNode = pointIDs[i + 1];

        // Найти список ребер для текущего узла
        auto it = graph.data.find(currentNode);

        const std::vector<Edge>& edges = it->second;

        // Найти ребро, ведущее к следующему узлу
        for (const Edge& edge : edges)
        {
            if (edge.nodeToID == nextNode)
            {
                totalLength += edge.distance;
                break;
            }
        }

    }

    this->totalLength = totalLength;
}

bool Path::IsValid()
{
    return pointIDs.size() > 1;
}

double Path::GetLength()
{
    return totalLength;
}

int Path::GetNumberOfPoints()
{
    return pointIDs.size();
}

Path Djkstra::FindWay(const Graph& graph, int startNodeID, int endNodeID, bool animated)
{
    std::unordered_map<int, double> distances;  // Расстояния от startNodeID до каждой вершины
    std::unordered_map<int, int> predecessors;
    std::vector<SearchStep> searchSteps;

    for (const auto& pair : graph.data)
    {
        distances[pair.first] = std::numeric_limits<double>::infinity();
    }

    distances[startNodeID] = 0.0;

    // Приоритетная очередь: {расстояние, ID вершины}
    std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<>> pq;
    pq.push({0.0, startNodeID});

    int steps = 0;

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

            if(animated)
            {
                searchSteps.push_back(
                {
                    steps,
                    currentNodeID,
                    nextNodeID
                });
            }

            ++steps;
        }
    }

    std::vector<int> nodeIDs;

    // Путь не найден
    if (distances[endNodeID] == std::numeric_limits<double>::infinity())
    {
        this->path = Path();
        return path;
    }

    for (int at = endNodeID; at != startNodeID; at = predecessors[at])
    {
        nodeIDs.push_back(at);
    }

    nodeIDs.push_back(startNodeID);
    std::reverse(nodeIDs.begin(), nodeIDs.end());

    this->path = Path(graph, nodeIDs);
    this->path.searchSteps = searchSteps; //@Todo: need a new constructor

    return path;
 }

Path Astar::FindWay(const Graph& graph, int startNodeID, int endNodeID, bool animated)
{
    std::unordered_map<int, double> distances;  // Расстояния от startNodeID до каждой вершины
    std::unordered_map<int, int> predecessors;
    std::vector<SearchStep> searchSteps;

    auto heuristic = [&](int nodeID) -> double
    {
        const auto& loc1 = osmium::geom::Coordinates(graph.nodeLocations.at(nodeID));
        const auto& loc2 = osmium::geom::Coordinates(graph.nodeLocations.at(endNodeID));

        return osmium::geom::haversine::distance(loc1, loc2);
    };

    for (const auto& pair : graph.data)
    {
        distances[pair.first] = std::numeric_limits<double>::infinity();
    }

    distances[startNodeID] = 0.0;

    // Приоритетная очередь: {расстояние, ID вершины}
    std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<>> pq;
    pq.push({heuristic(startNodeID), startNodeID});

    int steps = 0;

    while (!pq.empty())
    {
        auto [_, currentNodeID] = pq.top();
        pq.pop();

        if (currentNodeID == endNodeID)
        {
            break;
        }

        for (const Edge& edge : graph.data.at(currentNodeID))
        {
            int nextNodeID = edge.nodeToID;
            double newDistance = distances[currentNodeID] + edge.distance;

            // Если нашли более короткий путь к соседу
            if (newDistance < distances[nextNodeID])
            {
                distances[nextNodeID] = newDistance;
                predecessors[nextNodeID] = currentNodeID;
                pq.push({newDistance + heuristic(nextNodeID), nextNodeID});
            }

            if(animated)
            {
                searchSteps.push_back(
                {
                    steps,
                    currentNodeID,
                    nextNodeID
                });
            }

            ++steps;
        }
    }

    std::vector<int> nodeIDs;

    // Путь не найден
    if (distances[endNodeID] == std::numeric_limits<double>::infinity())
    {
        this->path = Path();
        return path;
    }

    for (int at = endNodeID; at != startNodeID; at = predecessors[at])
    {
        nodeIDs.push_back(at);
    }

    nodeIDs.push_back(startNodeID);
    std::reverse(nodeIDs.begin(), nodeIDs.end());

    this->path = Path(graph, nodeIDs);
    this->path.searchSteps = searchSteps; //@Todo: need a new constructor

    return path;
}

Path DFS::FindWay(const Graph& graph, int startNodeID, int endNodeID, bool animated)
{
    std::vector<int> path;

    return Path(graph, path);
}

Path BFS::FindWay(const Graph& graph, int startNodeID, int endNodeID, bool animated)
{
    std::unordered_map<int, double> distances;  // Расстояния от startNodeID до каждой вершины
    std::unordered_map<int, int> predecessors;
    std::vector<SearchStep> searchSteps;

    distances[startNodeID] = 0.0;
    for (const auto& pair : graph.data)
    {
        distances[pair.first] = std::numeric_limits<double>::infinity();
    }

    std::queue<std::pair<double, int>> q;
    
    q.push({0.0, startNodeID});

    int steps = 0;

    while (!q.empty())
    {
        auto [currentDistance, currentNodeID] = q.front();
        q.pop();

        for (const Edge& edge : graph.data.at(currentNodeID))
        {
            int nextNodeID = edge.nodeToID;
            double newDistance = currentDistance + edge.distance;
    
            if (newDistance < distances[nextNodeID])
            {
                distances[nextNodeID] = newDistance;
                predecessors[nextNodeID] = currentNodeID;
                q.push({newDistance, nextNodeID});
            }

            if(animated)
            {
                searchSteps.push_back(
                {
                    steps,
                    currentNodeID,
                    nextNodeID
                });
            }

            ++steps;
        }
    }

    // Путь не найден
    if (distances[endNodeID] == std::numeric_limits<double>::infinity())
    {
        this->path = Path();
        return path;
    }

    std::vector<int> nodeIDs;

    for (int at = endNodeID; at != startNodeID; at = predecessors[at])
    {
        nodeIDs.push_back(at);
    }

    nodeIDs.push_back(startNodeID);
    std::reverse(nodeIDs.begin(), nodeIDs.end());

    this->path = Path(graph, nodeIDs);
    this->path.searchSteps = searchSteps; //@Todo: need a new constructor

    return path;
}