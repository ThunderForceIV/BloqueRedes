#pragma once
#include <SFML\Network.hpp>
#include <mutex>
#include <string>

class UDPSocket
{
	sf::UdpSocket* udpSocket;
	sf::Socket::Status udpStatus;
public:
	UDPSocket();
	UDPSocket(sf::UdpSocket* _udpSocket);
	~UDPSocket();

	unsigned short GetLocalPort();
	void unBind();
	sf::Socket::Status Receive(sf::Packet&, sf::IpAddress&, unsigned short&);
	sf::Socket::Status Send(sf::Packet, sf::IpAddress, unsigned short);
	sf::Socket::Status Bind(unsigned short);


};

