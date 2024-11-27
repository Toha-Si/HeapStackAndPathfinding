#include <widgets.hpp>

#include <imgui.h>

void SelectionUI::show()
{
    if (ImGui::Begin("File Dropdown"))
    {
        if (ImGui::Combo("Select file", &selectedFile, files, IM_ARRAYSIZE(files)))
        {
            // Срабатывает при выборе нового элемента
        }
        ImGui::End();
    }

    if (ImGui::Begin("Bound Dropdown"))
    {
        if (ImGui::Combo("Select bound", &selectedBound, boundNames, IM_ARRAYSIZE(boundNames)))
        {
            // Срабатывает при выборе нового элемента
        }
        ImGui::End();
    }
}

