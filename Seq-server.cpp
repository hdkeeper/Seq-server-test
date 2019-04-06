#include <iostream>
#include <thread>
#include <cstring>

#include "Socket.h"
#include "ClientHandler.h"

using namespace std;


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
	Socket serverSocket;

	try {
		Socket::getAddrInfo(LISTEN_IP, LISTEN_PORT, &hint, &serverAddr);
		serverSocket = Socket(hint.ai_family, hint.ai_socktype, hint.ai_protocol);
		serverSocket.bind(serverAddr->ai_addr, sizeof(addrinfo));
		serverSocket.listen(100);

		cout << "Listening on port " << LISTEN_PORT << endl;

		while (true) {
			auto clientSocket = serverSocket.accept();
			auto handler = new ClientHandler(clientSocket);
			thread(&ClientHandler::main, handler).detach();
			this_thread::sleep_for(chrono::milliseconds(100));
		}
	}
	catch (SocketError &ex) {
		cout << ex.what() << endl;

		if (serverSocket.ok()) {
			serverSocket.close();
		}

		Socket::freeAddrInfo(serverAddr);
		return 1;
	}

	return 0;
}
