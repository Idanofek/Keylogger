#pragma once
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <queue>

#define DEFAULT_BUFFLEN 1024
#define DEFAULT_PORT 5898

/* Need to link with Ws2_32.lib */
#pragma comment (lib, "Ws2_32.lib")
using namespace std;
class Server
{
	public:

		Server(string address, int port = DEFAULT_PORT);
		void start(string address, int port = DEFAULT_PORT);
		bool onConnect(SOCKET ClientSocket);
		SOCKET getSocket() const;
		inline string get_time() const;
		sockaddr_in get_server_info() const;
		~Server();

	private:

		queue<string> cmnds;
		WSADATA wsaData;
		struct addrinfo *result = NULL;
		struct sockaddr_in hints;
		SOCKET ConnSocket;
		char recvbuf[DEFAULT_BUFFLEN];

};