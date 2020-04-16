// Midi2Client.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <windows.h>

int main()
{
    HANDLE pipeHandle = INVALID_HANDLE_VALUE;
    pipeHandle = CreateFileA("\\\\.\\pipe\\Midi2Sandbox",
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if (pipeHandle == INVALID_HANDLE_VALUE)
    {
        printf("Cannot create NamedPipe.");
        return -1;
    }

    bool isExit = false;
    char buffer[256 - 1];
    DWORD bytesWritten;

    while (!isExit)
    {
        printf("Input Message: ");
        std::cin >> buffer;
        
        if (!WriteFile(pipeHandle, buffer, sizeof(buffer), &bytesWritten, nullptr))
        {
            printf("Cannot write NamedPipe.");
            isExit = true;
            continue;
        }
        if (strncmp("End", buffer, sizeof("End")) == 0)
        {
            isExit = true;
            continue;
        }
    }

    CloseHandle(pipeHandle);
    return 0;
}
