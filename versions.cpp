#include "versions.h"

#include <imgui.h>
#include <imgui_internal.h>

bool IsInitialVersionButtonToggled = false;
bool IsFoundationsVersionButtonToggled = false;
bool IsPathfinderVersionButtonToggled = false;
bool IsAtlasRisesVersionButtonToggled = false;

// style.Colors[ImGuiCol_Button] = ImVec4(0.042, 0.09, 0.08, 0.1);
// style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.042, 0.09, 0.08, 0.3);
// style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.042, 0.09, 0.08, 0.2);

bool ToggleButtonWithImageAndText(const char *label, ImVec2 size, ImTextureID user_texture_id, ImVec2 user_image_size, bool &toggle_state)
{
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext &g = *GImGui;
    const ImGuiStyle &style = g.Style;

    ImVec2 textSize = ImGui::CalcTextSize(label, NULL, true);
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 itemSize(size.x, size.y);
    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(itemSize, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, 0))
        return false;

    // Draw the image background
    window->DrawList->AddImage(user_texture_id, bb.Min, bb.Max);

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, ImGui::GetID(label), &hovered, &held, ImGuiButtonFlags_PressedOnClick);

    if (!toggle_state)
    {
        if (hovered)
            ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_ButtonHovered), true);
        if (pressed)
            ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_ButtonHovered), true);
        if (!hovered && !pressed)
            ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_Button), true);
    }
    else
    {
        ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_ButtonHovered), true);
    }

    ImGui::RenderNavHighlight(bb, ImGui::GetID(label));
    ImGui::RenderFrameBorder(bb.Min, bb.Max);

    // Calculate the position for the text, centered towards the bottom of the button
    float textX = pos.x + (size.x - textSize.x) * 0.5f;
    float textY = pos.y + size.y - textSize.y - style.FramePadding.y; // Adjust for padding

    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    ImU32 color_start = IM_COL32(0, 0, 0, 0);    // Red
    ImU32 color_end = IM_COL32(50, 50, 50, 230); // Blue

    if (!hovered && !toggle_state)
        draw_list->AddRectFilledMultiColor(bb.Min, bb.Max, color_start, color_start, color_end, color_end);

    window->DrawList->AddText(ImVec2(textX, textY), ImGui::GetColorU32(ImGuiCol_Text), label);

    if (pressed)
        toggle_state = !toggle_state;

    return pressed;
}

void VersionPicker_Frame(ImFont *nms_medium_font, ImTextureID initial_release_image_texture, ImTextureID pathfinder_image_texture, ImTextureID foundations_image_texture, ImTextureID atlas_rises_image_texture, ImVec2 image_size)
{
    ImGui::PushFont(nms_medium_font);

    ImGui::SetNextWindowContentSize(ImVec2(400, 0));
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 800) * 0.5f);
    ImGui::BeginChild("VersionPicker", ImVec2(ImGui::GetContentRegionAvail().x, 0), false, ImGuiWindowFlags_NoDecoration);

    ImVec2 buttonSize(200, 400);

    ToggleButtonWithImageAndText("Initial Release", buttonSize, initial_release_image_texture, image_size, IsInitialVersionButtonToggled);
    ImGui::SameLine();
    ToggleButtonWithImageAndText("Foundations", buttonSize, foundations_image_texture, image_size, IsFoundationsVersionButtonToggled);
    ImGui::SameLine();
    ToggleButtonWithImageAndText("Pathfinder", buttonSize, pathfinder_image_texture, image_size, IsPathfinderVersionButtonToggled);
    ImGui::SameLine();
    ToggleButtonWithImageAndText("Atlas Rises", buttonSize, atlas_rises_image_texture, image_size, IsAtlasRisesVersionButtonToggled);

    ImGui::EndChild();

    const char *NextText = "Next";
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(NextText).x - 10) * 0.5);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 100);
    if (ImGui::Button(NextText))
    {
        printf("Next button pressed\n");
    }

    ImGui::PopFont();
}

bool IsInitial()
{
    return IsInitialVersionButtonToggled;
}

bool IsPathFinder()
{
    return IsPathfinderVersionButtonToggled;
}

bool IsFoundations()
{
    return IsFoundationsVersionButtonToggled;
}

bool IsAtlasRises()
{
    return IsAtlasRisesVersionButtonToggled;
}