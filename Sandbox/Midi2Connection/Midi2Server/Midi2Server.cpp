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
    DWORD bufferBytes;
    while (!isExit)
    {
        if (!ReadFile(pipeHandle, buffer, sizeof(buffer), &bufferBytes, nullptr))
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
        buffer[bufferBytes] = '\0';
        printf("ReceivedMessage: %s\n", buffer);

        printf("Input Message: ");
        std::cin >> buffer;
        if (!WriteFile(pipeHandle, buffer, sizeof(buffer) - 1, &bufferBytes, nullptr))
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

    FlushFileBuffers(pipeHandle);
    DisconnectNamedPipe(pipeHandle);
    CloseHandle(pipeHandle);
    return 0;
}
