#include "UDPSocket.h"

UDPSocket::UDPSocket()
{
    udpSocket = new sf::UdpSocket;
}

UDPSocket::UDPSocket(sf::UdpSocket* _udpSocket)
{
    udpSocket = _udpSocket;
}

UDPSocket::~UDPSocket()
{
}

unsigned short UDPSocket::GetLocalPort()
{
    return 0;
}

void UDPSocket::unBind()
{
    udpSocket->unbind();
}

sf::Socket::Status UDPSocket::Receive(sf::Packet& pack, sf::IpAddress& ip, unsigned short& port)
{
    this->udpStatus = udpSocket->receive(pack, ip, port);
    return udpStatus;
}

sf::Socket::Status UDPSocket::Send(sf::Packet pack, sf::IpAddress ip, unsigned short port)
{
    this->udpStatus = udpSocket->send(pack, ip, port);
    return udpStatus;
}

sf::Socket::Status UDPSocket::Bind(unsigned short port)
{
    this->udpStatus = udpSocket->bind(port);
    return udpStatus;
}
