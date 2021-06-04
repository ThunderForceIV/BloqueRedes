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
	Vector2D acumulationPosition;
	std::vector<Accumulation>accumulationMovement;
	std::vector<enemy>enemyPositions;
	PlayerInfo();
	~PlayerInfo();

};

struct CriticalPackets
{
	unsigned short port;
	sf::IpAddress ip;
	Timer* timer = new Timer();
	HEADER_PLAYER header = HEADER_PLAYER::CRITICALPACKAGE_P;
	int local;
	std::string message;



};