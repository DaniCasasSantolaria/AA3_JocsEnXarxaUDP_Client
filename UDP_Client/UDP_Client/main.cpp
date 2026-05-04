#include <SFML/Network.hpp>
#include <iostream>
#include <cstring>

#define SERVER_PORT 55008
const sf::IpAddress SERVER_IP = sf::IpAddress(127, 0, 0, 1);

void main() {
	sf::UdpSocket serverSocket;

	char buffer[1024];
	std::size_t bufferDataSize = 0;
	int data = 0;
	std::string message = "hola";
	std::string final = message + std::to_string(data);
	for (int i = 0; i < 100000; ++i) {
		int messageSize = message.size();
		std::memcpy(buffer + bufferDataSize, &messageSize, sizeof(messageSize));
		bufferDataSize += (sizeof(messageSize));

		std::memcpy(buffer + bufferDataSize, message.c_str(), messageSize);
		bufferDataSize += messageSize;

		std::memcpy(buffer + bufferDataSize, &i, sizeof(i));
		bufferDataSize += (sizeof(i));

		if (serverSocket.send(buffer, bufferDataSize, SERVER_IP, SERVER_PORT) == sf::Socket::Status::Done) {
			std::cout << "Mensaje entregado al sistema operativo" << std::endl;
			data += 1;
			//final = message + std::to_string(data);
			bufferDataSize = 0;
			//return;
		}
	}


}