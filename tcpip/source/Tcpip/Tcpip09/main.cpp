// Tcpip09.cpp: 콘솔 ?�용 ?�로그램??진입?�을 ?�의?�니??
//

#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string>
#include <WS2tcpip.h>
#pragma warning(disable:4996)

void ErrorHandling(const std::string message);
void ShowSocketBufSize(SOCKET sock);

int main(int argc, char *argv[])
{
    WSADATA  wsaData;
    SOCKET hSock;
    int sndBuf, rcvBuf, state;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandling("WSAStartup() error!");
    }
	
    hSock = socket(PF_INET, SOCK_STREAM, 0);
    ShowSocketBufSize(hSock);
	
    sndBuf = 1024 * 3;
    rcvBuf = 1024 * 3;

    state = setsockopt(hSock, SOL_SOCKET, SO_SNDBUF, (char*)&sndBuf, sizeof(sndBuf));
    if (state == SOCKET_ERROR) {
	ErrorHandling("setsockopt() error!");
    }

    state = setsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char*)&rcvBuf, sizeof(rcvBuf));
    if (state == SOCKET_ERROR) {
        ErrorHandling("setsockopt() error!");
    }
	
    ShowSocketBufSize(hSock);
    closesocket(hSock);
    WSACleanup();
    return 0;
}

void ShowSocketBufSize(SOCKET sock)
{
    int sndBuf, rcvBuf, state, len;

    len = sizeof(sndBuf);
    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&sndBuf, &len);
    if (state == SOCKET_ERROR) {
        ErrorHandling("getsockopt() error");
    }
	
    len = sizeof(rcvBuf);
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&rcvBuf, &len);
    if (state == SOCKET_ERROR) {
        ErrorHandling("getsockopt() error");
    }
	
    printf("Input buffer size: %d \n", rcvBuf);
    printf("Output buffer size: %d \n", sndBuf);
}

void ErrorHandling(const std::string message)
{
    fputs(message.c_str(), stderr);
    fputc('\n', stderr);
    exit(1);
}
