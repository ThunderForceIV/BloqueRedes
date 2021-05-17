#include "Client.h"

Client::Client()
{
	udpSocket = new UDPSocket();
}

Client::~Client()
{
}

void Client::RecievingThread() {//Escucha los paquetes que envia el servidor
	sf::Packet packet;
	sf::IpAddress ip;
	unsigned short port;
	std::string message;
	while (true)
	{
		udpSocket->udpStatus = udpSocket->Receive(packet, ip, port);
		packet >> message;
		if (udpSocket->udpStatus == sf::Socket::Done) {
			std::cout <<std::endl<<"Has recibido "<< message<<"."<<std::endl;
			packet.clear();
		}
		
		else {
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

	this->clientSalt = MAX_64BITS;
	packet << HEADER_PLAYER::HELLO;
	udpSocket->udpStatus = udpSocket->Send(packet, ip, port);
}
void Client::ClientLoop()
{
	std::thread tRecieve(&Client::RecievingThread,this);
	tRecieve.detach();
	std::thread tSend(&Client::SendingThread, this);
	tSend.detach();
	while (true)
	{

	}
}