#include "steam_auth.h"
#include "child_process.h"

char username[255]{};
char password[255]{};
char steamguard[255]{};

std::string childOutput = "";
std::string currentVersion = std::string("No Man's Sky");
bool completedADownload = false;
ChildProcessHandler *processHandler = NULL;
bool startedButtonPressed = false;

void DownloadDepot(std::string command)
{
    processHandler = new ChildProcessHandler(command);
    do
    {
        childOutput = processHandler->ReadFromChildProcess();
        std::printf(childOutput.c_str(), "\n");
    } while (!childOutput.contains("Disconnected from Steam"));

    delete processHandler;

    completedADownload = true;
}

DWORD WINAPI ChildProcessThread(LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);
    startedButtonPressed = true;
    std::string lsCmd = std::string();

    if (InstallerState::Get()->DoInitial())
    {
        printf("Downloading Initial\n");
        currentVersion = "Initial Release";
        lsCmd = "DepotDownloader.exe -app 275850 -depot 275851 -manifest 7324577403707723494";
        lsCmd.append(std::format(" -dir \"{}\"", std::filesystem::path(InstallerState::Get()->GetDownloadLocation()).append("Release").string().c_str()));
        lsCmd.append(std::format(" -username {} -password {} -remember-password", username, password));

        DownloadDepot(lsCmd);
    }

    if (InstallerState::Get()->DoFoundations())
    {
        printf("Downloading Foundations\n");
        currentVersion = "Foundations";
        lsCmd = "DepotDownloader.exe -app 275850 -depot 275851 -manifest 2123008115602074603";
        lsCmd.append(std::format(" -dir \"{}\"", std::filesystem::path(InstallerState::Get()->GetDownloadLocation()).append("Foundations").string().c_str()));
        lsCmd.append(std::format(" -username {} -password {} -remember-password", username, password));

        DownloadDepot(lsCmd);
    }

    if (InstallerState::Get()->DoPathFinder())
    {
        printf("Downloading Path Finder\n");
        currentVersion = "Path Finder";
        lsCmd = "DepotDownloader.exe -app 275850 -depot 275851 -manifest 3749359456608052294";
        lsCmd.append(std::format(" -dir \"{}\"", std::filesystem::path(InstallerState::Get()->GetDownloadLocation()).append("Path Finder").string().c_str()));
        lsCmd.append(std::format(" -username {} -password {} -remember-password", username, password));

        DownloadDepot(lsCmd);
    }

    if (InstallerState::Get()->DoAtlasRises())
    {
        printf("Downloading Atlas Rises\n");
        currentVersion = "Atlas Rises";
        lsCmd = "DepotDownloader.exe -app 275850 -depot 275851 -manifest 8262658978126728861";
        lsCmd.append(std::format(" -dir \"{}\"", std::filesystem::path(InstallerState::Get()->GetDownloadLocation()).append("Atlas Rises").string().c_str()));
        lsCmd.append(std::format(" -username {} -password {} -remember-password", username, password));

        DownloadDepot(lsCmd);
    }

    InstallerState::Get()->NextState();

    return TRUE;
}

void SteamAuth_Frame(ImFont *nms_font_medium, ImFont *nms_font)
{
    if (childOutput.empty() && (!completedADownload || startedButtonPressed))
    {
        ImGuiStyle &style = ImGui::GetStyle();
        ImGui::PushFont(nms_font);

        const char *username_input_text = "Username";
        const char *password_input_text = "Password";

        float username_input_size = ImGui::CalcTextSize(username_input_text).x + style.FramePadding.x * 2.0f;
        float password_input_size = ImGui::CalcTextSize(password_input_text).x + style.FramePadding.x * 2.0f;
        float avail = ImGui::GetContentRegionAvail().x;
        float off_username = (avail - username_input_size) * 0.5f;
        float off_password = (avail - password_input_size) * 0.5f;

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 300);

        const char *notice1 = "Please enter your Steam Username/Password";
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(notice1).x) * 0.5);
        ImGui::Text(notice1);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        const char *notice2 = "We need this information to download No Man's Sky from Steam. You might need to authorize with Steam Guard.";
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(notice2).x) * 0.5);
        ImGui::Text(notice2);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150) * 0.5);
        ImGui::SetNextItemWidth(150.0f);
        ImGui::InputText("##", username, sizeof(username));

        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150) * 0.5);
        ImGui::SetNextItemWidth(150.0f);
        ImGui::InputText("###", password, sizeof(password), ImGuiInputTextFlags_Password);
        ImGui::PopFont();

        const char *start_text = "Start";
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(start_text).x - 25) * 0.5);
        if (ImGui::Button(start_text))
        {
            if (username && password)
                CreateThread(NULL, NULL, ChildProcessThread, NULL, NULL, NULL);
        }
    }
    else
    {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 350);

        ImGui::PushFont(nms_font_medium);
        std::string downloading_text = std::format("Downloading {}", currentVersion);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(downloading_text.c_str()).x) * 0.5);
        ImGui::Text(downloading_text.c_str());
        ImGui::PopFont();

        ImGui::PushFont(nms_font);

        const char *downloading_subtext = childOutput.c_str();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(downloading_subtext).x) * 0.5);
        ImGui::Text(downloading_subtext);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150) * 0.5);
        ImGui::SetNextItemWidth(150.0f);

        if (childOutput.contains("Please enter"))
        {
            ImGui::InputText("####", steamguard, sizeof(steamguard));
            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Enter").x - 20) * 0.5);
            if (ImGui::Button("Enter"))
            {
                processHandler->WriteToChildProcess(steamguard);
            }
        }

        ImGui::PopFont();
    }
}