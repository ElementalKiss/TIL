// Tcpip05_op_server.cpp: 콘솔 ?�용 ?�로그램??진입?�을 ?�의?�니??
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string>
#include <WS2tcpip.h>

#define BUF_SIZE 1024
#define OPSZ 4

int Calculate(int opnum, int opnds[], char op);
void ErrorHandling(const std::string message);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hServSock;
    SOCKET hClntSock;
    
    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandling("WSAStartup() error!"); 
    }

    hServSock = socket(PF_INET, SOCK_STREAM, 0);   
    if (hServSock == INVALID_SOCKET) {
        ErrorHandling("socket() error");
    }

    SOCKADDR_IN servAdr, clntAdr;
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));

    if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR) {
        ErrorHandling("bind() error");
    }

    if (listen(hServSock, 5) == SOCKET_ERROR) {
        ErrorHandling("listen() error");	
    }
    
    int clntAdrSize = sizeof(clntAdr);

    int result;
    int opndCnt;
    int recvCnt;
    int recvLen;
    char opinfo[BUF_SIZE];

    for (int i = 0; i < 5; i++) {
        opndCnt = 0;
        hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSize);	
        int iResult = recv(hClntSock, (char*)(&opndCnt), 1, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
        } else if (iResult == 0) {
            printf("Connection closing...\n");
        } else {
            printf("recv failed with error close socket: %d\n", WSAGetLastError());
            closesocket(hClntSock);
        }

        recvLen = 0;
        while ((opndCnt * OPSZ + 1) > recvLen) {
            recvCnt = recv(hClntSock, &opinfo[recvLen], BUF_SIZE - 1, 0);
            recvLen += recvCnt;
        }

        result = Calculate(opndCnt, (int*)opinfo, opinfo[recvLen - 1]);
        printf("Caclulated num: %d close socket\n", result);
        send(hClntSock, (char*)&result, sizeof(result), 0);
        closesocket(hClntSock);
    }

    closesocket(hServSock);
    WSACleanup();
    return 0;
}

int Calculate(int opnum, int opnds[], char op)
{
    int result = opnds[0];

    switch(op) {
    case '+':
        for (int i = 1; i < opnum; i++) result += opnds[i];
        break;
    case '-':
        for (int i = 1; i < opnum; i++) result -= opnds[i];
        break;
    case '*':
        for (int i = 1; i < opnum; i++) result *= opnds[i];
        break;
    }
    return result;
}

void ErrorHandling(const std::string message)
{
    fputs(message.c_str(), stderr);
    fputc('\n', stderr);
    exit(1);
}