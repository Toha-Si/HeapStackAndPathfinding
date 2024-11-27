#include <graph.hpp>
#include <glm/glm.hpp>

std::vector<float> Graph::GetVerticesScreenSpace(Renderer& rndr)
{
    std::vector<float> vertices;

    for (const auto& [nodeID, edges] : data) 
    {
        glm::vec2 nodeFrom = rndr.LocationToScreen(nodeLocations[nodeID], box);

        for (const auto& edge : edges) 
        {
            glm::vec2 nodeTo = rndr.LocationToScreen(nodeLocations[edge.nodeToID], box);

            vertices.push_back(nodeFrom.x);
            vertices.push_back(nodeFrom.y);
            vertices.push_back(nodeTo.x);
            vertices.push_back(nodeTo.y);
        }
    }

    return vertices;
}