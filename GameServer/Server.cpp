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
	packet << HEADER_SERVER::GENERICMSG_S;
	packet << message;

	udpSocket->Send(packet, sf::IpAddress::LocalHost,port);


	packet.clear();
}
void Server::ManageHello(sf::Packet &packet, sf::IpAddress &ip, unsigned short &port) {
	PlayerInfo playerInfo;

	//Recibimos el playerSalt y guardamos informaci�n en el serverSalt
	packet >> playerInfo.playerSalt;
	playerInfo.serverSalt = rand() % MAX_64BITS;
	playerInfo.challengeNumber = ManageChallenge();
	//Insertamos el cliente en una lista de los clientes pendientes.
	clientsWaiting.insert(std::pair<unsigned int, PlayerInfo>(port, playerInfo));

	//Limpiamos el packet
	packet.clear();

	//A�adimos la informaci�n nueva al paquete. 
	packet << HEADER_SERVER::CHALLENGE_Q;
	packet << playerInfo.playerSalt;
	packet << playerInfo.serverSalt;
	packet << playerInfo.challengeNumber;

}
int Server::ManageChallenge() {
	int challengeNumber = -1;
	while (challengeNumber % 2 != 0) {
		challengeNumber = rand() % 100 + 1;
	}
	return challengeNumber;
}
bool Server::ResolveChallenge(int clientAnswer, int clientQuestion) {
	return clientAnswer * 2 == clientQuestion;
}
void Server::ManageChallenge_R(sf::Packet& packet, sf::IpAddress& ip, unsigned short& port) {
	int actualClientSalt;
	int actualServerSalt;
	int clientAnswer;

	for (std::map<unsigned short, PlayerInfo>::iterator it = clientsWaiting.begin();it != clientsWaiting.end();it++) {

		if (it->first == port) {

			packet >> actualClientSalt;
			packet >> actualServerSalt;
			packet >> clientAnswer;
			std::cout << "Player Salt del map: " << it->second.playerSalt << std::endl;
			std::cout << "Player Salt del cliente: " << actualClientSalt << std::endl;

			std::cout << "Server Salt del map: " << it->second.serverSalt << std::endl;
			std::cout << "server Salt del cliente: " << actualServerSalt << std::endl;

			std::cout << "Numero de desafio: " << it->second.challengeNumber << std::endl;
			std::cout << "ClientAnswer: " << clientAnswer << std::endl;

			if (it->second.playerSalt == actualClientSalt && it->second.serverSalt == actualServerSalt && ResolveChallenge(clientAnswer, it->second.challengeNumber)) {
				std::cout << "1sadadadadasdad5" << std::endl;

				packet.clear();
				packet << HEADER_SERVER::WELCOME;
				packet << actualClientSalt;
				packet << actualServerSalt;
				if (!this->IsClientInMap(port)) {
					clients.insert(std::pair<unsigned int, PlayerInfo>(port, it->second));
				}
				udpSocket->udpStatus = udpSocket->Send(packet, ip, port);

			}
		}
	}
}



void Server::RecieveClients() {
	int recieverInt;
	sf::Packet packet;
	sf::IpAddress ip;
	unsigned short port;
	std::cout << "7" << std::endl;
	while (true) {
		udpSocket->udpStatus = udpSocket->Receive(packet, ip, port);
		packet >> recieverInt;
		std::cout << "5" << std::endl;
		switch (recieverInt)
		{
		case HEADER_PLAYER::HELLO:
			//Creamos el serversalt
			//Preguntar donde guardar el client y como
			//packet << HEADER_SERVER::CHALLENGE_Q << serverSalt << clientSalt<<challenge;
			std::cout << "3" << std::endl;
			ManageHello(packet, ip, port);
			//Creamos el challenge�
			packet << ManageChallenge();
			udpSocket->udpStatus = udpSocket->Send(packet, ip, port);
			std::cout << "2" << std::endl;

			break;
		case HEADER_PLAYER::CHALLENGE_R:
			ManageChallenge_R(packet, ip, port);
			
			break;
		default:
			break;
		}

	}

}

void Server::checkInactivity() 
{
	std::list<unsigned short> inactivityCheck;
	std::mutex mtx;
	while (true)
	{
		for (std::map<unsigned short, PlayerInfo>::iterator it = clients.begin();it != clients.end();it++) {
			if (it->second.lastConnection->GetDuration() > PLAYER_DESCONNECTION)
			{
				inactivityCheck.push_back(it->first);
			}
		}
		mtx.lock();
		for (std::list<unsigned short>::iterator it = inactivityCheck.begin(); it != inactivityCheck.end(); ++it) {
			clients.erase(*it);
			std::cout << "Se ha desconectado ";
			for (std::map<unsigned short, PlayerInfo>::iterator it = clients.begin();it != clients.end();it++) {
				//Tenemos que avisar a los dem�s
			}
			

		}
		mtx.unlock();
		inactivityCheck.clear();

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}




}
void Server::ExitThread() {
	std::string message;

	std::cout << std::endl << "Escribe el mensaje que quieras enviar: ";
	std::cin >> message;

	if (message == "exit") {
		packet.clear();

		packet << HEADER_PLAYER::EXIT;
		packet << message;
		udpSocket->udpStatus = udpSocket->Send(packet, ip, port);
		exit(0);


	}
}

void Server::ServerLoop()
{

	this->udpSocket->Bind(50000);

	int recieverInt;
	int auxiliarPlayerSalt;
	int auxiliarServerSalt;
	std::string auxiliarMessage;
	std::cout << "LLEGAMOS DON FERNANDITO";

	std::thread tExit(&Server::ExitThread, this);
	tExit.detach();

	std::thread tCheckInactivity(&Server::checkInactivity, this);
	tCheckInactivity.detach();
	while (true)
	{
		udpSocket->udpStatus = udpSocket->Receive(packet, ip, port);
		packet >> recieverInt;


		if (udpSocket->udpStatus == sf::Socket::Done) {
			
			switch (recieverInt)
			{
			case HEADER_PLAYER::HELLO:
				//Creamos el serversalt
				//Preguntar donde guardar el client y como
				//packet << HEADER_SERVER::CHALLENGE_Q << serverSalt << clientSalt<<challenge;
				ManageHello(packet, ip, port);
				//Creamos el challenge

				udpSocket->udpStatus = udpSocket->Send(packet, ip, port);

				break;
			case HEADER_PLAYER::CHALLENGE_R:

				ManageChallenge_R(packet, ip, port);

				break;

			case HEADER_PLAYER::GENERICMSG_P:
			

		
				packet >> auxiliarMessage;
				for (std::map<unsigned short, PlayerInfo>::iterator it = this->clients.begin();it != clients.end();it++) {
					if (it->first != port) {
						SendMessage2AllClients(auxiliarMessage, it->first);
					}
					else {
						it->second.lastConnection->ResetTimer();
					}
				}
				break;
			case HEADER_PLAYER::EXIT:
				clients.erase(port);
					
				
				for (std::map<unsigned short, PlayerInfo>::iterator it = this->clients.begin();it != clients.end();it++) {
					SendMessage2AllClients("Un jugador se ha desconectado", it->first);

				}
			

				break;



			default:
				break;
			}
			
		}

	


	}
}

