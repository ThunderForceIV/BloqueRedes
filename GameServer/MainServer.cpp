#pragma once
#include "Server.h"

int main()
{
	srand(time(NULL));

	Server servidor;
	
	servidor.ServerLoop();

	return 0;

}