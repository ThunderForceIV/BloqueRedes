#pragma once
#include "../GameLib/UDPSocket.h"
#include "../GameLib/PlayerInfo.h"
class Server
{
	
	
public:
	UDPSocket* udpSocket;
	std::map<unsigned short, PlayerInfo> clients;
	Server();
	~Server();
	bool IsClientInMap(unsigned short checkPort);
	void SendMessage2AllClients(unsigned short port, sf::IpAddress ip, sf::Packet packet);
	void ServerLoop();
};

