#include "versions.h"

#include "state.h"
#include <Windows.h>
#include <codecvt>
#include <format>
#include <imgui.h>
#include <imgui_internal.h>
#include <locale>
#include <shobjidl.h>
#include <string>

bool IsInitialVersionButtonToggled = false;
bool IsFoundationsVersionButtonToggled = false;
bool IsPathfinderVersionButtonToggled = false;
bool IsAtlasRisesVersionButtonToggled = false;

// style.Colors[ImGuiCol_Button] = ImVec4(0.042, 0.09, 0.08, 0.1);
// style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.042, 0.09, 0.08, 0.3);
// style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.042, 0.09, 0.08, 0.2);

std::wstring GetSelectFolder()
{
    // Initialize the COM library
    CoInitialize(nullptr);

    IFileOpenDialog *pFileOpen;

    // Create the FileOpenDialog object
    HRESULT hr = CoCreateInstance(
        CLSID_FileOpenDialog,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pFileOpen));

    if (SUCCEEDED(hr))
    {
        // Set options to pick folders
        DWORD dwOptions;
        hr = pFileOpen->GetOptions(&dwOptions);

        if (SUCCEEDED(hr))
        {
            hr = pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);

            if (SUCCEEDED(hr))
            {
                // Show the dialog
                hr = pFileOpen->Show(nullptr);

                if (SUCCEEDED(hr))
                {
                    IShellItem *pItem;
                    hr = pFileOpen->GetResult(&pItem);

                    if (SUCCEEDED(hr))
                    {
                        PWSTR pszFilePath;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                        if (SUCCEEDED(hr))
                        {
                            std::wstring selectedFolder = pszFilePath;

                            CoTaskMemFree(pszFilePath);
                            pItem->Release();
                            pFileOpen->Release();

                            // Uninitialize the COM library
                            CoUninitialize();

                            return selectedFolder;
                        }
                    }
                }
            }
        }

        pFileOpen->Release();
    }
    CoUninitialize();

    return L"";
}

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

    if (pressed)
        toggle_state = !toggle_state;

    if (!hovered && !pressed && !held && !toggle_state)
        draw_list->AddRectFilledMultiColor(bb.Min, bb.Max, color_start, color_start, color_end, color_end);

    window->DrawList->AddText(ImVec2(textX, textY), ImGui::GetColorU32(ImGuiCol_Text), label);

    return pressed;
}

void VersionPicker_Frame(ImFont *nms_medium_font, ImTextureID initial_release_image_texture, ImTextureID pathfinder_image_texture, ImTextureID foundations_image_texture, ImTextureID atlas_rises_image_texture, ImVec2 image_size)
{
    ImGui::PushFont(nms_medium_font);

    ImGui::SetNextWindowContentSize(ImVec2(825, 0));
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 800) * 0.5f);
    ImGui::BeginChild("VersionPicker", ImVec2(ImGui::GetContentRegionAvail().x, 0), false, ImGuiWindowFlags_NoDecoration);

    ImVec2 buttonSize(200, 400);

    ToggleButtonWithImageAndText("Initial Release", buttonSize, initial_release_image_texture, image_size, IsInitialVersionButtonToggled);
    ImGui::SameLine();
    ToggleButtonWithImageAndText("Foundations", buttonSize, foundations_image_texture, image_size, IsFoundationsVersionButtonToggled);
    ImGui::SameLine();
    ToggleButtonWithImageAndText("Path Finder", buttonSize, pathfinder_image_texture, image_size, IsPathfinderVersionButtonToggled);
    ImGui::SameLine();
    ToggleButtonWithImageAndText("Atlas Rises", buttonSize, atlas_rises_image_texture, image_size, IsAtlasRisesVersionButtonToggled);

    const char *NextText = "Next";

    float buttonWidth = 100.0f;
    float spacing = 10.0f;
    float totalWidth = 2 * buttonWidth + spacing;

    float availableWidth = ImGui::GetContentRegionAvail().x - 8;
    float offset = (availableWidth - totalWidth) / 2.0f;
    if (offset < 0)
        offset = 0; // Prevent negative offset

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
    if (ImGui::Button(NextText, ImVec2(buttonWidth, 30)))
    {
        if (IsInitialVersionButtonToggled || IsFoundationsVersionButtonToggled || IsPathfinderVersionButtonToggled || IsAtlasRisesVersionButtonToggled)
        {
            InstallerState::Get()->SetVersions(IsInitialVersionButtonToggled, IsFoundationsVersionButtonToggled, IsPathfinderVersionButtonToggled, IsAtlasRisesVersionButtonToggled);
            InstallerState::Get()->NextState();
        }
    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + spacing);
    if (ImGui::Button("Browse", ImVec2(buttonWidth, 30)))
    {
        std::wstring location = GetSelectFolder();
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;

        if (!location.empty())
        {
            InstallerState::Get()->SetDownloadLocation(const_cast<char *>(conv.to_bytes(location).c_str()));
        }
    }

    ImGui::EndChild();

    ImGui::PopFont();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 60);
    std::string prefix_text = std::format("Will be installed to: {}", InstallerState::Get()->GetDownloadLocation());
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(prefix_text.c_str()).x) * 0.5);
    ImGui::Text(prefix_text.c_str());
}