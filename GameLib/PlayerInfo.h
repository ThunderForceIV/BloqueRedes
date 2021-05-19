#pragma once
#include <SFML\Graphics.hpp>
#include <random>

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
	PlayerInfo();
	~PlayerInfo();
};