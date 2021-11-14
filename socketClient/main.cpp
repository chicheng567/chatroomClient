#include<WS2tcpip.h>
#include<iostream>
#include<string>
#include<sstream>
#include<thread>
#include<string.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)
void sockStream(SOCKET serverSock, string& message)
{
	fd_set sockSet;
	FD_ZERO(&sockSet);
	FD_SET(serverSock, &sockSet);
	timeval limited = { 0, 0 };
	while (1) {
		fd_set copy = sockSet;
		int selectResult = select(0, &copy, nullptr, nullptr, &limited);
		if (selectResult == 1) {
			char buf[4069];
			recv(sockSet.fd_array[0], buf, 4069, 0);
			cout << buf << endl;
			//server closed condition
			if (!strcmp(buf, "Server clossing!!\n")) {
				message = "\\quit";
				break;
			}
		}
		else if (selectResult < 0) {
			cerr << "Select Error" << endl;
			break;
		}
		else if (message != "" && !selectResult) {
			send(sockSet.fd_array[0], message.c_str(), message.length() + 1, 0);
			message = "";
		}
	}
	cout << "Thread Finished" << endl;
	FD_CLR(serverSock, &sockSet);
	closesocket(serverSock);
	return;
}
int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		cerr << "Dll initialize failed!" << endl;
		return 1;
	}

	SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);

	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_port = htons(1234);
	sockAddr.sin_addr.s_addr = inet_addr("140.117.198.149");
	if (connect(sock, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR) {
		cerr << "Connnected error. Quit!!" << endl;
	}
	string message = "";
	thread socketWork(sockStream, sock, ref(message));

	while (message != "\\quit") {
		string tmp;
		getline(cin, tmp);
		message = tmp;
	}
	socketWork.join();
	WSACleanup();
	return 0;
}