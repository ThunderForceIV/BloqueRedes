#include "PlayerInfo.h"


PlayerInfo::PlayerInfo()
{
	position.x = std::rand() % 10;
	position.y = std::rand() % 10;
	lives = 3;
}

PlayerInfo::~PlayerInfo()
{
}
