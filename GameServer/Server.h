#pragma once
#include "../GameLib/UDPSocket.h"
#include "../GameLib/PlayerInfo.h"
class Server
{
	int serverSalt;
	UDPSocket* udpSocket;
	std::map<unsigned short, PlayerInfo> clients;
	
public:
	Server();
	~Server();
	bool IsClientInMap(unsigned short checkPort);
	void SendMessage2AllClients(std::string message, unsigned short port);
	void RecieveClients();
	void ServerLoop();
};

