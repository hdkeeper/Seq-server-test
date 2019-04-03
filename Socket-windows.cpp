#include "Socket.h"

#ifdef _WIN32
#include <mutex>

#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


namespace Socket {
	// Инициализация библиотеки
	bool wsaReady = false;
	WSADATA wsaData;
	std::mutex initMutex;

	void init() {
		std::lock_guard<std::mutex> lock(initMutex);
		if (wsaReady) return;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			throw error("Socket library error");
		}
		wsaReady = true;
	}

	// Адреса

	int getaddrinfo(const char *node, const char *service, const addrinfo *hints, addrinfo **result) {
		init();
		return ::getaddrinfo(node, service, hints, result);
	}

	void freeaddrinfo(addrinfo * &result) {
		::freeaddrinfo(result);
		result = nullptr;
	}

	// Проверка

	bool ok(int s) {
		return (s != INVALID_SOCKET) && (s != SOCKET_ERROR);
	}

	// Создание и подключение

	int socket(int family, int type, int protocol) {
		init();
		return (int) ::socket(family, type, protocol);
	}

	int bind(int s, const sockaddr *addr, int addrlen) {
		return ::bind(s, addr, addrlen);
	}
	
	int listen(int s, int backlog) {
		return ::listen(s, backlog);
	}
	
	int connect(int s, const sockaddr *addr, int addrlen) {
		return ::connect(s, addr, addrlen);
	}
	
	int accept(int s, sockaddr *addr, int *addrlen) {
		return (int) ::accept(s, addr, addrlen);
	}

	// Чтение и запись

	int recv(int s, void *buf, int len, int flags) {
		return ::recv(s, (char *) buf, len, flags);
	}

	int recvfrom(int s, void *buf, int len, int flags, sockaddr *from, int *fromlen) {
		return ::recvfrom(s, (char *) buf, len, flags, from, fromlen);
	}
	
	int read(int s, void *buf, int len) {
		return ::recv(s, (char *) buf, len, 0);
	}

	int send(int s, const void *buf, int len, int flags) {
		return ::send(s, (const char *) buf, len, flags);
	}

	int sendto(int s, const void *buf, int len, int flags, const sockaddr *to, int tolen) {
		return ::sendto(s, (const char *) buf, len, flags, to, tolen);
	}
	
	int write(int s, const void *buf, int len) {
		return ::send(s, (const char *) buf, len, 0);
	}

	// Закрытие

	int close(int s) {
		return ::closesocket(s);
	}

	// Исключение

	error::error(const char *what) : std::runtime_error(what) {}
	error::error(const std::string &what) : std::runtime_error(what) {}

} // namespace Socket
#endif
