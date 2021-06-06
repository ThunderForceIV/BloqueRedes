#pragma once
#include <SFML/Network.hpp>
#include <mutex>
#include <string>
#include <SFML\Graphics.hpp>
#include <iostream>
#include <map>
#include "Types.h"
#include <chrono>
#include <cstdlib>
#include <list>
#include <mutex>
#include <random>
#include <vector>
#include <numeric>
class UDPSocket
{
	sf::UdpSocket* udpSocket;
	
public:
	sf::Socket::Status udpStatus;
	UDPSocket();
	UDPSocket(sf::UdpSocket* _udpSocket);
	~UDPSocket();

	unsigned short GetLocalPort();
	void unBind();
	sf::Socket::Status Receive(sf::Packet&, sf::IpAddress&, unsigned short&);
	sf::Socket::Status Send(sf::Packet, sf::IpAddress, unsigned short);
	sf::Socket::Status Bind(unsigned short);
	

};

