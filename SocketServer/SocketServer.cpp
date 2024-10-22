#include <WinSock2.h>
#include <iostream>
#include <string>
#include <cstring>
using namespace std;

const int MAX_CLIENTS = 2;

struct ClientInfo {
    int socket;
    string name;
    bool nameAttempt;
};

ClientInfo clients[MAX_CLIENTS]; 



void broadcastingMessage(int clientIndex,char buff[256]) {
    for (int index = 0; index < MAX_CLIENTS; index++) {
        if (clients[index].socket != 0 && clients[clientIndex].socket != clients[index].socket) {
            send(clients[index].socket, buff, 256, 0);
        }
        else {
            char nothing[] = "\0";
            send(clients[index].socket, nothing, 256, 0);
        }
    }
}



void ProcessNewMessage(int clientIndex) {
    char buff[256 + 1] = { 0 };
    int nRet = recv(clients[clientIndex].socket, buff, 256, 0);
    if (nRet < 0) {
        closesocket(clients[clientIndex].socket);
        cout << endl << clients[clientIndex].name << " disconnected";
        clients[clientIndex].socket = 0;
        clients[clientIndex].name.clear();
        clients[clientIndex].nameAttempt = false;
    }
    else {
        if (clients[clientIndex].nameAttempt) {
            buff[strcspn(buff, "\n")] = '\0'; 
            cout << endl << "New client connected named: " << buff << endl;
            clients[clientIndex].name = buff;
            clients[clientIndex].nameAttempt = false;
        }
        else {
            cout << endl << "The message received from " << clients[clientIndex].name << " is: " << buff;
            string nameSender = clients[clientIndex].name + " sent : ";
            string result = nameSender + buff;
            strcpy_s(buff, result.c_str());
            broadcastingMessage(clientIndex, buff);
        }
        cout << "***********************************************";
    }
}

void setupServerSocket(int port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed with error: " << WSAGetLastError() << endl;
        return;
    }

    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Error creating socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return;
    }

    cout << "Server setup complete, waiting for connections..." << endl;

    while (true) {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(serverSocket, &readSet);

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i].socket != 0) {
                FD_SET(clients[i].socket, &readSet);
            }
        }

        int activity = select(0, &readSet, NULL, NULL, NULL);
        if (activity < 0) {
            cerr << "Error in select(): " << WSAGetLastError() << endl;
            continue;
        }

        if (FD_ISSET(serverSocket, &readSet)) {
            sockaddr_in clientAddr;
            int addrLen = sizeof(clientAddr);
            int newSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
            if (newSocket == INVALID_SOCKET) {
                cerr << "Error accepting connection: " << WSAGetLastError() << endl;
                continue;
            }

            bool clientAdded = false;
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (clients[i].socket == 0) { 
                    clients[i].socket = newSocket;
                    clients[i].name.clear();
                    clients[i].nameAttempt = true; 
                    send(newSocket, "Got the connection successfully", strlen("Got the connection successfully"), 0);
                    clientAdded = true;
                    break;
                }
            }

            if (!clientAdded) {
                cout << "\nNo space for new connection";
                closesocket(newSocket);
            }
        }

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i].socket != 0 && FD_ISSET(clients[i].socket, &readSet)) {
                ProcessNewMessage(i);
            }
        }
    }

    WSACleanup();
}

int main() {
    setupServerSocket(5001);
    return 0;
}
