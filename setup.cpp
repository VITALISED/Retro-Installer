#include "setup.h"

bool hasDownloadedThisFrame = false;
bool finishedDownloading = false;

#include "zip_file.hpp"

DWORD WINAPI DownloadThread(LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);
    std::string url = "https://github.com/SteamRE/DepotDownloader/releases/download/DepotDownloader_2.6.0/DepotDownloader-windows-x64.zip";
    std::string tmp_path = (std::filesystem::temp_directory_path() / "depotdownloader_x64_retro.zip").string();
    DownloadFile(url, tmp_path);
    hasDownloadedThisFrame = true;

    miniz_cpp::zip_file zipfile(tmp_path);
    std::string unpacked_ar = (std::filesystem::temp_directory_path()).string();

    finishedDownloading = true;

    zipfile.extractall(unpacked_ar);

    InstallerState::Get()->NextState();

    return TRUE;
}

void Setup_Frame(ImFont *nms_font_medium, ImFont *nms_font)
{
    ImGui::PushFont(nms_font_medium);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 400);

    const char *preparing_text = "Preparing";
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(preparing_text).x) * 0.5);
    ImGui::Text(preparing_text);
    ImGui::PopFont();

    ImGui::PushFont(nms_font);

    if (!finishedDownloading)
    {
        const char *downloading_depotdownloader_text = "Downloading DepotDownloader...";
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(downloading_depotdownloader_text).x) * 0.5);
        ImGui::Text(downloading_depotdownloader_text);
    }
    else
    {
        const char *extracting_depotdownloader_text = "Extracting Archive...";
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(extracting_depotdownloader_text).x) * 0.5);
        ImGui::Text(extracting_depotdownloader_text);
    }

    ImGui::PopFont();

    if (!hasDownloadedThisFrame)
    {
        hasDownloadedThisFrame = true;
        CreateThread(NULL, NULL, DownloadThread, NULL, NULL, NULL);
    }
}