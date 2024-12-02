#include <fileMapHandler.hpp>

#include <osmium/io/any_input.hpp>
#include <osmium/util/file.hpp>
#include <osmium/osm/box.hpp>
#include <osmium/visitor.hpp>
#include <osmium/handler.hpp>
#include <osmium/util/memory.hpp>
#include <osmium/geom/haversine.hpp>
#include <osmium/util/progress_bar.hpp>

class NodeHandler : public osmium::handler::Handler
{
    public:
        Graph& graph;

        NodeHandler(Graph &g) : graph(g)
        {
            std::cout << "Initialized Node Handler.\n";
        }

        void node(const osmium::Node& node)
        {
            if (node.location()) 
            {
                if(graph.box.contains(node.location()))
                {
                    graph.nodeLocations[node.id()] = node.location();
                }
            }
        }
};

class RoadHandler : public osmium::handler::Handler
{
    public:
        OSMDataReader& FMH;

        RoadHandler(OSMDataReader &fmh) : FMH(fmh)
        {
            std::cout << "Initialized Road Handler.\n";
        }

        void way(const osmium::Way& way)
        {
            const char* highway = way.tags()["highway"];

            if (!highway)
            {
                return;
            }

            auto& graph = FMH.graph;

            bool intersectsBox = true;

            for (const auto& nodeRef : way.nodes())
            {
                auto it = graph.nodeLocations.find(nodeRef.ref());

                if (it != graph.nodeLocations.end()) 
                {
                    osmium::Location loc = it->second;

                    if (!graph.box.contains(loc)) 
                    {
                        intersectsBox = false;
                        break;
                    }
                }
            }

            if (!intersectsBox)
            {
                return;
            }

            const char* oneway = way.tags()["oneway"];
            bool isOneway = false;
            bool isReversed = false;

            if (oneway) 
            {
                if (std::strcmp(oneway, "yes") == 0 || std::strcmp(oneway, "1") == 0) 
                {
                    isOneway = true;
                } 
                else if (std::strcmp(oneway, "-1") == 0) 
                {
                    isOneway = true;
                    isReversed = true;
                }
            }

            const auto& nodes = way.nodes();

            for (size_t i = 0; i < nodes.size() - 1; ++i)
            {
                int fromID = nodes[i].ref();
                int toID = nodes[i+1].ref();

                if (graph.nodeLocations.count(fromID) && graph.nodeLocations.count(toID))
                {
                    const auto& distance = osmium::geom::haversine::distance(osmium::geom::Coordinates(graph.nodeLocations[fromID]), 
                                                                             osmium::geom::Coordinates(graph.nodeLocations[ toID ]));


                    if(distance > FMH.distanceCutoff)
                    {
                        continue;
                    }

                    if (!isOneway)
                    {
                        graph.data[fromID].push_back({toID, distance});
                        graph.data[toID].push_back({fromID, distance});
                    }
                    else
                    {
                        if(isReversed)
                        {
                            graph.data[toID].push_back({fromID, distance});
                        }
                        else
                        {
                            graph.data[fromID].push_back({toID, distance});
                        }
                    }
                }
            }
        }
};

bool OSMDataReader::FileIsSet()
{
    return fileName != nullptr;
}

bool OSMDataReader::BoundIsSet()
{
    return box.valid();
}

void OSMDataReader::ReadFile()
{
    try
    {
        const osmium::io::File inputFile{fileName};
        graph.box = box;
        osmium::io::Reader nodeReader{inputFile, osmium::osm_entity_bits::node};
        NodeHandler nodeHandler(graph);
        status = FMHStatus::ReadingNodes;
        osmium::apply(nodeReader, nodeHandler);

        RoadHandler roadHandler(*this);
        osmium::io::Reader roadReader{inputFile, osmium::osm_entity_bits::way};
        status = FMHStatus::ConstructingGraph;
        osmium::apply(roadReader, roadHandler);
        
        status = FMHStatus::CreatingRTree;
        graph.rtree = graph.BuildRTree();

        status = FMHStatus::Ready;
    }

    catch(const std::exception e)
    {
        std::cerr << e.what() << '\n';
    }
}