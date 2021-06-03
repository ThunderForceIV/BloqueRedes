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

void Client::manageCriticalPackage(sf::Packet &packet) {
	int key;
	std::string message;
	packet >> key;
	packet >> message;
	packet.clear();
	packet << HEADER_PLAYER::CRITICALPACKAGE_P;
	packet << key;
	packet << "respuesta";
	udpSocket->udpStatus = udpSocket->Send(packet, sf::IpAddress::LocalHost, SERVER_PORT);
	packet.clear();
}
static float GetRandomFloat() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dis(0.f, 1.f);
	return dis(gen);
}

void Client::ManageWelcome(sf::Packet& packet) {
		packet >> clientSalt;
	 packet >> serverSalt;
	 packet >> position.x;
	 packet >> position.y;
	 std::cout << position.x <<" Position y   "<< position.y << std::endl;
}

void Client::ManageMovement(sf::Packet& packet) {
	int x = 0,  y = 0, auxiliar = 0;
	packet >> auxiliar;
	packet >> x;
	packet >> y;
	
	position.x = x;
	position.y = y;

	clientMtx.lock();
	accumulationVector.erase(accumulationVector.begin() + auxiliar);
	clientMtx.unlock();


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

				port = SERVER_PORT;
				udpSocket->udpStatus = udpSocket->Send(packet, ip, port);

				break;
			case HEADER_SERVER::WELCOME:
				ManageWelcome(packet);
				userRegisted = true;
				protocolConnected = true;
				break;
			case HEADER_SERVER::GENERICMSG_S:
				packet >> message;
				if (udpSocket->udpStatus == sf::Socket::Done) {
					std::cout << std::endl << "Has recibido " << message << "." << std::endl;
					packet.clear();
				}

				break;
			case HEADER_SERVER::CRITICALPACKAGE_S:
				manageCriticalPackage(packet);
				std::cout << "Se ha enviado AKM"<<std::endl;
				break;
			case HEADER_SERVER::MOVE_S:
				ManageMovement(packet);
				break;
			}
			
		}
	
		
		else {
			std::cout << "SE HA PERDIDO EL PAQUETE";
		}
	}
}
void Client::SendHello()
{
	std::string msg;
	sf::Packet packet;
	while (!protocolConnected) {
		this->clientSalt = rand() % MAX_64BITS;
		std::cout << clientSalt;
		packet << HEADER_PLAYER::HELLO;
		packet << clientSalt;
		udpSocket->udpStatus = udpSocket->Send(packet, sf::IpAddress::LocalHost, SERVER_PORT);
		std::cout << "Se ha enviado Hello al server" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	
	}
}

void Client::SendingThread() {//Envia los paquetes
	sf::Packet packet;
	sf::IpAddress ip = sf::IpAddress::LocalHost;
	unsigned short port = SERVER_PORT;//Modificar este magic number
	std::string message;
	while (true)
	{
		if (protocolConnected) {
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
void Client::DrawDungeon()
{
	sf::RenderWindow _window(sf::VideoMode(800, 600), "Game");
	sf::RectangleShape shape(sf::Vector2f(DUNGEON_SIZE, DUNGEON_SIZE));
	shape.setOutlineColor(sf::Color::Black);
	shape.setOutlineThickness(2.f);

	while (_window.isOpen())
	{
		sf::Event event;
		while (_window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				_window.close();
				//gameloop = false;
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape)
				{
					_window.close();
					//gameloop = false;

				}
				if (event.key.code == sf::Keyboard::Up)
				{
					UpdateDungeonPosition(acumulationPosition.x, acumulationPosition.y - 1);

				}
				else if (event.key.code == sf::Keyboard::Down)
				{
					UpdateDungeonPosition(acumulationPosition.x, acumulationPosition.y + 1);
				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					UpdateDungeonPosition(acumulationPosition.x + 1, acumulationPosition.y);

				}
				else if (event.key.code == sf::Keyboard::Left)
				{
					UpdateDungeonPosition(acumulationPosition.x-1, acumulationPosition.y);
				}
				
				
				break;
			}
		}
		_window.clear();


		for (int i = 0; i < CELL_WIDTH_WINDOW; i++)
		{
			for (int j = 0; j < CELL_HEIGHT_WINDOW; j++)
			{
				shape.setFillColor(sf::Color(90, 90, 90, 255));
				shape.setPosition(sf::Vector2f(i * DUNGEON_SIZE, j * DUNGEON_SIZE));
				_window.draw(shape);
			}
		}

		shape.setFillColor(sf::Color::Red);
		shape.setFillColor(sf::Color(0, 0, 255, 255));
		shape.setPosition(sf::Vector2f(position.x * DUNGEON_SIZE, position.y * DUNGEON_SIZE));
		_window.draw(shape);

		_window.display();
	}
}
void Client::ClientLoop()
{

	std::thread sendHello(&Client::SendHello,this);
	sendHello.detach();
	std::thread tRecieve(&Client::RecievingThread, this);
	tRecieve.detach();
	std::thread tSend(&Client::SendingThread, this);
	tSend.detach();
	
	
	while (true)
	{
	if (protocolConnected) {
		if (!dungeonCreated) {
			dungeonCreated = true;
			std::thread drawDungeon(&Client::DrawDungeon, this);
			drawDungeon.detach();
			
			std::thread tAccumulationMove(&Client::SendAcumulationPackets, this);
			tAccumulationMove.detach();
		}
		}
	}
}
void Client::UpdateDungeonPosition(int x, int y)
{
	acumulationPosition.x = x;
	acumulationPosition.y = y;
}


void Client::SendAcumulationPackets() {
	while (true) {
		sf::Packet packet;
		Accumulation accumulationMovement;
		accumulationMovement.position = acumulationPosition;
		accumulationMovement.id = accumulationVector.size();
		accumulationVector.push_back(accumulationMovement);
		packet << HEADER_PLAYER::MOVE_P;
		packet << accumulationMovement.id;
		packet << acumulationPosition.x;
		packet << acumulationPosition.y;
		udpSocket->udpStatus = udpSocket->Send(packet, sf::IpAddress::LocalHost, SERVER_PORT);
		std::this_thread::sleep_for(std::chrono::milliseconds(1300));

	}
}