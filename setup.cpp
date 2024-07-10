#include "setup.h"
#include "zip_file.hpp"

bool hasDownloadedThisFrame = false;

DWORD WINAPI DownloadThread(LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);
    hasDownloadedThisFrame = true;

    std::string url = "https://github.com/SteamRE/DepotDownloader/releases/download/DepotDownloader_2.6.0/DepotDownloader-windows-x64.zip";
    std::string tmp_path = (std::filesystem::temp_directory_path() / "depotdownloader_x64_retro.zip").string();
    DownloadFile(url, tmp_path);

    miniz_cpp::zip_file zipfile(tmp_path);
    std::string unpacked_ar = (std::filesystem::temp_directory_path()).string();

    zipfile.extractall(unpacked_ar);

    url = "https://github.com/atom0s/Steamless/releases/download/v3.1.0.5/Steamless.v3.1.0.5.-.by.atom0s.zip";
    tmp_path = (std::filesystem::temp_directory_path() / "Steamless.v3.1.0.5.-.by.atom0s_retro.zip").string();
    DownloadFile(url, tmp_path);

    std::string unpacked_ar2 = (std::filesystem::temp_directory_path()).string();
    miniz_cpp::zip_file zipfile2(tmp_path);

    zipfile2.extractall(unpacked_ar2);

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

    const char *extracting_depotdownloader_text = "Downloading Pre-Requisites";
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(extracting_depotdownloader_text).x) * 0.5);
    ImGui::Text(extracting_depotdownloader_text);

    ImGui::PopFont();

    if (!hasDownloadedThisFrame)
    {
        hasDownloadedThisFrame = true;
        CreateThread(NULL, NULL, DownloadThread, NULL, NULL, NULL);
    }
}