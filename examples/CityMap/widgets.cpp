#include <widgets.hpp>

#include <imgui.h>

void SelectionUI::show()
{
    if (ImGui::Begin("File Dropdown"))
    {
        if (ImGui::Combo("Select file", &selectedFile, files, IM_ARRAYSIZE(files)))
        {
            app.mapReader.fileName = files[selectedFile];
        }
        ImGui::End();
    }

    if (ImGui::Begin("Bound Dropdown"))
    {
        if (ImGui::Combo("Select bound", &selectedBound, boundNames, IM_ARRAYSIZE(boundNames)))
        {
            //if I destroy selection ui, then null reference exception may arise when accessing app.mapReader.box?
            app.mapReader.box = &bounds[selectedBound];
        }
        ImGui::End();
    }
}

void LoadingUI::show()
{
    if(ImGui::Begin("Loading Animation")) 
    {
        ImGui::Text("Loading...");
        DrawLoadingAnimation(StatusToString(app.mapReader.status), 20.0f, 12, 4.0f, 3.0f);
    }

    ImGui::End();
}

char* LoadingUI::StatusToString(FMHStatus s)
{
    switch (s)
    {
        case FMHStatus::Ready:   return "Map is ready";
        case FMHStatus::ReadingNodes:   return "Reading input file";
        case FMHStatus::ConstructingGraph: return "Creating map";
        default:      return "Not started";
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

void MapUI::show()
{

}