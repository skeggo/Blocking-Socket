#include <iostream>


#include <WinSock.h>


#define PORT 5001

using namespace std;



int nClientSocket;


struct sockaddr_in srv;


int nameAttempt = 1;




int main() {


	int nRet = 0;


	WSADATA ws;

	if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
		cout << endl << "WSA FAILED";
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		cout << endl << "WSA initialized";;
	}

	int nClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (nClientSocket < 0) {
		cout << endl << "The socket  not opened";
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		cout << endl << "The socket opened " << nClientSocket;
	}

	srv.sin_family = AF_INET;

	srv.sin_port = htons(PORT);

	srv.sin_addr.s_addr = inet_addr("127.0.0.1");


	memset(&(srv.sin_zero), 0, 8);
	

	nRet = connect(nClientSocket, (struct  sockaddr*)&srv, sizeof(srv));

	if (nRet < 0) {
		cout << endl << "Connection failed";
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		cout << endl << "Connected to the client";
		char buffer[255];
		cout << endl << "Type your name:";
		while (1) {
			fgets(buffer, 256, stdin);
			send(nClientSocket, buffer, 256, 0);

			char buffer[256];
			recv(nClientSocket, buffer, 256, 0);
			cout << endl << buffer;
			cout << endl << "Now send next message :";

		}

	}


	return 0;
	}