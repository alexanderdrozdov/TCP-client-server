#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <fstream>
#include <vector>


#pragma comment(lib, "ws2_32.lib")
const int bufsize = 512;

SOCKET connection;

void recfile(char* filename);

int main() {

	WSADATA wsaData;
	
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cout << "error" << std::endl;
		return 1;
	}
	
	SOCKADDR_IN addr;
	inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr));
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (connection == INVALID_SOCKET) {
		std::cout << "error sock" << std::endl;
		return 2;
	}
	
	if (connect(connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {

		std::cout << "Connection failed" << std::endl;
		return 3;

	}
    
	else std::cout << "Connected" << std::endl;
	char filename[bufsize];
	char buffer[bufsize];
	std::cout << "enter name of file" << std::endl;
	std::cin >> filename;
	strcpy_s(buffer, filename);

	recfile(buffer);

	closesocket(connection);
	WSACleanup();

	system("pause");
	
	return 0;
}
 
void recfile(char* filename) {

	if (send(connection, filename, bufsize, NULL) == SOCKET_ERROR) {
		std::cout << "send error " << WSAGetLastError() << std::endl;
		return;
	}

	char buffer[bufsize];
	std::ofstream file;
	file.open(filename, std::ios::binary);

	while (true) {
		int recl = recv(connection, buffer, bufsize, NULL);

		if (recl == SOCKET_ERROR) {
			std::cout << "recieve error " << WSAGetLastError() << std::endl;
			file.close();
			closesocket(connection);
			WSACleanup();
			break;
		}

		if (file.bad()) {
			std::cout << "file not exist on server " << std::endl;
			return;
		}

		file.write(buffer, sizeof(char));
		if (file.eof()) break;
	}

	std::cout << "file recieved" << std::endl;
	file.close();
	closesocket(connection);
	WSACleanup();

}