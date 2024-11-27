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
        Graph graph;
        FMHStatus status = FMHStatus::Idle;
        double distanceCutoff = 5000;

        Graph ReadFile(char* fileName);
};