#pragma once

#include "../GameLib/UDPSocket.h"
struct Client
{
	UDPSocket* udpSocket;

public:
	Client();
	~Client();
	void RecievingThread();
	void SendingThread();
	void ClientLoop();
};

