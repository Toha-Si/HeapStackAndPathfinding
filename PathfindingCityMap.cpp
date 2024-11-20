#include <iostream>
#include <cstdint> 
#include <unordered_map>

#include <osmium/io/any_input.hpp>
#include <osmium/util/file.hpp>
#include <osmium/osm/box.hpp>
#include <osmium/visitor.hpp>
#include <osmium/handler.hpp>
#include <osmium/util/memory.hpp>
#include <osmium/geom/haversine.hpp>
#include <osmium/util/progress_bar.hpp>

struct Edge
{
    int nodeToID;
    double distance;
};

std::unordered_map<int, osmium::Location> nodeLocations;
std::unordered_map<int, std::vector<Edge>> graph;

struct NodeHandler : public osmium::handler::Handler
{
    public:
        void node(const osmium::Node& node)
        {
            if (node.location()) 
            {
                nodeLocations[node.id()] = node.location();
            }
        }
};

struct RoadHandler : public osmium::handler::Handler
{
    osmium::Box BOX_BOLSHOY_SOCHI = osmium::Box
    {
        osmium::Location{39.5961, 43.3671},
        osmium::Location{40.2869, 43.6967}
    };

    osmium::Box BOX_SOCHI = osmium::Box
    {
        osmium::Location{39.6795, 43.5361},
        osmium::Location{39.8131, 43.6507}
    };

    void way(const osmium::Way& way)
    {
        const char* highway = way.tags()["highway"];

        if (!highway)
        {
            return;
        }

        bool intersectsBox = false;

        for (const auto& nodeRef : way.nodes())
        {
            auto it = nodeLocations.find(nodeRef.ref());

            if (it != nodeLocations.end()) 
            {
                osmium::Location loc = it->second;

                if (BOX_SOCHI.contains(loc)) 
                {
                    intersectsBox = true;
                }
            }
        }

        if (intersectsBox)
        {
            const auto& nodes = way.nodes();

            for (size_t i = 0; i < nodes.size() - 1; ++i)
            {
                int fromID = nodes[i].ref();
                int toID = nodes[i+1].ref();

                if (nodeLocations.count(fromID) && nodeLocations.count(toID))
                {
                    const auto& distance = osmium::geom::haversine::distance(osmium::geom::Coordinates(nodeLocations[fromID]), 
                                                                             osmium::geom::Coordinates(nodeLocations[ toID ]));

                    graph[fromID].push_back({toID, distance});
                    graph[toID].push_back({fromID, distance});
                }
            }
        }
    }
};

int main(int argc, char** argv)
{
    if (argc != 2) 
    {
        std::cerr << "Usage: " << argv[0] << " OSMFILE\n";
        return 1;
    }

    try
    {
        const osmium::io::File inputFile{argv[1]};

        osmium::io::Reader nodeReader{inputFile, osmium::osm_entity_bits::node};
        NodeHandler nodeHandler;
        osmium::apply(nodeReader, nodeHandler);

        RoadHandler roadHandler;
        osmium::io::Reader roadReader{inputFile, osmium::osm_entity_bits::way};
        osmium::apply(roadReader, roadHandler);

        const osmium::MemoryUsage memory;

        std::cout << "\nMemory used: " << memory.peak() << " MBytes\n";
    }
    catch(const std::exception e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
}