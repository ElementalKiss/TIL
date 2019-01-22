// Tcpip01.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
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
	WSADATA	wsaData;
	SOCKET hServSock, hClntSock;		
	SOCKADDR_IN servAddr, clntAddr;		

	int szClntAddr;
	char message[] = "Hello World!";

	if(argc!=2)  {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ErrorHandling("WSAStartup() error!"); 
    }
	
	hServSock=socket(PF_INET, SOCK_STREAM, 0);

    if(hServSock == INVALID_SOCKET) {
		ErrorHandling("socket() error");
    }
  
    const char* serverIp = "127.0.0.1";
	memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    inet_pton(AF_INET, serverIp, &(servAddr.sin_addr.s_addr));
    servAddr.sin_port = htons(atoi(argv[1]));

	if(bind(hServSock, (SOCKADDR*) &servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
		ErrorHandling("bind() error");  
    }
	
	if(listen(hServSock, 5) == SOCKET_ERROR) {
		ErrorHandling("listen() error");
    }

	szClntAddr = sizeof(clntAddr);    	
	hClntSock=accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
	if(hClntSock == INVALID_SOCKET) {
		ErrorHandling("accept() error");  
    }
	
	send(hClntSock, message, sizeof(message), 0);
	closesocket(hClntSock);
	closesocket(hServSock);
	WSACleanup();

	return 0;
}

void ErrorHandling(const std::string message)
{
	fputs(message.c_str(), stderr);
	fputc('\n', stderr);
	exit(1);
}