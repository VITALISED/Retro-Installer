#pragma once

#include <algorithm>
#include <string>

enum eState
{
    EState_Picker,
    EState_Setup,
    EState_SteamAuth,
};

class InstallerState
{
public:
    InstallerState()
    {
        this->mState = EState_Picker;
        this->mDownloadLocation = new char[255];
        strcpy_s(this->mDownloadLocation, 255, "C:/Retro NMS/");
    }
    eState CurrentState() { return this->mState; }
    void NextState() { this->mState = (eState)(this->mState + 1); }
    void SetState(eState state) { this->mState = state; }
    char *GetDownloadLocation() { return this->mDownloadLocation; }
    void SetDownloadLocation(char *str)
    {
        std::string location(str);
        std::replace(location.begin(), location.end(), '\\', '/');
        strcpy_s(mDownloadLocation, 255, location.c_str());
    }
    void SetVersions(bool initial, bool foundations, bool pathfinder, bool atlas_rises)
    {
        this->mDoInitial = initial;
        this->mDoFoundations = foundations;
        this->mDoPathFinder = pathfinder;
        this->mDoAtlasRises = atlas_rises;
    }
    bool DoInitial() { return this->mDoInitial; }
    bool DoFoundations() { return this->mDoFoundations; }
    bool DoPathFinder() { return this->mDoPathFinder; }
    bool DoAtlasRises() { return this->mDoAtlasRises; }

    static InstallerState *Get()
    {
        static InstallerState *state;

        if (!state)
            state = new InstallerState();

        return state;
    }

    eState mState;
    bool mDoInitial;
    bool mDoFoundations;
    bool mDoPathFinder;
    bool mDoAtlasRises;
    char *mDownloadLocation;
};