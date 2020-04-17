﻿// Midi2Client.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <windows.h>
#include <midici/midi2_MidiCiMessages.h>

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

    char buffer[256];
    DWORD bufferBytes;

    while (true)
    {
        midi2::midici::UniversalSysExMessageBase message = midi2::midici::UniversalSysExMessageBase(midi2::midici::MessageType::Discovery, midi2::midici::DeviceId::MidiPort, 0u, 0u);
        auto messageBytes = message.Write(buffer, sizeof(buffer) - 1);
        if (messageBytes < 0)
        {
            break;
        }

        if (!WriteFile(pipeHandle, buffer, sizeof(buffer) - 1, &bufferBytes, nullptr))
        {
            printf("Cannot write NamedPipe.");
            break;
        }
        if (strncmp("End", buffer, sizeof("End")) == 0)
        {
            break;
        }

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
    }

    CloseHandle(pipeHandle);
    return 0;
}
