#include "Server.h"

Server::Server()
{
	udpSocket = new UDPSocket();
	
}

void Server::ServerLoop()
{

	udpSocket->Bind(50000);
	sf::Packet packet;
	sf::IpAddress ip;
	unsigned short port;

	while (true)
	{
		udpSocket->Receive(packet, ip, port);

	}

}
