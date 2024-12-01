#include <widgets.hpp>
#include <iostream>
#include <imgui.h>

//UI IMPLEMENTATION
UI::UI(Application& application) : app(application)
{
    
}

UI::~UI() {}
//SELECTION UI
SelectionUI::SelectionUI(Application& application) : UI(application) 
{
    
}

void SelectionUI::Show()
{
    //Initially it was in a constructor above, but app would randomly segfault¯\_(ツ)_/¯
    if(!app.mapReader.BoundIsSet() && !app.mapReader.FileIsSet())
    {
        app.mapReader.fileName = files[selectedFile];
        app.mapReader.box = bounds[selectedBound];
    }

    ImGui::Begin("File Dropdown");

    if(ImGui::Combo("Select file", &selectedFile, files, IM_ARRAYSIZE(files)))
    {
        app.mapReader.fileName = files[selectedFile];
    }

    if (ImGui::Combo("Select bound", &selectedBound, boundNames, IM_ARRAYSIZE(boundNames), 5))
    {
        app.mapReader.box = bounds[selectedBound];
    }

    if (ImGui::Button("Load"))
    {
        app.shouldLoad = true;
    }

    ImGui::End();
}

//LOADING UI
LoadingUI::LoadingUI(Application& application) : UI(application) 
{

}

void LoadingUI::Show()
{
    ImGui::Begin("Loading map");
    ImGui::Text(StatusToString(app.mapReader.status).data());
    DrawLoadingAnimation(StatusToString(app.mapReader.status).data(), 20.0f, 12, 4.0f, 3.0f);
    ImGui::End();
}

std::string LoadingUI::StatusToString(FMHStatus s)
{
    switch (s)
    {
        case FMHStatus::Ready:   return std::string("Map is ready");
        case FMHStatus::ReadingNodes:   return std::string("Reading input file");
        case FMHStatus::ConstructingGraph: return std::string("Creating map");
        default:      return std::string("Not started");
    }
}

//It is possible that there is no async so this animation might just freeze until everything is loaded
void LoadingUI::DrawLoadingAnimation(const char* label, float radius, int num_segments, float thickness, float speed)
{
    static float start_time = ImGui::GetTime();
    float elapsed_time = ImGui::GetTime() - start_time;

    ImVec2 window_size = ImGui::GetWindowSize();
    ImVec2 window_pos = ImGui::GetWindowPos();

    ImVec2 center = ImVec2(
        window_pos.x + window_size.x / 2,
        window_pos.y + window_size.y / 2
    );

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);

    float angle_offset = speed * elapsed_time;

    for (int i = 0; i < num_segments; i++) {
        float angle = (2 * 3.14159f * i / num_segments) + angle_offset;

        float alpha = 1.0f - ((float)i / num_segments);
        ImU32 col_with_alpha = ImGui::GetColorU32(ImVec4(
            ImGui::ColorConvertU32ToFloat4(col).x,
            ImGui::ColorConvertU32ToFloat4(col).y,
            ImGui::ColorConvertU32ToFloat4(col).z,
            alpha));

        ImVec2 start = ImVec2(
            center.x + radius * cosf(angle),
            center.y + radius * sinf(angle));

        ImVec2 end = ImVec2(
            center.x + (radius - thickness) * cosf(angle),
            center.y + (radius - thickness) * sinf(angle));

        draw_list->AddLine(start, end, col_with_alpha, thickness);
    }

    ImGui::Dummy(ImVec2(radius * 2, radius * 2));
}

//MAP UI
MapUI::MapUI(Application& application) : UI(application)
{
    searchAlgo = new Djkstra();
}

void MapUI::Show()
{
    auto* map = dynamic_cast<MapState*>(app.GetCurrentState());
    app.input->controlCamera = true;

    switch(map->state)
    {
        case MapNavigationState::Roaming:
        {
            DrawWayInfo(map, false);
            break;
        }
        case MapNavigationState::WaypointSelection:
        {
            DrawWayInfo(map, true);
            glm::vec2 mousePos(app.input->lastMouseX, app.input->lastMouseY);
            int nearestNodeID = 0;

            //if (app.mapReader.graph.TryGetNearestNodeID(app.renderer.ScreenToLocation(mousePos, app.mapReader.graph.box), nearestNodeID))
            //{
                //DrawNodeInfo(nearestNodeID);

            //    if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            //    {
            //        //SetWaypoint(nearestNodeID);
            //    }
            //}
            break;
        }
        case MapNavigationState::Pause:
        {
            app.input->controlCamera = false;
            DrawAppNavWindow();
            break;
        }
    }
}

void MapUI::DrawAppNavWindow()
{
    ImGui::Begin("Pathfinding app");
    
    if(ImGui::Button("Resume"))
    {
        //@Todo: save previous state and go back to that state
        //map->state = MapNavigationState::Roaming;
    }

    if(ImGui::Button("Go to main"))
    {

    }

    if(ImGui::Button("Exit"))
    {

    }
    
    ImGui::End();
}

void MapUI::DrawWayInfo(MapState* map, bool isSelecting)
{
    ImGui::Begin("Way info");
    
    if(ImGui::Button("DFS"))
    {
        delete searchAlgo;
        searchAlgo = new DFS();
    }

    ImGui::SameLine();

    if(ImGui::Button("BFS"))
    {
        delete searchAlgo;
        searchAlgo = new BFS();
    }

    ImGui::SameLine();

    if(ImGui::Button("Djkstra"))
    {
        delete searchAlgo;
        searchAlgo = new Djkstra();
    }

    ImGui::SameLine();

    if(ImGui::Button("Astar"))
    {
        delete searchAlgo;
        searchAlgo = new Astar();
    }


    if(startNodeSet)
    {
        //ImGui::Text("Start point: " + startNodeID);
    }

    if(endNodeSet)
    {
        //ImGui::Text("End point: " + endNodeID);
    }

    if(!isSelecting)
    {
        if(ImGui::Button("Choose waypoints"))
        {
            map->state = MapNavigationState::WaypointSelection;
        }
    }
    else
    {
        if(ImGui::Button("Roam map"))
        {
            map->state = MapNavigationState::Roaming;
            ResetWaypoints();
        }

        if(ImGui::Button("Find way"))
        {
            //auto way = searchAlgo->FindWay(app.mapReader.graph, startNodeID, endNodeID);
        }
    }

    ImGui::End();
}

void MapUI::DrawNodeInfo(int ID)
{
    if(ImGui::BeginTooltip())
    {
        ImGui::Text("ssss");
        ImGui::EndTooltip();
    }
}

void MapUI::SetWaypoint(int ID)
{
    if(!startNodeSet)
    {
        startNodeSet = true;
        startNodeID = ID;
        return;
    }

    endNodeSet = true;
    endNodeID = ID;
}

void MapUI::ResetWaypoints()
{
    startNodeSet, endNodeSet = false;
    startNodeID, endNodeID = 0;
}
