#pragma once
#include "../GameLib/UDPSocket.h"
#include "../GameLib/PlayerInfo.h"
class Server
{
	int serverSalt;
	UDPSocket* udpSocket;
	std::map<unsigned short, PlayerInfo> clients;
	std::map<unsigned short, PlayerInfo> clientsWaiting;
	
public:
	sf::Packet packet;
	sf::IpAddress ip;
	unsigned short port;
	Server();
	~Server();
	bool IsClientInMap(unsigned short checkPort);
	void SendMessage2AllClients(std::string message, unsigned short port);
	void ManageHello(sf::Packet& packet, sf::IpAddress& ip, unsigned short& port);
	void ManageChallenge_R(sf::Packet& packet, sf::IpAddress& ip, unsigned short& port);
	int ManageChallenge();
	bool ResolveChallenge(int clientAnswer, int clientQuestion);
	void RecieveClients();
	void ExitThread();
	
	void ServerLoop();
};


