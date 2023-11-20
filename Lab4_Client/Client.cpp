#include "Client.h"


Client::Client() {
    clientSocket = socket(AF_INET, SOCK_STREAM, NULL);
    if (clientSocket == INVALID_SOCKET)
    {
        WSACleanup();
        throw std::exception("Invalid Socket");
    }
}


void Client::TryToConnect(std::string ipAddress, short port) {
    serverAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    serverAddress.sin_port = htons(port);
    serverAddress.sin_family = AF_INET;

    while (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        Sleep(5000);
    }
    
    clientThread = std::thread(&Client::ReceiveSignal, this);
}

void Client::HandleSignal(char signal[]) {

}

void Client::ReceiveSignal() {
    char signal[32];
    while(true){
        recv(clientSocket, signal, sizeof(signal), 0);
        HandleSignal(signal);
    }
    
    clientThread.join();
}

void Client::SendDataToServer(std::string data) {
    send(clientSocket, data.c_str(), sizeof(data), 0);
}