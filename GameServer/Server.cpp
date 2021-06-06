#include "Server.h"

float Server::AverageRtt()
{
	double average = std::accumulate(averageRtt.begin(), averageRtt.end(), 0.0) / averageRtt.size();
	if (averageRtt.size() == 0) average = 0;

	return average;
}

Server::Server()
{
	this->udpSocket = new UDPSocket();

}

Server::~Server()
{
}

void Server::fillCriticalMap(int key, std::string message, unsigned short port) {
	CriticalPackets critical;
	critical.ip = sf::IpAddress::LocalHost;
	critical.port = port;
	critical.local = key;
	critical.message = message;
	criticalPackets.insert(std::pair<int, CriticalPackets>(key, critical));

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
bool Server::CheckClientAndServerSalt(unsigned short puerto, int clientSalt, int serverSalt) {
	auto it = clients.find(puerto);
	if (clientSalt == it->second.playerSalt && serverSalt == it->second.serverSalt) {
		return true;
	}
	return false;
}
void Server::ManageHello(sf::Packet &packet, sf::IpAddress &ip, unsigned short &port) {
	PlayerInfo playerInfo;

	//Recibimos el playerSalt y guardamos información en el serverSalt
	packet >> playerInfo.playerSalt;
	playerInfo.serverSalt = rand() % MAX_64BITS;
	playerInfo.challengeNumber = ManageChallenge();
	//Insertamos el cliente en una lista de los clientes pendientes.
	clientsWaiting.insert(std::pair<unsigned int, PlayerInfo>(port, playerInfo));

	//Limpiamos el packet
	packet.clear();

	//Añadimos la información nueva al paquete. 
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

	bool addedClient = false;

	for (std::map<unsigned short, PlayerInfo>::iterator it = clientsWaiting.begin();it != clientsWaiting.end();it++) {

		if (it->first == port) {

			packet >> actualClientSalt;
			packet >> actualServerSalt;
			packet >> clientAnswer;
		

			if (it->second.playerSalt == actualClientSalt && it->second.serverSalt == actualServerSalt && ResolveChallenge(clientAnswer, it->second.challengeNumber)) {
				std::cout << std::endl<<"-------------------------------------------------------------" << std::endl;
				std::cout << "                          New Player                         " << std::endl;
				std::cout << "-------------------------------------------------------------" << std::endl;

				std::cout << "Player Salt: " << it->second.playerSalt << std::endl;

				std::cout << "Server Salt: " << it->second.serverSalt << std::endl;

				std::cout << "Challenge: " << it->second.challengeNumber << std::endl;
				std::cout << "Answer: " << clientAnswer << std::endl;
				
				int key = 0;
				for (std::map<unsigned short, PlayerInfo>::iterator it = this->clients.begin();it != clients.end();it++) {
					SendMessage2AllClients("Un jugador se ha conectado", it->first);
					fillCriticalMap(key, "Critical", it->first);
					key++;

				}
				key = 0;
				packet.clear();
				packet << HEADER_SERVER::WELCOME;
				packet << actualClientSalt;
				packet << actualServerSalt;
				it->second.position.x = rand() % INITIALPOSPLAYER;
				it->second.position.y = rand() % INITIALPOSPLAYER;
			
				packet << it->second.position.x;
				packet << it->second.position.y;
				FillEnemyOfNewPlayer(port, it->second.position);
				clients.insert(std::pair<unsigned int, PlayerInfo>(port, it->second));
				
				udpSocket->udpStatus = udpSocket->Send(packet, ip, port);
				FillEnemyToNewPlayer(port);
			}

		}
	}
	clientsWaiting.clear();
	//ESTO SIRVE PARA LOS PAQUETES CRITICOS


}

void Server::FillEnemyOfNewPlayer(unsigned short port, sf::Vector2i position) {
	enemy auxiliarEnemy;
	auxiliarEnemy.port = port;
	auxiliarEnemy.position = position;

	for (std::map<unsigned short, PlayerInfo>::iterator it = clients.begin();it != clients.end();it++) {
		it->second.enemyPositions.push_back(auxiliarEnemy);
	}

}

void Server::FillEnemyToNewPlayer(unsigned short port) {
	auto it = clients.find(port);
	enemy auxiliarEnemy;

	for (std::map<unsigned short, PlayerInfo>::iterator it2 = clients.begin();it2 != clients.end();it2++) {
		if (it2->first != port) {
			auxiliarEnemy.port = it2->first;
			auxiliarEnemy.position = it2->second.position;
			it->second.enemyPositions.push_back(auxiliarEnemy);

		}

	}

}
void Server::SendEnemyPos() {
	while (true) {
		sf::Packet packet;
		
		for (std::map<unsigned short, PlayerInfo>::iterator it = clients.begin();it != clients.end();it++) {
			for (int i = 0;i < it->second.enemyPositions.size();i++) {
				packet << ENEMYPOS_S;
				packet << it->second.enemyPositions[i].port;
				packet << it->second.enemyPositions[i].position.x;
				packet << it->second.enemyPositions[i].position.y;

				udpSocket->udpStatus = udpSocket->Send(packet, sf::IpAddress::LocalHost, it->first);
				packet.clear();

			}
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));

	}
}
void Server::ManageMove(sf::Packet& packet, unsigned short& port) {
	int x = 0, y = 0, auxiliar = 0;
	packet >> auxiliar;
	packet >> x;
	packet >> y;
	if (clients.size() != 0) {
		auto it = clients.find(port);

		if (auxiliar < it->second.accumulationMovement.size()) {
		}
		else {

			Accumulation acumulationAuxiliar;
			acumulationAuxiliar.id = auxiliar;
			acumulationAuxiliar.position.x = x;
			acumulationAuxiliar.position.y = y;
			servermtx.lock();
			it->second.accumulationMovement.push_back(acumulationAuxiliar);
			servermtx.unlock();
			packet.clear();

		}
	}

}
float Server::GetRTT(int key) {
	return criticalPackets[key].timer->GetMilisDuration();
}

void Server::RecieveClients() {
	int recieverInt;
	sf::Packet packet;
	sf::IpAddress ip;
	unsigned short port;
	while (true) {
		udpSocket->udpStatus = udpSocket->Receive(packet, ip, port);
		packet >> recieverInt;
		switch (recieverInt)
		{
		case HEADER_PLAYER::HELLO:
			//Creamos el serversalt
			//Preguntar donde guardar el client y como
			//packet << HEADER_SERVER::CHALLENGE_Q << serverSalt << clientSalt<<challenge;
			ManageHello(packet, ip, port);
			//Creamos el challengeç
			packet << ManageChallenge();
			udpSocket->udpStatus = udpSocket->Send(packet, ip, port);

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
	sf::Packet packet;
	std::mutex mtx;
	while (true)
	{
		for (std::map<unsigned short, PlayerInfo>::iterator it = clients.begin();it != clients.end();it++) {
			if (it->second.lastConnection->GetDuration() > PLAYER_DESCONNECTION)
			{
				inactivityCheck.push_back(it->first);
			}
		}
		for (std::list<unsigned short>::iterator it = inactivityCheck.begin(); it != inactivityCheck.end(); ++it) {
			packet << HEADER_SERVER::DISCONNECT;
			auto it2 = clients.find(*it);
			it2->second.playerSalt;
			it2->second.serverSalt;

			udpSocket->udpStatus = udpSocket->Send(packet, sf::IpAddress::LocalHost, *it);
			clients.erase(*it);
			packet.clear();

		}
		inactivityCheck.clear();

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}




}

void Server::ExitThread() {
	std::string message;
	sf::Packet packet;
	while(true){
	std::cout << std::endl << "If you type exit at any time you will close the server, if you close the tab it will be for inactivity: ";
	std::cin >> message;

	if (message == "exit") {
		for (std::map<unsigned short, PlayerInfo>::iterator it = clients.begin();it != clients.end();it++) {

			packet << HEADER_SERVER::SERVERDISCONNECTED;
			packet << it->second.playerSalt;
			packet << it->second.serverSalt;
			packet << message;
			udpSocket->udpStatus = udpSocket->Send(packet, sf::IpAddress::LocalHost, it->first);
			packet.clear();
			
		}
		exit(0);
	}
	
	}
}

void Server::SendCriticalPackets() {
	while (true) {
		sf::Packet packet;
		unsigned short idDelete = -1;
		bool deletePacket = false;
		for (std::map<int, CriticalPackets>::iterator it = criticalPackets.begin();it != criticalPackets.end();it++) {
			if (IsClientInMap(it->second.port)) {
				packet << HEADER_SERVER::CRITICALPACKAGE_S;
				packet << it->second.local;
				packet << it->second.message;
				if (it == criticalPackets.begin()) {
					std::cout << "Se envia paquete critico a  " << it->second.port << " con la key: " << it->second.local << std::endl;
				}
				udpSocket->udpStatus = udpSocket->Send(packet, sf::IpAddress::LocalHost, it->second.port);

				it->second.timer->ResetTimer();
			}
			else {
				idDelete = it->second.port;
				deletePacket = true;
			}
			
		}
		if (deletePacket == true) {
			criticalPackets.erase(idDelete);
			deletePacket = false;

		}
		std::this_thread::sleep_for(std::chrono::seconds(4));

	}
}

void Server::manageCriticalPackets(int key, unsigned short port) {
	servermtx.lock();
	std::cout << "El paquete critico ha tardado "<<GetRTT(key)<<std::endl;
	criticalPackets.erase(key);
	servermtx.unlock();
	}

void Server::DeleteEnemiesInPlayersVectors(unsigned short port) {
	int auxiliar;
	sf::Packet packet;
	for (std::map<unsigned short, PlayerInfo>::iterator it = clients.begin();it != clients.end();it++) {
		if (it->second.enemyPositions.size() != 0) {
			for (int i = 0;i < it->second.enemyPositions.size();i++) {
				if (port == it->second.enemyPositions[i].port) {
					auxiliar = i;
				}
			}
			it->second.enemyPositions.erase(it->second.enemyPositions.begin() + auxiliar);
			packet << HEADER_SERVER::DELETEENEMYPOS;
			packet << it->second.playerSalt;
			packet << it->second.serverSalt;
			packet << port;
			udpSocket->udpStatus = udpSocket->Send(packet, sf::IpAddress::LocalHost, it->first);

		}
	}
}
void Server::ServerLoop()
{

	this->udpSocket->Bind(50000);

	int recieverInt;
	int auxiliarPlayerSalt=-2;
	int auxiliarServerSalt=-1;
	int key = 0;
	int keyPackage = 0;

	std::string auxiliarMessage;
	std::thread tCheckInactivity(&Server::checkInactivity, this);
	tCheckInactivity.detach();

	std::thread tExit(&Server::ExitThread, this);
	tExit.detach();

	std::thread criticalPackages(&Server::SendCriticalPackets, this);
	criticalPackages.detach();



	std::thread tSendEnemyPos(&Server::SendEnemyPos, this);
	tSendEnemyPos.detach();
	
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
				if (clients.size() != 0) {
					auto it = clients.find(port);
					it->second.lastConnection->ResetTimer();
				}
				for (std::map<unsigned short, PlayerInfo>::iterator it = this->clients.begin();it != clients.end();it++) {
					if (it->first != port) {
						SendMessage2AllClients(auxiliarMessage, it->first);
					}
					
				}
				break;
			case HEADER_PLAYER::EXIT:
				packet >> auxiliarPlayerSalt;
				packet >> auxiliarServerSalt;
				if (CheckClientAndServerSalt(port, auxiliarPlayerSalt, auxiliarServerSalt)) {

					clients.erase(port);
					//criticalPackets.erase(port);
					DeleteEnemiesInPlayersVectors(port);
					for (std::map<unsigned short, PlayerInfo>::iterator it = this->clients.begin();it != clients.end();it++) {
						SendMessage2AllClients("Un jugador se ha desconectado", it->first);
						fillCriticalMap(key, "Critical", it->first);
						key++;

					}
					key = 0;
				}
				break;
			case HEADER_PLAYER::CRITICALPACKAGE_P:
				packet >> auxiliarPlayerSalt;
				packet >> auxiliarServerSalt;
				if(CheckClientAndServerSalt(port, auxiliarPlayerSalt, auxiliarServerSalt)) {
					packet >> keyPackage;
					manageCriticalPackets(keyPackage, port);
					packet.clear();
					keyPackage = -1;
				}
				break;

			case HEADER_PLAYER::MOVE_P:
				if (!playerCanMove) {
					gameTime = new Timer;
					
					playerCanMove = true;
					std::thread tPlayerMovement(&Server::SendClientsPositions, this);
					tPlayerMovement.detach();
				}
				if (clients.size() != 0) {
					auto it = clients.find(port);
					it->second.lastConnection->ResetTimer();
				}
				ManageMove(packet,port);
				break;
			default:
				break;
			}
			
		}

	


	}
}
void Server::ModifyEnemyPositions(unsigned short port, sf::Vector2i positions) {
	for (std::map<unsigned short, PlayerInfo>::iterator it = clients.begin();it != clients.end();it++) {
		if (port != it->first) {
			if (it->second.enemyPositions.size() != 0) {
				for (int i = 0;i < it->second.enemyPositions.size();i++) {
					if (it->second.enemyPositions[i].port == port) {
						it->second.enemyPositions[i].position = positions;
					}
				}
			}
		}
	}
}
bool Server::CheckIfEnemyIsInPlayerPos(unsigned short port) {
	auto it2 = clients.find(port);
	if (it2->second.enemyPositions.size() != 0) {
		for (int i = 0;i < it2->second.enemyPositions.size();i++) {
			if ((it2->second.position.x == it2->second.enemyPositions[i].position.x) && (it2->second.position.y == it2->second.enemyPositions[i].position.y)) {
				return true;
			}
		}
	}
	return false;

}
void Server::SendClientsPositions() {
	while (true) {
		sf::Packet packet;
		int auxiliarCheck = 0;
		if (gameTime->GetDuration() > GAME_TIMER) {
			for (std::map<unsigned short, PlayerInfo>::iterator it = clients.begin();it != clients.end();it++) {
				it->second.accumulationMovement.clear();
				packet << HEADER_SERVER::RESET_GAME;
				packet << it->second.playerSalt;
				packet << it->second.serverSalt;
				it->second.position.x = rand() % INITIALPOSPLAYER;
				it->second.position.y = rand() % INITIALPOSPLAYER;
				packet << it->second.position.x;
				packet << it->second.position.y;
				udpSocket->udpStatus = udpSocket->Send(packet, sf::IpAddress::LocalHost, it->first);
				gameTime->ResetTimer();

			}
		}
		else {
			for (std::map<unsigned short, PlayerInfo>::iterator it = clients.begin();it != clients.end();it++) {
				if (it->second.accumulationMovement.size() != 0) {
					packet << HEADER_SERVER::MOVE_S;
					packet << it->second.accumulationMovement[it->second.accumulationMovement.size() - 1].id;

					it->second.position.x = it->second.accumulationMovement[it->second.accumulationMovement.size() - 1].position.x;
					it->second.position.y = it->second.accumulationMovement[it->second.accumulationMovement.size() - 1].position.y;
					while (CheckIfEnemyIsInPlayerPos(it->first)) {
						auxiliarCheck = rand() % 4;
						switch (auxiliarCheck)
						{
						case 0:
							it->second.position.x++;
							break;
						case 1:
							it->second.position.x--;

							break;
						case 2:
							it->second.position.y++;

							break;
						case 3:
							it->second.position.y--;

							break;

						default:
							break;
						}
					}
					packet << it->second.position.x;
					packet << it->second.position.y;
					auxiliarCheck = 2;
					it->second.accumulationMovement.clear();

					ModifyEnemyPositions(it->first, it->second.position);
					udpSocket->udpStatus = udpSocket->Send(packet, sf::IpAddress::LocalHost, it->first);
					packet.clear();
				}
			}
		}
	}
}