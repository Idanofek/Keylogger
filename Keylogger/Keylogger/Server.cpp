#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <string>
#include <iostream>
#include <thread>
#include "Server.h"

/* Need to link with Ws2_32.lib */
#pragma comment (lib, "Ws2_32.lib")

/* Servers default constructor */
Server::Server(string address, int port) {
	thread srv(&Server::start, this, address, port);
	srv.detach();
	Sleep(1000);
}

void Server::start(string address, int port)
{
	system("color A");

	int iResult = 0;
	int iSendResult = 0;

	/* Initialize Winsock */
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("\n%s------> WSAStartup failed with error: %d\n", get_time(), iResult);
		WSACleanup();
		system("pause");
		exit(0);
	}

	cout << get_time() << "---------> Initializing server on port " << port << endl;

	/* Create a SOCKET for connecting to server */
	ConnSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnSocket == INVALID_SOCKET) {
		printf("\n%s------> Socket failed with error: %ld\n", get_time(), WSAGetLastError());
		WSACleanup();
		system("pause");
		exit(0);
	}
	cout << get_time() << "---------> Opening new socket " << endl;

	memset(&hints, 0, sizeof(struct sockaddr_in));
	hints.sin_family = AF_INET;
	hints.sin_port = htons(port); /* Port to listen */
	hints.sin_addr.s_addr = inet_addr("127.0.0.1");

	int iOptVal = 0;
	int iOptLen = sizeof(int);
	iResult = getsockopt(ConnSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&iOptVal, &iOptLen);

	iResult = connect(ConnSocket, (struct sockaddr*)&hints, sizeof(hints));
	if (iResult == SOCKET_ERROR)
	{
		printf("\n%s------> Socket failed with error: %ld\n", get_time(), WSAGetLastError());
		WSACleanup();
		system("pause");
		exit(0);
	}
	

	do
	{
		iResult = send(ConnSocket, "OK", sizeof("OK"), NULL);
		if (iResult > 0)
		{

			iResult = recv(ConnSocket, recvbuf, sizeof(recvbuf), NULL);
			if(iResult > 0)
			{
				//cmnds.push(string(recvbuf));  // Push command to commands queue
				//cout << recvbuf << endl;
				cout << string(recvbuf) << endl;
			}
			else
			{

			}
		}
		else if (iResult == 0)
		{
			std::cout << "empty" << std::endl;
		}
		else 
		{

		}

		//cout << cmnds.front() << endl;

		//cout << get_time() << "---------> Connected to server!" << endl;
		//iResult = recv(ConnSocket, recvbuf, sizeof(recvbuf), NULL);
		//if (iResult > 0)
		//{
		//	cmnds.push(string(recvbuf));  // Push command to commands queue
		//	iResult = send(ConnSocket, "OK", sizeof(recvbuf), NULL);
		//	if (iResult > 0)
		//	{
		//		continue;
		//	}
		//	else
		//	{

		//	}
		//}
		//else if (iResult == 0)
		//{

		//}
		//else 
		//{

		//}

		//cout << cmnds.front() << endl;
		Sleep(1000);
	} while (1);

	freeaddrinfo(result);
}

inline string Server::get_time() const
{
	time_t t = time(0);   // Get current time
	struct tm * now = localtime(&t);
	return ("[" + to_string(now->tm_hour + 1) + ":" + to_string(now->tm_min) + ":" + to_string(now->tm_sec) + "] ");
}

/* Returns server's socket */
SOCKET Server::getSocket() const { return (ConnSocket); }

/* Returns server info socket (IP, Port, etc) */
sockaddr_in Server::get_server_info() const { return (hints); }

Server::~Server() { closesocket(ConnSocket); }
