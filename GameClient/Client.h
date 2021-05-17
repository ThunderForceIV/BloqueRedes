#pragma once

#include "../GameLib/UDPSocket.h"
struct Client
{
	UDPSocket* udpSocket;

public:
	int serverSalt;
	int clientSalt;
	Client();
	~Client();
	void RecievingThread();
	void SendingThread();
	void ConnectionThread();
	void ClientLoop();
};

