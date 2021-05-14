#pragma once
#include "UDPSocket.h"
#include "PlayerInfo.h"
#include <map>

class Server
{
	UdpSocket * udpSocket;

	std::map<unsigned short, PlayerInfo> clients;
public:
	Server();
	~Server();

	void ServerLoop();
};

