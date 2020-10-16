#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
	// Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsok = WSAStartup(ver, &wsData);
	if (wsok != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't Create a Socket! Quitting" << endl;
		return;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton ....

	bind(listening, (sockaddr*)&hint, sizeof(hint));


	// Tell Winsock the socket is for listening
	listen(listening, SOMAXCONN);

	fd_set master;
	FD_ZERO(&master);
	FD_SET(listening, &master);

	while (true)
	{


		fd_set copy = master;
		cout << "-----------------------" << endl;
		cout << "Number of fd_Size" << copy.fd_count << endl;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		cout << "copy.fd_array[0]" << copy.fd_array[0] << endl;
		cout << "copy.fd_array[1]" << copy.fd_array[1] << endl;
		cout << "copy.fd_array[2]" << copy.fd_array[2] << endl;
		cout << "copy.fd_array[3]" << copy.fd_array[3] << endl;

		cout << "Select initialized socketCount : " << socketCount << endl;

		for (int i = 0; i < socketCount; i++)
		{
			cout << "socketCount" << socketCount << endl;
			cout << "copy.fd_array[i]" << copy.fd_array[i] << endl;
			cout << "value of i : " << i << endl;
			SOCKET sock = copy.fd_array[i];
			cout << "Value of Listening" << listening << endl;
			if (sock == listening)
			{
				cout << "[In listening statement]" << endl;
				// Accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				// add the new connection to the list of connected clients
				FD_SET(client, &master);
				// Send a welcome message to the connected client
				string welcomeMsg = "Welcome to the Awesome Chat Server!";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
				// TODO: Broadcast we have a new connection
			}
			else
			{	
				cout << "[in else statement]value of i : " << i << endl;
				char buf[4096];
				ZeroMemory(buf, 4096);

				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					// Send message to other clients, and definiately Not  the listening socket
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock)
						{
							send(outSock, buf, bytesIn, 0);
						}
					}
				}
				// Accept a new message
				// Send message to other clients, and definitely not the listening socket
			}



		}

	}

	//Close the socket
	//closesocket(clientSocket);

	//Shutdown winsock

	WSACleanup();





}

