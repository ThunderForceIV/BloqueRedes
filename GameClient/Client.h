#pragma once

#include "../GameLib/UDPSocket.h"
struct Client
{
	UDPSocket* udpSocket;
	
public:
	bool protocolConnected = false, dungeonCreated = false;

	int serverSalt;
	int clientSalt;
	int challengeNumber;
	bool userRegisted = false;
	bool firstTimeBro = false;
	float rndPacketLoss;

	//Mutex Client
	std::mutex clientMtx;

	//Position Player
	Vector2D position;

	//Acumulation
	Vector2D acumulationPosition;
	std::vector<Accumulation>accumulationVector;
	
	Client();
	~Client();
	void ManageChallenge_Q(sf::Packet& packet, sf::IpAddress& ip, unsigned short& port);
	int ResolveChallenge(int challengeNumber);
	void manageCriticalPackage(sf::Packet& packet);
	void ManageWelcome(sf::Packet& packet);
	void RecievingThread();
	void SendHello();
	void SendingThread();
	void DrawDungeon();
	void ConnectionThread();
	void ClientLoop();
	void UpdateDungeonPosition(int x, int y);
	void SendAcumulationPackets();
};

