#pragma once
#include <graph.hpp>

enum class FMHStatus
{
    Idle,
    ReadingNodes,
    ConstructingGraph,
    CreatingRTree,
    Ready
};
 
class OSMDataReader
{
    public:
        char* fileName = nullptr;
        osmium::Box box;

        Graph graph;
        FMHStatus status = FMHStatus::Idle;
        double distanceCutoff = 5000;

        void ReadFile();

        bool FileIsSet();
        bool BoundIsSet();
};