#pragma once

#include "UDPSocket.h"
class Client
{
	UDPSocket* udpSocket;

public:
	Client();
	~Client();
	void RecievingThread();
	void SendingThread();
	void ClientLoop();
};

