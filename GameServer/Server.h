#pragma once
#include "UDPSocket.h"
#include "PlayerInfo.h"
#include <map>
#include <iostream> 
class Server
{
	UDPSocket * udpSocket;
	std::map<unsigned short, PlayerInfo> clients;
	
public:
	Server();
	~Server();
	bool IsClientInMap(unsigned short checkPort);
	void ServerLoop();
};

