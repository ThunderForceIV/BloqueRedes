#include "Server.h"

Server::Server()
{
	udpSocket = new UDPSocket();

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

void Server::ServerLoop()
{

	udpSocket->Bind(50000);
	sf::Packet packet;
	sf::IpAddress ip;
	unsigned short port;
	while (true)
	{
		udpSocket->udpStatus = udpSocket->Receive(packet, ip, port);
		if (udpSocket->udpStatus == sf::Socket::Done) {
			if (!this->IsClientInMap(port)) {
				PlayerInfo playerInfo;
				clients.insert(std::pair<unsigned int, PlayerInfo>(port, playerInfo));
				for (std::map<unsigned short, PlayerInfo>::iterator it = clients.begin();it != clients.end();it++) {
					udpSocket->Send(packet, ip, port);
					
				}
			}
		}
		else {
			std::cout << "Ha habido un error recibiendo el paquete";
		}


	}
}

