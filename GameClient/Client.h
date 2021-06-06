#pragma once

#include "../GameLib/UDPSocket.h"
#include "../GameLib/Timer.h"
struct Client
{
	UDPSocket* udpSocket;
	bool protocolConnected = false, dungeonCreated = false;
	bool gamerunning = true;
	int serverSalt;
	int clientSalt;
	int auxiliarServerSalt;
	int auxiliarClientSalt;
	int challengeNumber;
	bool userRegisted = false;
	bool firstTimeBro = false;
	Timer* serverDisconnected;
	bool timerActivated = false;
	float rndPacketLoss;
	std::vector<enemy>enemyPos;
	//Mutex Client
	std::mutex clientMtx;
	std::string username;

	//Position Player
	Vector2D position;
	//Acumulation
	Vector2D acumulationPosition;
	std::vector<Accumulation>accumulationVector;
public:
	
	
	Client();
	~Client();
	void Username();
	void JoinGame();
	void ManageChallenge_Q(sf::Packet& packet, sf::IpAddress& ip, unsigned short& port);
	int ResolveChallenge(int challengeNumber);
	void manageCriticalPackage(sf::Packet& packet);
	void ManageWelcome(sf::Packet& packet);
	void ManageMovement(sf::Packet& packet);
	void ManageEnemyPos(sf::Packet& packet);
	void ManageDisconnect();
	void ManageResetGame(sf::Packet packet);

	void RecievingThread();
	void SendHello();
	void SendingThread();
	void DrawDungeon();
	void ConnectionThread();
	void ClientLoop();
	void UpdateDungeonPosition(int x, int y);
	void SendAcumulationPackets();
};

