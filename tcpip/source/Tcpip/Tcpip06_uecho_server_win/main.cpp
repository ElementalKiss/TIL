// Tcpip06_uecho_server_win.cpp: 콘솔 ?�용 ?�로그램??진입?�을 ?�의?�니??
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string>
#include <WS2tcpip.h>

#define BUF_SIZE 30
void ErrorHandling(const std::string message);

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    SOCKET servSock;
    char message[BUF_SIZE];
    int strLen;
    int clntAdrSz;

    SOCKADDR_IN servAdr, clntAdr;
    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandling("WSAStartup() error!"); 
    }

    servSock = socket(PF_INET, SOCK_DGRAM, 0);
    if(servSock == INVALID_SOCKET) {
        ErrorHandling("UDP socket creation error");
    }

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));

    if (bind(servSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR) {
        ErrorHandling("bind() error");
    }

    while(1) {
        clntAdrSz = sizeof(clntAdr);
        strLen = recvfrom(servSock, message, BUF_SIZE, 0, (SOCKADDR*)&clntAdr, &clntAdrSz);
        sendto(servSock, message, strLen, 0, (SOCKADDR*)&clntAdr, sizeof(clntAdr));
    }

    closesocket(servSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(const std::string message)
{
    fputs(message.c_str(), stderr);
    fputc('\n', stderr);
    exit(1);
}