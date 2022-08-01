#undef UNICODE

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "basicChat.h"

#pragma comment(lib, "Ws2_32.lib")

int main()
{
    DWORD serverThreadParam, clientThreadParam;
    DWORD dwThreadIdArray[2];
    HANDLE hThreadArray[2];

    WSADATA wsaData;
    int iResult;

    /* Initialize Winsock */
    if ((iResult = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
    {
        printf("WSAStartup failed, error: %d\n", iResult);
        return -1;
    }

    /* Thread creation */
    serverThreadParam = 1;
    if ((hThreadArray[0] = CreateThread(NULL, 0, chatServer, &serverThreadParam, 0, &dwThreadIdArray[0])) == NULL)
    {
        printf("server CreateThread() failed, error: %d\n", GetLastError());
        return -1;
    }

    clientThreadParam = 2;
    if ((hThreadArray[1] = CreateThread(NULL, 0, chatClient, &clientThreadParam, 0, &dwThreadIdArray[1])) == NULL)
    {
        printf("client CreateThread() failed, error: %d\n", GetLastError());
        return -1;
    }

    WaitForMultipleObjects(2, hThreadArray, TRUE, INFINITE);

    /* Thread finalization */
    if (CloseHandle(hThreadArray[0]) == 0)
    {
        printf("server CloseHandle() failed, error: %s\n", GetLastError());
        return -1;
    }
    if (CloseHandle(hThreadArray[1]) == 0)
    {
        printf("client CloseHandle() failed, error: %s\n", GetLastError());
        return -1;
    }
    printf("Exited chat successfully\n");

    return 0;
}

DWORD WINAPI chatServer(LPVOID lpParam)
{
    int param = *(int *)lpParam;
    printf("Server Thread, param: %d\n", param);
    return 0;
}

DWORD WINAPI chatClient(LPVOID lpParam)
{
    int param = *(int *)lpParam;
    fprintf(stdout, "Client Thread, param: %d\n", param);
    return 0;
}