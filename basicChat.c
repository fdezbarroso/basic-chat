#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include "basicChat.h"

#pragma comment(lib, "Ws2_32.lib")

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
    SOCKET serverSockfd, clientSockfd;
    struct sockaddr_in servAddr, aux;
    u_short serverPort;
    socklen_t auxLen;
    int iResult;

    char confirmationMsg[50];

    char *param = lpParam;
    printf("Server Thread, param: %s\n", param);

    /* Create socket */
    serverSockfd = INVALID_SOCKET;
    if ((serverSockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        printf("server socket() failed, error: %ld\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    /* Define server address */
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    // TODO: Change to htons(0)
    servAddr.sin_port = htons(0);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Bind socket */
    if ((iResult = bind(serverSockfd, (struct sockaddr *)&servAddr, sizeof(servAddr))) == SOCKET_ERROR)
    {
        printf("server bind() failed, error: %ld\n", WSAGetLastError());
        closesocket(serverSockfd);
        WSACleanup();
        return -1;
    }

    /* Get server socket information */
    auxLen = sizeof(aux);
    getsockname(serverSockfd, (struct sockaddr *)&aux, &auxLen);
    serverPort = ntohs(aux.sin_port);
    printf("Server port: %u\n", serverPort);

    /* Listen on socket */
    if ((iResult = listen(serverSockfd, 1)) == SOCKET_ERROR)
    {
        printf("server listen() failed, error: %ld\n", WSAGetLastError());
        closesocket(serverSockfd);
        WSACleanup();
        return -1;
    }

    /* Accept connection */
    clientSockfd = INVALID_SOCKET;
    if ((clientSockfd = accept(serverSockfd, NULL, NULL)) == INVALID_SOCKET)
    {
        printf("server accept() failed, error: %ld\n", WSAGetLastError());
        closesocket(serverSockfd);
        WSACleanup();
        return -1;
    }
    // TODO: Show connection information

    /* Send connection confirmation */
    sprintf(confirmationMsg, "%s: Connection stablished!", lpParam);
    if ((iResult = send(clientSockfd, confirmationMsg, sizeof(confirmationMsg), 0)) == SOCKET_ERROR)
    {
        printf("confirmation send() failed, error: %ld\n", WSAGetLastError());
        closesocket(serverSockfd);
        closesocket(clientSockfd);
        WSACleanup();
        return -1;
    }

    /* TODO: START CHAT */

    /* Disconnect */
    closesocket(serverSockfd);
    closesocket(clientSockfd);
    WSACleanup();

    return 0;
}

DWORD WINAPI chatClient(LPVOID lpParam)
{
    SOCKET clientSockfd;
    struct sockaddr_in clientAddr;
    u_short serverPort;
    int iResult;

    char confirmationMsg[50];

    char *param = lpParam;
    fprintf(stdout, "Client Thread, param: %s\n", param);

    /* Create socket */
    clientSockfd = INVALID_SOCKET;
    if ((clientSockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        printf("client socket() failed, error: %ld\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    /* User input port used */
    Sleep(500);
    printf("Please enter the port number you wish to connect to:\n");
    scanf("%u", &serverPort);

    /* Define server address */
    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    // TODO: htons(serverPort)
    clientAddr.sin_port = htons(serverPort);
    // TODO: let user modify IP
    clientAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Connect */
    if ((iResult = connect(clientSockfd, (struct sockaddr *)&clientAddr, sizeof(clientAddr))) == SOCKET_ERROR)
    {
        printf("client connect() failed, error: %ld\n", WSAGetLastError());
        closesocket(clientSockfd);
        WSACleanup();
        return -1;
    }

    iResult = recv(clientSockfd, confirmationMsg, sizeof(confirmationMsg), 0);
    if (iResult < 0)
    {
        printf("confirmation recv() failed, error: %ld\n", WSAGetLastError());
    }
    else if (iResult == 0)
    {
        printf("Connection closed\n");
    }
    else
    {
        /* Print connection confirmation */
        printf("%s\n", confirmationMsg);

        /* TODO: START CHAT */

    }

    /* Disconnect */
    closesocket(clientSockfd);
    WSACleanup();

    return 0;
}