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
			std::cout << "El cliente esta en el mapa" << std::endl;
		}
	}
	return false;
}

void Server::SendMessage2AllClients(unsigned short port,sf::IpAddress ip, sf::Packet packet)
{
	std::string message;
	packet >> message;
	for (std::map<unsigned short, PlayerInfo>::iterator it = this->clients.begin();it != clients.end();it++) {
		udpSocket->Send(packet, ip, port);
		std::cout << "Se ha enviado " << message << " al cliente " << std::endl;
	}
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
		std::cout << pickpocket;
		if (udpSocket->udpStatus == sf::Socket::Done) {
			if (!this->IsClientInMap(port)) {
				std::cout << "ENTRAMOS AQUI"<<std::endl;
				PlayerInfo playerInfo;
				clients.insert(std::pair<unsigned int, PlayerInfo>(port, playerInfo));
				
			}
			SendMessage2AllClients(port, ip, packet);

		}
		else {
			std::cout << "Ha habido un error recibiendo el paquete";
		}


	}
}

