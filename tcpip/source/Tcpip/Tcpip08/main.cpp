// Tcpip08.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
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
int main(int argc, char *argv[])
{
    WSADATA wsaData;
    struct hostent *host;
    SOCKADDR_IN addr;

    if(argc != 3) {
        printf("Usage : %s <addr> <IP>\n", argv[0]);
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandling("WSAStartup() error!"); 
    }

    {
        printf("----------gethostbyname test----------\n");
        host = gethostbyname(argv[1]);
        if(!host) {
            ErrorHandling("gethost... error");
        }

        printf("Official name: %s \n", host->h_name);
        for (int i = 0; host->h_aliases[i]; i++) {
            printf("Aliases %d: %s \n", i+1, host->h_aliases[i]);
        }

        printf("Address type: %s \n",  (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
        for (int i = 0; host->h_addr_list[i]; i++) {
            printf("IP addr %d: %s \n", i+1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
        }
    }

    {
        printf("----------gethostbyaddr test----------\n");
        memset(&addr, 0, sizeof(addr));
        addr.sin_addr.s_addr=inet_addr(argv[2]);
        host = gethostbyaddr((char*)&addr.sin_addr, 4, AF_INET);
        if (!host) {
            ErrorHandling("gethost... error");
        }
    
        printf("Official name: %s \n", host->h_name);
        for (int i = 0; host->h_aliases[i]; i++) {
            printf("Aliases %d: %s \n", i+1, host->h_aliases[i]);
        }

        printf("Address type: %s \n", (host->h_addrtype == AF_INET)?"AF_INET":"AF_INET6");
        for (int i = 0; host->h_addr_list[i]; i++) {
            printf("IP addr %d: %s \n", i+1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));	
        }
    }

    WSACleanup();
    return 0;
}

void ErrorHandling(const std::string message)
{
    fputs(message.c_str(), stderr);
    fputc('\n', stderr);
    exit(1);
}