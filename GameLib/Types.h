
#pragma once
const int MAX_64BITS = 2147483647;
const int SERVER_PORT = 50000;
const int PLAYER_DESCONNECTION = 30;
const int SERVER_DESCONNECTION = 30;
const float PERCENT_PACKETLOSS = 0.1f;


enum HEADER_PLAYER
{
	HELLO,
	CHALLENGE_R,
	GENERICMSG_P,
	EXIT,
};

enum HEADER_SERVER
{
	CHALLENGE_Q,
	WELCOME,
	GENERICMSG_S,
};

//struct CriticalPackets
//{
//	unsigned short port;
//	sf::IpAddress ip;
//
//	int local;
//	
//	Timer* rtt = new Timer();
//	std::string message;
//
//};