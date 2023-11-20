#pragma once
#include <winsock.h>
#include <iostream>
#include <string>
#include <thread>

class Client
{
public:
	Client();

	void TryToConnect(std::string ipAddress, short port);
	void SendDataToServer(std::string data);
private:
	SOCKET clientSocket;
	sockaddr_in serverAddress;
	std::thread clientThread;

	void ReceiveSignal();
	void HandleSignal(char signal[]);
};

