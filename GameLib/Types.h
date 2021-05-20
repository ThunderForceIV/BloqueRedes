#pragma once
const int MAX_64BITS = 2147483647;
const int SERVER_PORT = 50000;

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
