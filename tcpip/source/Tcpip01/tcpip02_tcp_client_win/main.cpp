// tcpip02_tcp_client_win.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string>
#include <WS2tcpip.h>

void ErrorHandling(const std::string message);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ErrorHandling("WSAStartup() error!");  
    }
	
	hSocket=socket(PF_INET, SOCK_STREAM, 0);
    if(hSocket == INVALID_SOCKET) {
		ErrorHandling("socket() error");
    }
	
	memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &(servAddr.sin_addr.s_addr));
    servAddr.sin_port = htons(atoi(argv[2]));
 
	if(connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
		ErrorHandling("connect() error!");
    }

    char message[30];
	int strLen = 0;
    int idx = 0, readLen = 0;

    while (readLen = recv(hSocket, &message[idx++], 1, 0)) {
        if (readLen == -1) {
            ErrorHandling("read() error!");
        }

        strLen += readLen;
    }
	
    printf("Message from server: %s \n", message);  
    printf("Functino read call count: %d \n", strLen);

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