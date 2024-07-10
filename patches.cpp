#include "patches.h"
#include "child_process.h"
#include "state.h"

bool createdThread = false;
const char *currentStatus = "";

void WriteSteamAppIDTxt(std::string &filepath)
{
    FILE *numbersFile = fopen(filepath.c_str(), "w");
    if (numbersFile)
    {
        const char *numbers = "275850";
        size_t written = fwrite(numbers, 1, strlen(numbers), numbersFile);
        if (written != strlen(numbers))
        {
            perror("Failed to write to file");
            fclose(numbersFile);
        }
        fclose(numbersFile);
    }
    else
    {
        perror("Failed to open steam_appid.txt file");
    }
}

DWORD WINAPI FinaliseThread(LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);

    std::string cmd;

    if (InstallerState::Get()->DoInitial())
    {
        // 0x1331AD8 "\HelloGames\NMS\"L
        // 0x13118B4 "prod"
        currentStatus = "Patching Initial Release";

        WriteSteamAppIDTxt(std::string(InstallerState::Get()->GetDownloadLocation()).append("Release/Binaries/steam_appid.txt"));
    }

    if (InstallerState::Get()->DoFoundations())
    {
        currentStatus = "Patching Foundations";
        WriteSteamAppIDTxt(std::string(InstallerState::Get()->GetDownloadLocation()).append("Foundations/Binaries/steam_appid.txt"));
    }

    if (InstallerState::Get()->DoPathFinder())
    {
        // 0x16F8CA0 "\HelloGames\NMS\"

        currentStatus = "Patching Path Finder";
        cmd = std::format("Steamless.CLI.exe \"{}\"", std::string(InstallerState::Get()->GetDownloadLocation()).append("Path Finder/Binaries/NMS.exe"));
        ChildProcessHandler proc(cmd);

        while (!std::filesystem::exists(std::string(InstallerState::Get()->GetDownloadLocation()).append("Path Finder/Binaries/NMS.exe.unpacked.exe")))
        {
            Sleep(200);
        }

        std::string originalFilePath = std::string(InstallerState::Get()->GetDownloadLocation()).append("Path Finder/Binaries/NMS.exe");
        if (remove(originalFilePath.c_str()) != 0)
        {
            perror("Failed to delete original NMS.exe file");
            return false;
        }

        std::string unpackedFilePath = std::string(InstallerState::Get()->GetDownloadLocation()).append("Path Finder/Binaries/NMS.exe.unpacked.exe");
        if (rename(unpackedFilePath.c_str(), originalFilePath.c_str()) != 0)
        {
            perror("Failed to rename unpacked NMS.exe file");
            return false;
        }

        char bytes[] = {0x5C, 0x00, 0x48, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x6F, 0x00, 0x47, 0x00, 0x61, 0x00,
                        0x6D, 0x00, 0x65, 0x00, 0x73, 0x00, 0x5C, 0x00, 0x31, 0x00, 0x5F, 0x00, 0x32, 0x00, 0x34, 0x00,
                        0x5C, 0x00};

        WriteSteamAppIDTxt(std::string(InstallerState::Get()->GetDownloadLocation()).append("Path Finder/Binaries/steam_appid.txt"));
    }

    if (InstallerState::Get()->DoAtlasRises())
    {
        currentStatus = "Patching Atlas Rises";
        cmd = std::format("Steamless.CLI.exe \"{}\"", std::string(InstallerState::Get()->GetDownloadLocation()).append("Atlas Rises/Binaries/NMS.exe"));
        ChildProcessHandler proc(cmd);
        WriteSteamAppIDTxt(std::string(InstallerState::Get()->GetDownloadLocation()).append("Atlas Rises/Binaries/steam_appid.txt"));

        while (!std::filesystem::exists(std::string(InstallerState::Get()->GetDownloadLocation()).append("Atlas Rises/Binaries/NMS.exe.unpacked.exe")))
        {
            Sleep(200);
        }

        std::string originalFilePath = std::string(InstallerState::Get()->GetDownloadLocation()).append("Atlas Rises/Binaries/NMS.exe");
        if (remove(originalFilePath.c_str()) != 0)
        {
            perror("Failed to delete original NMS.exe file");
            return false;
        }

        std::string unpackedFilePath = std::string(InstallerState::Get()->GetDownloadLocation()).append("Atlas Rises/Binaries/NMS.exe.unpacked.exe");
        if (rename(unpackedFilePath.c_str(), originalFilePath.c_str()) != 0)
        {
            perror("Failed to rename unpacked NMS.exe file");
            return false;
        }
    }

    InstallerState::Get()->NextState();

    return TRUE;
}

void Patches_Frame(ImFont *nms_font_medium, ImFont *nms_font)
{
    ImGui::PushFont(nms_font_medium);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 400);

    const char *preparing_text = "Finalising";
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(preparing_text).x) * 0.5);
    ImGui::Text(preparing_text);
    ImGui::PopFont();

    ImGui::PushFont(nms_font);

    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(currentStatus).x) * 0.5);
    ImGui::Text(currentStatus);

    ImGui::PopFont();

    if (!createdThread)
    {
        createdThread = true;
        CreateThread(NULL, NULL, FinaliseThread, NULL, NULL, NULL);
    }
}