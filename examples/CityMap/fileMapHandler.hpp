#include <graph.hpp>
#include <osmium/osm/box.hpp>

enum class FMHStatus
{
    Idle,
    ReadingNodes,
    ConstructingGraph,
    Ready
};
 
class OSMDataReader
{
    public:
        const char* fileName;
        const osmium::Box* box;

        Graph graph;
        FMHStatus status = FMHStatus::Idle;
        double distanceCutoff = 5000;

        Graph ReadFile();

        bool FileIsSet();
        bool BoundIsSet();
};