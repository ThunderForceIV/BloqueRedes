#pragma once
#include "UDPSocket.h"
#include "PlayerInfo.h"
class Server
{
	UDPSocket * udpSocket;
	std::map<unsigned short, PlayerInfo> clients;
	
public:
	Server();
	~Server();
	bool IsClientInMap(unsigned short checkPort);
	void SendMessage2AllClients(unsigned short port, sf::IpAddress ip, sf::Packet packet);
	void ServerLoop();
};

