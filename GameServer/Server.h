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
	Timer* gameTime;
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
	bool CheckClientAndServerSalt(unsigned short puerto, int clientSalt, int serverSalt);
	void ManageHello(sf::Packet& packet, sf::IpAddress& ip, unsigned short& port);
	void ManageChallenge_R(sf::Packet& packet, sf::IpAddress& ip, unsigned short& port);
	void FillEnemyOfNewPlayer(unsigned short port, sf::Vector2i position);
	int ManageChallenge();
	bool ResolveChallenge(int clientAnswer, int clientQuestion);
	void FillEnemyToNewPlayer(unsigned short port);
	void SendEnemyPos();
	void ManageMove(sf::Packet& packet, unsigned short& port);
	float GetRTT(int key);
	void RecieveClients();
	void checkInactivity();
	void ExitThread();
	

	

	void SendCriticalPackets();


	void manageCriticalPackets(int key, unsigned short port);

	void DeleteEnemiesInPlayersVectors(unsigned short port);

	void ServerLoop();
	void ModifyEnemyPositions(unsigned short port, sf::Vector2i positions);
	bool CheckIfEnemyIsInPlayerPos(unsigned short port);
	void SendClientsPositions();
};


