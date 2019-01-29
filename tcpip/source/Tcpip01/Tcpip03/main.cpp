// Tcpip03.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string>
#include <WS2tcpip.h>

void ErrorHandling(const std::string message);

int main(int argc, char* argv[])
{
    printf("\n----------------1. endian convert---------------- \n");

    // 1. endian convert
    {
	    WSADATA	wsaData;
	    unsigned short host_port = 0x1234;
	    unsigned short net_port;
	    unsigned long host_addr = 0x12345678;
	    unsigned long net_addr;

	    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		    ErrorHandling("WSAStartup() error!"); 
        }
	
	    net_port = htons(host_port);
	    net_addr = htonl(host_addr);
	
	    printf("Host ordered port: %#x \n", host_port);
	    printf("Network ordered port: %#x \n", net_port);
	    printf("Host ordered address: %#lx \n", host_addr);
	    printf("Network ordered address: %#lx \n", net_addr);
    }

    printf("\n----------------2. inet_addr, inet_ntoa----------------\n");

    // 2. inet_addr, inet_ntoa
	{
		const char* addr = "192.168.0.1";
		unsigned long conv_addr = inet_addr(addr);

		if(conv_addr == INADDR_NONE) {
			printf("Error occured! \n");
        } else {
            printf("Network ordered integer addr: %#lx \n", conv_addr);
        }
	}

	{
		struct sockaddr_in addr;
		char* strPtr;
		char strArr[20];
		
		addr.sin_addr.s_addr = htonl(0x1020304);
		strPtr = inet_ntoa(addr.sin_addr);
		strcpy_s(strArr, strPtr);
		printf("Dotted Decimal Notation3 %s \n", strArr);
	}

    printf("\n----------------3. WSAStringToAddress, WSAAddressToString---------------- \n");

    // 3. WSAStringToAddress, WSAAddressToString
    {
        const char* strAddr = "203.211.218.102:9190";

	    char strAddrBuf[50];
	    SOCKADDR_IN servAddr;
	    int size;

	    WSADATA	wsaData;
	    WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	    size = sizeof(servAddr);
	    WSAStringToAddress(const_cast<char*>(strAddr), AF_INET, NULL, (SOCKADDR*)&servAddr, &size);

	    DWORD bufSize = sizeof(strAddrBuf);
	    WSAAddressToString((SOCKADDR*)&servAddr, sizeof(servAddr), NULL, strAddrBuf, &bufSize);

	    printf("Second Conversion Result: %s \n\n", strAddrBuf);
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