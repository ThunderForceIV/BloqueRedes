#pragma once
#include <SFML\Graphics.hpp>
#include <random>

class PlayerInfo
{
	int playerSalt;
	int playerId;
	std::string name;
	sf::Vector2i position;
	int lives;
public:
	PlayerInfo();
	~PlayerInfo();
};