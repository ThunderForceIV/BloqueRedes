#pragma once
const int MAX_64BITS = 2147483647;
const int SERVER_PORT = 50000;
const int PLAYER_DESCONNECTION = 30;
const int SERVER_DESCONNECTION = 30;
const float PERCENT_PACKETLOSS = 0.1f;

//SIZE OF WINDOW IN CELLS
const int CELL_WIDTH_WINDOW = 80;
const int CELL_HEIGHT_WINDOW = 60;

const float DUNGEON_SIZE = 10.f;

//SIZE OF WINDOW IN PX
const int PX_WIDTH_WINDOW = 800;
const int PX_HEIGHT_WINDOW = 600;

enum HEADER_PLAYER
{
	HELLO,
	CHALLENGE_R,
	GENERICMSG_P,
	EXIT,
	CRITICALPACKAGE_P,
	MOVE_P,
};

enum HEADER_SERVER
{
	CHALLENGE_Q,
	WELCOME,
	GENERICMSG_S,
	CRITICALPACKAGE_S,
	MOVE_S,
};

enum HEADER_GAMESTATE
{
	MOVE,
};
struct Vector2D {
	int x, y;
};

struct Accumulation {
	Vector2D position;
	int id;


};