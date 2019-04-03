#include <iostream>
#include <exception>
#include <thread>
#include <cstring>

using namespace std;

#include "Socket.h"
#include "ClientHandler.h"


const auto LISTEN_IP = "0.0.0.0";
const auto LISTEN_PORT = "2323";


int main()
{
	addrinfo hint;
	addrinfo *serverAddr = nullptr;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_protocol = IPPROTO_TCP;
	hint.ai_flags = AI_PASSIVE;

	int err = 0;
	if ((err = Socket::getaddrinfo(LISTEN_IP, LISTEN_PORT, &hint, &serverAddr)) != 0) {
		cout << "getaddrinfo() error: " << err << "\n";
		return 1;
	}

	int serverSocket = 0;
	try {
		if (!Socket::ok(serverSocket = Socket::socket(hint.ai_family, hint.ai_socktype, hint.ai_protocol))) {
			throw Socket::error("socket() error");
		}

		if (!Socket::ok(Socket::bind(serverSocket, serverAddr->ai_addr, sizeof(addrinfo)))) {
			throw Socket::error("bind() error");
		}

		if (!Socket::ok(Socket::listen(serverSocket, 100))) {
			throw Socket::error("listen() error");
		}

		cout << "Listening on port " << LISTEN_PORT << endl;

		while (true) {
			// TEST
			sockaddr clientAddr;
			int clientAddrLen = sizeof(clientAddr);

			int clientSocket = 0;
			if (Socket::ok(clientSocket = Socket::accept(serverSocket, &clientAddr, &clientAddrLen))) {
				auto handler = new ClientHandler(clientSocket);
				thread(&ClientHandler::main, handler).detach();
			}

			this_thread::sleep_for(chrono::milliseconds(100));
		}
	}
	catch (Socket::error &ex) {
		cout << ex.what() << endl;

		if (Socket::ok(serverSocket)) {
			Socket::close(serverSocket);
		}

		Socket::freeaddrinfo(serverAddr);
		return 1;
	}

	return 0;
}
