#include "Client.h"

Client::Client()
{
	udpSocket = new UDPSocket();
}

Client::~Client()
{
}


void Client::ManageChallenge_Q(sf::Packet &packet, sf::IpAddress &ip, unsigned short &port) {
	packet >> clientSalt;
	packet >> serverSalt;
	packet >> challengeNumber;
	packet.clear();
	packet << HEADER_PLAYER::CHALLENGE_R;
	packet << clientSalt;
	packet << serverSalt;

	
}
int Client::ResolveChallenge(int challengeNumber) {
	return challengeNumber / 2;
}
void Client::RecievingThread() {//Escucha los paquetes que envia el servidor
	sf::Packet packet;
	sf::IpAddress ip;
	unsigned short port;
	std::string message;
	int recieverInt;

	while (true)
	{
		udpSocket->udpStatus = udpSocket->Receive(packet, ip, port);
		packet >> recieverInt;

		switch (recieverInt) {
		case HEADER_SERVER::CHALLENGE_Q:
			ManageChallenge_Q(packet, ip, port);
			packet << ResolveChallenge(challengeNumber);
			port = SERVER_PORT;
			udpSocket->udpStatus = udpSocket->Send(packet, ip, port);

			break;
		case HEADER_SERVER::WELCOME:
			userRegisted = true;

			break;
		case HEADER_SERVER::GENERICMSG_S:
			packet >> message;
			if (udpSocket->udpStatus == sf::Socket::Done) {
				std::cout << std::endl << "Has recibido " << message << "." << std::endl;
				packet.clear();
			}
		}
		
	}
}
void Client::SendingThread() {//Envia los paquetes
	sf::Packet packet;
	sf::IpAddress ip = sf::IpAddress::LocalHost;
	unsigned short port = SERVER_PORT;//Modificar este magic number
	std::string message;
	while (true)
	{
		std::cout << std::endl<< "Escribe el mensaje que quieras enviar: ";
		std::cin >> message;
		packet << message;
		udpSocket->udpStatus = udpSocket->Send(packet, ip, port);

		if (udpSocket->udpStatus == sf::Socket::Done) {
			std::cout <<std::endl<< "Se ha enviado: " << message << std::endl;
			packet.clear();
			message = "";
			//std::cout << std::endl << "Escribe el mensaje que quieras enviar: ";

		}
		else {
			std::cout << "Ha habido un error enviando el paquete";
		}
	}
}
void Client::ConnectionThread() {
	sf::Packet packet;
	sf::IpAddress ip = sf::IpAddress::LocalHost;
	unsigned short port = SERVER_PORT;//Modificar este magic number

	this->clientSalt = rand()%MAX_64BITS;
	packet << HEADER_PLAYER::HELLO;
	packet << clientSalt;
	
	while(true){
	udpSocket->udpStatus = udpSocket->Send(packet, ip, port);
	}
}
void Client::ClientLoop()
{
	std::thread tRecieve(&Client::RecievingThread, this);
	tRecieve.detach();
	std::thread tSend(&Client::SendingThread, this);
	tSend.detach();
	
	while (true)
	{

	}
}