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

static float GetRandomFloat() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dis(0.f, 1.f);
	return dis(gen);
}
void Client::RecievingThread() {//Escucha los paquetes que envia el servidor
	sf::Packet packet;
	sf::IpAddress ip;
	unsigned short port;
	std::string message;
	int recieverInt;

	while (true)
	{
		rndPacketLoss = GetRandomFloat();
		udpSocket->udpStatus = udpSocket->Receive(packet, ip, port);

		if (rndPacketLoss > PERCENT_PACKETLOSS) {
			packet >> recieverInt;

			switch (recieverInt) {
			case HEADER_SERVER::CHALLENGE_Q:


				ManageChallenge_Q(packet, ip, port);
				packet << ResolveChallenge(challengeNumber);
				std::cout << ResolveChallenge(challengeNumber);

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
		else {
			std::cout << "SE HA PERDIDO EL PAQUETE";
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
		if (userRegisted==false) {
			if (!firstTimeBro) {
				this->clientSalt = rand() % MAX_64BITS;
				packet << HEADER_PLAYER::HELLO;
				packet << clientSalt;
				udpSocket->udpStatus = udpSocket->Send(packet, ip, port);
				std::cout << "1" << std::endl;
				firstTimeBro = true;
			}
		}
		else {
			std::cout << std::endl << "Escribe el mensaje que quieras enviar: ";
			std::cin >> message;
			packet.clear();

			if (message == "exit") {
				packet << HEADER_PLAYER::EXIT;
				packet << message;
				udpSocket->udpStatus = udpSocket->Send(packet, ip, port);
				exit(0);


			}
			else {
				packet << HEADER_PLAYER::GENERICMSG_P;

				packet << message;
				udpSocket->udpStatus = udpSocket->Send(packet, ip, port);
			}
			if (udpSocket->udpStatus == sf::Socket::Done) {
				std::cout << std::endl << "Se ha enviado: " << message << std::endl;
				packet.clear();

				message = "";
				//std::cout << std::endl << "Escribe el mensaje que quieras enviar: ";

			}
			else {
				std::cout << "Ha habido un error enviando el paquete";
			}
		}
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