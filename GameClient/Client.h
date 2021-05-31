#pragma once

#include "../GameLib/UDPSocket.h"
struct Client
{
	UDPSocket* udpSocket;

public:
	bool protocolConnected = false;

	int serverSalt;
	int clientSalt;
	int challengeNumber;
	bool userRegisted = false;
	bool firstTimeBro = false;
	float rndPacketLoss;
	Client();
	~Client();
	void ManageChallenge_Q(sf::Packet& packet, sf::IpAddress& ip, unsigned short& port);
	int ResolveChallenge(int challengeNumber);
	void manageCriticalPackage(sf::Packet& packet);
	void RecievingThread();
	void SendHello();
	void SendingThread();
	void ConnectionThread();
	void ClientLoop();
};

