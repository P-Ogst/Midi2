// Midi2Server.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <windows.h>
#include <midici\midi2_MidiCiMessages.h>
#include <midici\midi2_MessageInterpreter.h>

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
        midi2::midici::MessageInterpreter interpreter;
        auto receivedMessageBytes = 0;
        auto receivedMessageType = interpreter.ReadMessageType(buffer, bufferBytes);
        if (receivedMessageType == midi2::midici::MessageType::Discovery)
        {
            auto message = midi2::midici::DiscoveryMessage();
            message.Read(buffer, bufferBytes);
            message.Dump();
        }
        else
        {
            printf("Undefined message.");
            break;
        }

        midi2::midici::ReplyToDiscoveryMessage message = midi2::midici::ReplyToDiscoveryMessage(0u, 0u, 0u, 0u, 0u, 0u, midi2::midici::CiCategorySupportedBitFlag::None, 256u);
        DWORD messageBytes = message.Write(buffer, sizeof(buffer) - 1);
        if (!WriteFile(pipeHandle, buffer, messageBytes, &bufferBytes, nullptr))
        {
            printf("Cannot write NamedPipe.");
            break;
        }
        break;
    }

    FlushFileBuffers(pipeHandle);
    DisconnectNamedPipe(pipeHandle);
    CloseHandle(pipeHandle);
    return 0;
}
