#pragma once

enum eState
{
    EState_Picker,
};

class InstallerState
{
public:
    InstallerState() { this->mState = EState_Picker; }
    eState CurrentState() { return this->mState; }
    void NextState() { this->mState = (eState)(this->mState + 1); }
    void SetState(eState state) { this->mState = state; }

    static InstallerState *Get()
    {
        static InstallerState *state;

        if (!state)
            state = new InstallerState();

        return state;
    }

    eState mState;
};