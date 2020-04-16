// Midi2Server.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <windows.h>

int main()
{
    HANDLE pipeHandle = INVALID_HANDLE_VALUE;
    pipeHandle = CreateNamedPipeA("\\\\.\\pipe\\Midi2Sandbox",
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_WAIT,
        1,
        0,
        0,
        100,
        nullptr);

    if (pipeHandle == INVALID_HANDLE_VALUE)
    {
        printf("Cannot create NamedPipe.\n");
        return -1;
    }

    if (!ConnectNamedPipe(pipeHandle, nullptr))
    {
        printf("Cannot connect to NamedPipe");
        CloseHandle(pipeHandle);
    }

    bool isExit = false;
    char buffer[256];
    DWORD bytesRead;
    while (!isExit)
    {
        if (!ReadFile(pipeHandle, buffer, sizeof(buffer), &bytesRead, nullptr))
        {
            printf("Cannot read NamedPipe");
            isExit = true;
            continue;
        }
        if (strncmp("End", buffer, sizeof("End")) == 0)
        {
            isExit = true;
            continue;
        }
        buffer[bytesRead] = '\0';
        printf("Message: %s\n", buffer);
    }

    FlushFileBuffers(pipeHandle);
    DisconnectNamedPipe(pipeHandle);
    CloseHandle(pipeHandle);
    return 0;
}
