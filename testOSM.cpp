#include <iostream>
#include <cstdint> 

#include <osmium/io/any_input.hpp>
#include <osmium/util/file.hpp>
#include <osmium/util/progress_bar.hpp>
#include <osmium/osm/box.hpp>
#include <osmium/visitor.hpp>
#include <osmium/handler.hpp>
#include <osmium/util/memory.hpp>
#include <unordered_map>
#include <osmium/geom/haversine.hpp>

std::unordered_map<osmium::object_id_type, osmium::Location> node_locations;

struct NodeHandler : public osmium::handler::Handler
{
    public:
        void node(const osmium::Node& node)
        {
            if (node.location()) 
            {
                node_locations[node.id()] = node.location();
            }
        }
};

struct RoadHandler : public osmium::handler::Handler
{
    osmium::Box bbox_bolshoy_sochi = osmium::Box
    {
        osmium::Location{39.5961, 43.3671},
        osmium::Location{40.2869, 43.6967}
    };

    osmium::Box bbox_sochi = osmium::Box
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

        bool intersects_bbox = false;

        for (const auto& node_ref : way.nodes())
        {
            auto it = node_locations.find(node_ref.ref());
            if (it != node_locations.end()) 
            {
                osmium::Location loc = it->second;
                if (bbox_sochi.contains(loc)) 
                {
                    intersects_bbox = true;
                }
            }
        }

        if (intersects_bbox)
        {
            // Вывод информации о дороге
            //std::cout << "Way ID: " << way.id() << "\n";

            // Узлы дороги
            //std::cout << "Nodes:\n";
            for (const auto& node_ref : way.nodes()) 
            {
                //std::cout << "  Node ID: " << node_ref.ref();
                //if (node_locations[node_ref.ref()]) 
                //{
                //    std::cout << " (" << node_locations[node_ref.ref()] << ")";
                //}
                //std::cout << "\n";
            }

            // // Теги дороги
            // std::cout << "Tags:\n";
            // for (const auto& tag : way.tags())
            // {
            //     std::cout << "  " << tag.key() << " = " << tag.value() << "\n";
            // }
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
        const osmium::io::File input_file{argv[1]};

        osmium::io::Reader node_reader{input_file, osmium::osm_entity_bits::node};
        NodeHandler node_handler;
        osmium::apply(node_reader, node_handler);

        RoadHandler road_handler;
        osmium::io::Reader road_reader{input_file, osmium::osm_entity_bits::way};
        osmium::apply(road_reader, road_handler);

        const osmium::MemoryUsage memory;

        std::cout << "\nWays: " << road_handler.ways << std::endl;
        std::cout << "\nMemory used: " << memory.peak() << " MBytes\n";
    }
    catch(const std::exception e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

}