#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include "basicChat.h"

#pragma comment(lib, "Ws2_32.lib")

#define PORT "26000"

int main()
{
    DWORD dwThreadIdArray[2];
    HANDLE hThreadArray[2];

    WSADATA wsaData;
    int iResult;

    char id[11];
    int idLen;

    /* Get id */
    printf("Please enter your id (10 characters maximum):\n");
    scanf("%s", &id);
    idLen = strlen(id);
    while (idLen > 10)
    {
        printf("Exceded maximum id size, please enter your id again\n");
        scanf("%s", &id);
        idLen = strlen(id);
    }
    printf("Your id is: %s\n", id);

    /* Initialize Winsock */
    if ((iResult = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
    {
        printf("WSAStartup failed, error: %d\n", iResult);
        return -1;
    }

    /* Thread creation */
    if ((hThreadArray[0] = CreateThread(NULL, 0, chatServer, &id, 0, &dwThreadIdArray[0])) == NULL)
    {
        printf("server CreateThread() failed, error: %d\n", GetLastError());
        return -1;
    }

    if ((hThreadArray[1] = CreateThread(NULL, 0, chatClient, &id, 0, &dwThreadIdArray[1])) == NULL)
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
    char* param = lpParam;
    printf("Server Thread, param: %s\n", param);

    return 0;
}

DWORD WINAPI chatClient(LPVOID lpParam)
{
    char* param = lpParam;
    fprintf(stdout, "Client Thread, param: %s\n", param);

    return 0;
}