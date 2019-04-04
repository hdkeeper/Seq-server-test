#include <iostream>
#include <thread>
#include <regex>

#include "Socket.h"
#include "ClientHandler.h"

using namespace std;


ClientHandler::ClientHandler(int socket) {
	this->socket = socket;
	for (int i = 0; i < SEQ_COUNT; i++) {
		cur[i] = start[i] = step[i] = 0;
	}
}

ClientHandler::~ClientHandler() {
	Socket::close(socket);
}


void ClientHandler::main() {
	const auto CMD_MAX_LEN = 50;
	char recvBuf[CMD_MAX_LEN + 1];
	string inputBuf;
	static string prompt("> ");

	try {
		// Отправить приглашение
		if (!Socket::ok(Socket::send(socket, prompt.c_str(), (int)prompt.size(), 0))) {
			throw Socket::error("Network send error");
		}

		while (true) {
			// Получить данные
			memset(recvBuf, 0, sizeof(recvBuf));
			int received = Socket::recv(socket, recvBuf, CMD_MAX_LEN, 0);
			if (received == 0) {
				throw Socket::error("Connection closed");
			}
			else if (!Socket::ok(received)) {
				throw Socket::error("Network recv error");
			}

			// Ищем конец введенной команды
			inputBuf.append(recvBuf);
			size_t endPos = 0;
			if ((endPos = inputBuf.find("\n")) != string::npos) {
				string command(inputBuf.substr(0, endPos));
				inputBuf.assign(inputBuf.substr(endPos + 1));

				// Обработать команду
				executeCommand(command);

				// Отправить приглашение
				if (!Socket::ok(Socket::send(socket, prompt.c_str(), (int)prompt.size(), 0))) {
					throw Socket::error("Network send error");
				}
			}

			this_thread::sleep_for(chrono::milliseconds(50));
		}
	}
	catch (Socket::error &ex) {
		// cout << ex.what() << endl;
		delete this;
		return;
	}
}


bool ClientHandler::executeCommand(const string &cmd) {
	static regex reSeqCmd("^seq(\\d) (\\d+) (\\d+)");
	static regex reExportCmd("^export seq");
	smatch m;

	if (regex_search(cmd, m, reSeqCmd)) {
		return setupSequence(
			stoi(m[1]),
			(uint64_t)stoll(m[2]),
			(uint64_t)stoll(m[3]));
	}

	if (regex_search(cmd, m, reExportCmd)) {
		return exportSequence();
	}

	return false;
}

// Здесь idx от 1 до SEQ_COUNT
bool ClientHandler::setupSequence(int idx, uint64_t start, uint64_t step) {
	if (idx < 1 || idx > ClientHandler::SEQ_COUNT) {
		return false;
	}
	if (step == 0) {
		return false;
	}

	auto i = idx - 1;
	this->start[i] = start;
	this->step[i] = step;
	return true;
}

bool ClientHandler::exportSequence() {
	// Проверить, что хотя бы одна последовательность задана
	bool defined = false;
	for (int i = 0; i < ClientHandler::SEQ_COUNT; i++) {
		cur[i] = start[i];
		defined = defined || (start[i] && step[i]);
	}
	if (!defined) {
		return false;
	}

	// Генерация последовательности
	while (true) {
		string sendBuf;
		for (int i = 0; i < ClientHandler::SEQ_COUNT; i++) {
			if (start[i] && step[i]) {
				if (sendBuf.length()) {
					sendBuf.append("\t");
				}
				sendBuf.append(to_string(cur[i]));
				if (UINT64_MAX - cur[i] < step[i]) {
					cur[i] = start[i];
				}
				else {
					cur[i] += step[i];
				}
			}
		}
		sendBuf.append("\r\n");

		// Отправить в сокет
		if (!Socket::ok(Socket::send(socket, sendBuf.c_str(), (int)sendBuf.size(), 0))) {
			throw Socket::error("Network send error");
		}

		this_thread::sleep_for(chrono::milliseconds(50));
	}

	return true;
}
