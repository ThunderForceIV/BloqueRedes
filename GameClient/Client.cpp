#include "Client.h"

Client::Client()
{
	udpSocket = new UDPSocket();
	clientSalt = rand() % MAX_64BITS;

}

Client::~Client()
{
}
//Se introduce el username del cliente
void Client::Username() {
	LineCout();

	std::cout <<std::endl<< "                           CLIENT                         " << std::endl;
	LineCout();
	std::cout << "Introduce your username: ";
	std::cin >> username;
}
//Se une a la partida si escribe y, sino se cierra la consola
void Client::JoinGame() {
	std::string confirmation;
	std::cout << std::endl << "Do you want to join or create a game? (y/n): ";
	std::cin >> confirmation;
	if (confirmation == "y" || confirmation == "Y") {
		gamerunning = true;
		system("cls");
	}
	else {
		exit(0);
	}
}

//Cuando el cliente recibe un manageChallenge_Q, lo recibe, solucionar el challenge se hace en la parte donde recibe el paquete
void Client::ManageChallenge_Q(sf::Packet &packet, sf::IpAddress &ip, unsigned short &port) {
	packet >> clientSalt;
	packet >> serverSalt;
	packet >> challengeNumber;
	packet.clear();
	packet << HEADER_PLAYER::CHALLENGE_R;
	packet << clientSalt;
	packet << serverSalt;

	
}
//Función para resolver el paquete
int Client::ResolveChallenge(int challengeNumber) {
	return challengeNumber / 2;
}
void Client::LineCout() {
	std::cout << std::endl<<"-------------------------------------------------------------" << std::endl;

}
//Cuando recibe un paquete critico lo responde con un A<K
void Client::manageCriticalPackage(sf::Packet &packet) {
	int key;
	std::string message;
	packet >> key;
	packet >> message;
	packet.clear();
	packet << HEADER_PLAYER::CRITICALPACKAGE_P;
	packet << clientSalt;
	packet << serverSalt;
	packet << key;
	packet << "respuesta";
	udpSocket->udpStatus = udpSocket->Send(packet, sf::IpAddress::LocalHost, SERVER_PORT);
	packet.clear();
	LineCout();
	if (udpSocket->udpStatus == sf::Socket::Done) {
		std::cout << "The ACK packet has been sent successfully" << std::endl;
	}
	else {
		std::cout << "The ACK packet has not been sent";
	}
	LineCout();

}
//Conseguimos un random float para la perdida de paquetes
static float GetRandomFloat() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dis(0.f, 1.f);
	return dis(gen);
}
//El cliente recibe Welcome
void Client::ManageWelcome(sf::Packet& packet) {
		packet >> clientSalt;
	 packet >> serverSalt;
	 packet >> position.x;
	 packet >> position.y;
	 acumulationPosition = position;
	 LineCout();
	 std::cout << "You have been connected to the server correctly" << std::endl << "Position X: " << position.x << std::endl<<"Position Y: "<<position.y;
	 LineCout();
}

//Controlamos el movimiento
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

//Se controlan las posiciones de los enemigos para poder printearlas correctamente
void Client::ManageEnemyPos(sf::Packet& packet) {
	if (protocolConnected == true) {
		unsigned short ipLocal;
		bool isInVector = false;
		sf::Vector2i positionAuxiliar;
		packet >> ipLocal;
		packet >> positionAuxiliar.x;
		packet >> positionAuxiliar.y;
		clientMtx.lock();
		if (enemyPos.size() != 0) {
			for (int i = 0; i < enemyPos.size();i++) {
				if (ipLocal == enemyPos[i].port) {
					enemyPos[i].position.x = positionAuxiliar.x;
					enemyPos[i].position.y = positionAuxiliar.y;
					isInVector = true;
					std::cout << "The enemy " << enemyPos[i].port << " Position X: " << enemyPos[i].position.x << "  Position Y: " << enemyPos[i].position.y << std::endl;

				}
			}
		}
		if (isInVector == false) {
			enemy auxiliar;
			auxiliar.port = ipLocal;
			auxiliar.position.x = positionAuxiliar.x;
			auxiliar.position.y = positionAuxiliar.y;
			enemyPos.push_back(auxiliar);
		}
		LineCout();
		isInVector = false;
		clientMtx.unlock();
	}
}
void Client::ManageDisconnect() {
	udpSocket->unBind();

	exit(0);

	

}

//En el reset game reiniciamos la partida
void Client::ManageResetGame(sf::Packet packet) {
	packet >> auxiliarClientSalt;
	packet >> auxiliarServerSalt;
	packet >> position.x;
	packet >> position.y;
	acumulationPosition.x = position.x;
	acumulationPosition.y = position.y;
	system("cls");
	std::cout << "---------------------------------------------------------------" << std::endl;
	std::cout << "                            RESET GAME                         " << std::endl;
	std::cout << "---------------------------------------------------------------" << std::endl;
	std::cout << "New Position " <<"---------- Position X: " << position.x << " ---------- Position Y: " << position.y << std::endl;

}

//Controlamos el delete de los enemigos
void Client::ManageDeleteEnemyPos(sf::Packet &packet) {
	packet >> auxiliarClientSalt;
	packet >> auxiliarServerSalt;
	unsigned short port;

	packet >> port;
	int auxiliar;
	if (enemyPos.size() != 0) {
		for (int i = 0;i < enemyPos.size();i++) {
			if (port == enemyPos[i].port) {
				auxiliar = i;
				LineCout();
				std::cout << "Player with port " << port << " has been disconnected from the server";
				LineCout();
			}
		}
		
		enemyPos.erase(enemyPos.begin() + auxiliar);
		

	}
}
void Client::ManageCleanDisconnected() {
	LineCout();
	std::cout <<"          THE SERVER HAS BEEN DISCONNECTED            ";
	LineCout();
	disconnected = new Timer;
	while (disconnected->GetDuration() < SERVER_DESCONNECTIONCLEAN) {

	}
	ManageDisconnect();

}
//Aqui sera donde reciba todos los paquetes y los maneje dependiendo de su tag
void Client::RecievingThread() {//Escucha los paquetes que envia el servidor
	sf::Packet packet;
	sf::IpAddress ip;
	unsigned short port;
	std::string message;
	int recieverInt;

	while (true)
	{
		//Tenemos un porcentaje de perdida para los paquetes
		rndPacketLoss = GetRandomFloat();
		udpSocket->udpStatus = udpSocket->Receive(packet, ip, port);

		if (rndPacketLoss > PERCENT_PACKETLOSS) {

			packet >> recieverInt;
			if (udpSocket->udpStatus == sf::Socket::Done) {
				if (timerActivated == true) {
					serverDisconnected->ResetTimer();
				}
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
					serverDisconnected = new Timer;
					timerActivated = true;
					break;
				case HEADER_SERVER::GENERICMSG_S:
					packet >> message;
					if (udpSocket->udpStatus == sf::Socket::Done) {
						std::cout << std::endl << "You recived from the server: " << message << "." << std::endl;
						packet.clear();
					
				}
				else {
					std::cout << "Se ha perdido el paquete";
				}

				break;
			case HEADER_SERVER::CRITICALPACKAGE_S:
				manageCriticalPackage(packet);
				break;
			case HEADER_SERVER::MOVE_S:

				ManageMovement(packet);
				break;
			case HEADER_SERVER::ENEMYPOS_S:
				ManageEnemyPos(packet);
				break;
			case HEADER_SERVER::DISCONNECT:
				ManageDisconnect();
				break;
			
			case HEADER_SERVER::RESET_GAME:
				ManageResetGame(packet);
				break;
			case HEADER_SERVER::DELETEENEMYPOS:
				ManageDeleteEnemyPos(packet);
				break;
			case HEADER_SERVER::SERVERDISCONNECTED:
				ManageCleanDisconnected();
				break;
			}
		}


		}

	}
}
		
		

void Client::SendHello()
{
	std::string msg;
	sf::Packet packet;
	while (!protocolConnected) {
		packet << HEADER_PLAYER::HELLO;
		packet << clientSalt;
		packet << username;
		udpSocket->udpStatus = udpSocket->Send(packet, sf::IpAddress::LocalHost, SERVER_PORT);
		if (udpSocket->udpStatus != sf::Socket::Done) {
			std::cout << "El paquete no ha salido del cliente";
		}
		else {
			std::cout << "Hello has been sent correctly ClientSalt: " <<clientSalt <<" Username: " <<username <<std::endl;

		
			packet.clear();
		}		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	
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
				packet << clientSalt;
				packet << serverSalt;
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
					position.y -= 1;


				}
				else if (event.key.code == sf::Keyboard::Down)
				{
					UpdateDungeonPosition(acumulationPosition.x, acumulationPosition.y + 1);
					position.y += 1;

				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					UpdateDungeonPosition(acumulationPosition.x + 1, acumulationPosition.y);
					position.x += 1;

				}
				else if (event.key.code == sf::Keyboard::Left)
				{
					UpdateDungeonPosition(acumulationPosition.x-1, acumulationPosition.y);
					position.x -= 1;

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
		if(enemyPos.size()!=0){
			for (int i = 0;i < enemyPos.size();i++) {
				shape.setFillColor(sf::Color::Red);
				shape.setPosition(sf::Vector2f(enemyPos[i].position.x * DUNGEON_SIZE, enemyPos[i].position.y * DUNGEON_SIZE));
				_window.draw(shape);
			}

		}

		_window.display();
	}
}
void Client::ClientLoop()
{

	Username();
	JoinGame();
	std::thread sendHello(&Client::SendHello,this);
	sendHello.detach();
	std::thread tRecieve(&Client::RecievingThread, this);
	tRecieve.detach();
	std::thread tSend(&Client::SendingThread, this);
	tSend.detach();
	
	
	while (true)
	{
	if (protocolConnected) {
		if (timerActivated == true) {
			if (serverDisconnected->GetDuration() > SERVER_DESCONNECTION) {
				ManageDisconnect();
			}
		}
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
		packet.clear();
		std::this_thread::sleep_for(std::chrono::milliseconds(1300));

	}
}