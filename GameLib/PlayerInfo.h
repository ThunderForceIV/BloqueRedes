#pragma once
#include <SFML\Graphics.hpp>
#include <random>
#include "Timer.h"
class PlayerInfo
{
	
public:
	
	int playerSalt;
	int playerId;
	int serverSalt;
	int challengeNumber;
	std::string name;
	sf::Vector2i position;
	int lives;
	Timer* lastConnection;
	PlayerInfo();
	~PlayerInfo();
};

struct CriticalPackets
{
	unsigned short port;
	sf::IpAddress ip;
	Timer* timer = new Timer();
	HEADER_GAMESTATE header = HEADER_GAMESTATE::CRITICALPACKAGE;
	int local;
	std::string message;


};