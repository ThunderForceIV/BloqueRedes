#pragma once
#include "../GameLib/UDPSocket.h"
#include "../GameLib/PlayerInfo.h"
class Server
{
	int serverSalt;
	UDPSocket* udpSocket;
	std::map<unsigned short, PlayerInfo> clients;
	std::map<unsigned short, PlayerInfo> clientsWaiting;
	std::mutex servermtx;
	int localPacket;
	std::vector<float>averageRtt;
public:
	bool playerCanMove = false;
	sf::Packet packet;
	sf::IpAddress ip;
	unsigned short port;
	std::map<int, CriticalPackets>criticalPackets;
	float AverageRtt();
	Server();
	~Server();
	void fillCriticalMap(int key, std::string message, unsigned short port);
	bool IsClientInMap(unsigned short checkPort);
	void SendMessage2AllClients(std::string message, unsigned short port);
	void ManageHello(sf::Packet& packet, sf::IpAddress& ip, unsigned short& port);
	void ManageChallenge_R(sf::Packet& packet, sf::IpAddress& ip, unsigned short& port);
	int ManageChallenge();
	bool ResolveChallenge(int clientAnswer, int clientQuestion);
	void ManageMove(sf::Packet& packet, unsigned short& port);
	float GetRTT(int key);
	void RecieveClients();
	void checkInactivity();
	void ExitThread();
	

	

	void SendCriticalPackets();


	void manageCriticalPackets(int key, unsigned short port);

	void ServerLoop();
	void SendClientsPositions();
};


