#pragma once

#include "../GameLib/UDPSocket.h"
struct Client
{
	UDPSocket* udpSocket;

public:
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
	void RecievingThread();
	void SendingThread();
	void ConnectionThread();
	void ClientLoop();
};

