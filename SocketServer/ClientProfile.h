
#include <iostream>

#include <string>



using namespace std;

#pragma once
class ClientProfile
{
private:
	string nameClient;


public:
	int clientSocket;
	ClientProfile(string nameClient, int clientSocket);
	~ClientProfile();
};

