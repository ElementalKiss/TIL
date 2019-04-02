// Tcpip07_file_client_win.cpp: 콘솔 ?�용 ?�로그램??진입?�을 ?�의?�니??
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string>
#include <WS2tcpip.h>
#pragma warning(disable:4996)

#define BUF_SIZE 30
void ErrorHandling(const std::string message);

int main(int argc, char* argv[])
{
    if(argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandling("WSAStartup() error!"); 
    }

    FILE *fp;
    SOCKET hSocket;
    fp = fopen("receive.dat", "wb");
    hSocket = socket(PF_INET, SOCK_STREAM, 0);   

    SOCKADDR_IN servAdr;
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr(argv[1]);
    servAdr.sin_port = htons(atoi(argv[2]));

    connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr));

    char buf[BUF_SIZE];
    int readCnt;
    while((readCnt = recv(hSocket, buf, BUF_SIZE, 0)) != 0){
        fwrite((void*)buf, 1, readCnt, fp);
    }

    puts("Received file data");
    send(hSocket, "Thank you", 10, 0);
    fclose(fp);
    closesocket(hSocket);
    WSACleanup();
    return 0;
}

void ErrorHandling(const std::string message)
{
    fputs(message.c_str(), stderr);
    fputc('\n', stderr);
    exit(1);
}