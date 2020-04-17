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

    char buffer[256];
    DWORD bufferBytes;
    while (true)
    {
        if (!ReadFile(pipeHandle, buffer, sizeof(buffer), &bufferBytes, nullptr))
        {
            printf("Cannot read NamedPipe");
            break;
        }
        if (strncmp("End", buffer, sizeof("End")) == 0)
        {
            break;
        }
        buffer[bufferBytes] = '\0';
        printf("ReceivedMessage: %s\n", buffer);

        printf("Input Message: ");
        std::cin >> buffer;
        if (!WriteFile(pipeHandle, buffer, sizeof(buffer) - 1, &bufferBytes, nullptr))
        {
            printf("Cannot write NamedPipe.");
            break;
        }
        if (strncmp("End", buffer, sizeof("End")) == 0)
        {
            break;
        }
    }

    FlushFileBuffers(pipeHandle);
    DisconnectNamedPipe(pipeHandle);
    CloseHandle(pipeHandle);
    return 0;
}
