#include "ClientProfile.h"


#include <iostream>

using namespace std;


ClientProfile::ClientProfile(string nameClient, int clientSocket) :nameClient(nameClient), clientSocket(clientSocket) {

}


ClientProfile::~ClientProfile(){}