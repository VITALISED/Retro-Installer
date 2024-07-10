#pragma once

#include <Windows.h>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class ChildProcessHandler
{
public:
    ChildProcessHandler(const std::string &cmd) : cmd_(cmd)
    {
        InitializePipes();
        StartChildProcess();
    }

    ~ChildProcessHandler()
    {
        CloseHandle(hChildStdoutRd_);
        CloseHandle(hChildStdoutWr_);
        CloseHandle(hChildStdinRd_);
        CloseHandle(hChildStdinWr_);
    }

    void WriteToChildProcess(const std::string &text)
    {
        DWORD dwBytesWritten;
        WriteFile(hChildStdinWr_, text.c_str(), text.length(), &dwBytesWritten, NULL);
    }

    std::string ReadFromChildProcess()
    {
        char buffer[4096];
        DWORD dwBytesRead;
        if (!ReadFile(hChildStdoutRd_, buffer, sizeof(buffer) - 1, &dwBytesRead, NULL) || dwBytesRead == 0)
            return std::string();
        buffer[dwBytesRead] = '\0';
        return std::string(buffer);
    }

private:
    void InitializePipes()
    {
        // Create pipes for communication
        SECURITY_ATTRIBUTES saAttr = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};

        CreatePipe(&hChildStdoutRd_, &hChildStdoutWr_, &saAttr, 0);
        CreatePipe(&hChildStdinRd_, &hChildStdinWr_, &saAttr, 0);
    }

    void StartChildProcess()
    {
        PROCESS_INFORMATION piProcInfo;
        STARTUPINFO siStartInfo;
        BOOL bSuccess = FALSE;

        ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
        siStartInfo.cb = sizeof(STARTUPINFO);
        siStartInfo.hStdError = hChildStdoutWr_;
        siStartInfo.hStdOutput = hChildStdoutWr_;
        siStartInfo.hStdInput = hChildStdinRd_;
        siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

        fs::path tempPath = fs::temp_directory_path();
        fs::path fullCmd = tempPath / cmd_;

        char lbuf[255]{};
        strcpy_s(lbuf, sizeof(lbuf), fullCmd.string().c_str());

        bSuccess = CreateProcess(NULL,
                                 lbuf,
                                 NULL,
                                 NULL,
                                 TRUE,
                                 CREATE_NO_WINDOW,
                                 NULL,
                                 NULL,
                                 &siStartInfo,
                                 &piProcInfo);

        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);
    }

    // Member variables
    HANDLE hChildStdoutRd_;
    HANDLE hChildStdoutWr_;
    HANDLE hChildStdinRd_;
    HANDLE hChildStdinWr_;
    std::string cmd_;
};