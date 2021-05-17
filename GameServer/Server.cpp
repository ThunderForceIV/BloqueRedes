#include "Server.h"

Server::Server()
{
	this->udpSocket = new UDPSocket();

}

Server::~Server()
{
}

bool Server::IsClientInMap(unsigned short port)
{
	for (std::map<unsigned short, PlayerInfo>::iterator it = clients.begin();it != clients.end();it++) {
		if (it->first == port) {
			return true;
		}
	}
	return false;
}

void Server::SendMessage2AllClients(std::string message, unsigned short port)
{
	sf::Packet packet;
	packet << message;
	udpSocket->Send(packet, sf::IpAddress::LocalHost,port);


	packet.clear();
}
void Server::ServerLoop()
{

	this->udpSocket->Bind(50000);
	sf::Packet packet;
	sf::IpAddress ip;
	unsigned short port;
	std::string pickpocket;

	while (true)
	{
		udpSocket->udpStatus = udpSocket->Receive(packet, ip, port);
		packet >> pickpocket;
		if (udpSocket->udpStatus == sf::Socket::Done) {
			if (!this->IsClientInMap(port)) {
				PlayerInfo playerInfo;
				clients.insert(std::pair<unsigned int, PlayerInfo>(port, playerInfo));
				
			}
			for (std::map<unsigned short, PlayerInfo>::iterator it = this->clients.begin();it != clients.end();it++) {
				SendMessage2AllClients(pickpocket, it->first);

			}

		}
		else {
			std::cout << "Ha habido un error recibiendo el paquete";
		}


	}
}

