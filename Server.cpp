#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <fstream>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

const int bufsize = 512;

std::vector<SOCKET> connections;

void sendfile(int index);


int main() {

	WSADATA wsaData;
	
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cout << "error" << std::endl;
		return 1;
	}
    
	SOCKADDR_IN server;
	inet_pton(AF_INET, "127.0.0.1", &(server.sin_addr));
	server.sin_port = htons(1111);
	server.sin_family = AF_INET;

	SOCKET listensock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (listensock == INVALID_SOCKET) {
		std::cout << "error sock" << std::endl;
		return 2;
	}

	bind(listensock, (SOCKADDR*)&server, sizeof(server));
	listen(listensock, SOMAXCONN);
	
	int sizeaddr = sizeof(server);
	SOCKET newconnection;

	for (int i = 0; i < 150; ++i) {
	newconnection = accept(listensock, (SOCKADDR*)&server, &sizeaddr);

		if (newconnection == 0) {

			std::cout << "Error" << std::endl;
			return 2;

		}

		else {

			std::cout << "Client connected" << std::endl;
			connections.push_back(newconnection);
			sendfile(i);

		}
	}

	for (auto it = connections.begin(); it != connections.end(); ++it) {

		closesocket(*it);

	}

	WSACleanup();

	return 0;
}


void sendfile(int index) {

	char buffer[bufsize];
	int counter = connections.size();

	if (recv(connections[index], buffer, bufsize, NULL) == SOCKET_ERROR) {
		std::cout << "recieve error " << WSAGetLastError() << std::endl;
		--counter;
		closesocket(connections[index]);
		WSACleanup();
		system("pause");
		return;
	}
	else {

		std::ifstream file;
		file.open(buffer, std::ios::binary);

		std::cout << "file " << buffer << std::endl;

		if (file.is_open() == false) {
			std::cout << "could not open file" << std::endl;
			system("pause");
			return;
		}

		if (send(connections[index], buffer, bufsize, NULL) == SOCKET_ERROR) {
			std::cout << " send error" << WSAGetLastError() << std::endl;
			file.close();
			--counter;
			closesocket(connections[index]);
			WSACleanup();
			system("pause");
			return;
		}

		size_t fsize;
		file.seekg(0, std::ios::end);
		fsize = file.tellg();

		int part = 0;
		file.seekg(std::ios::beg);

		while (!file.eof()) {

			file.read(buffer, 1);
			if (send(connections[index], buffer, bufsize, NULL) == SOCKET_ERROR) {
				std::cout << " send error" << WSAGetLastError() << std::endl;
				file.close();
				--counter;
				closesocket(connections[index]);
				WSACleanup();
				break;
			}
			++part;
			//std::cout << "now send" << part << " of " << fsize << std::endl;
		}

		std::cout << "send is over" << std::endl;
		std::cout << "send " << --part << " of " << fsize << std::endl;
	}
	/*while (counter > 0 && connections[index] != 0) {

		if (recv(connections[index], buffer, bufsize, NULL) == SOCKET_ERROR) {
			std::cout << "recieve error " << WSAGetLastError() << std::endl;
			--counter;
			closesocket(connections[index]);
			WSACleanup();
			system("pause");
			return;
		}
		else {
			std::ifstream file;
			file.open(buffer, std::ios::binary);

			std::cout << "file " << buffer << std::endl;

			if (file.is_open() == false) {
				std::cout << "could not open file" << std::endl;
				system("pause");
				return;
			}

			if (send(connections[index], buffer, bufsize, NULL) == SOCKET_ERROR) {
				std::cout << " send error" << WSAGetLastError() << std::endl;
				file.close();
				--counter;
				closesocket(connections[index]);
				WSACleanup();
				system("pause");
				return;
			}

			size_t fsize;
			file.seekg(0, std::ios::end);
			fsize = file.tellg();

			int part = 0;
			file.seekg(std::ios::beg);

			while (!file.eof()) {

				file.read(buffer, 1);
				if (send(connections[index], buffer, bufsize, NULL) == SOCKET_ERROR) {
					std::cout << " send error" << WSAGetLastError() << std::endl;
					file.close();
					--counter;
					closesocket(connections[index]);
					WSACleanup();
					break;
				}
				++part;
				std::cout << "now send" << part << " of " << fsize << std::endl;
			}


			std::cout << "send is over" << std::endl;
			std::cout << "send " << --part << " of " << fsize << std::endl;
		}
	}*/
}