#include "Socket.h"

#ifdef __linux__
#include <mutex>
#include <csignal>
#include <unistd.h>


namespace Socket {
	// Обработка сигналов
	bool signalReady = false;
	std::mutex initMutex;

	void init() {
		std::lock_guard<std::mutex> lock(initMutex);
		if (signalReady) return;
		signal(SIGPIPE, SIG_IGN);
		signalReady = true;
	}

	// Адреса

	int getaddrinfo(const char *node, const char *service, const addrinfo *hints, addrinfo **result) {
		return ::getaddrinfo(node, service, hints, result);
	}

	void freeaddrinfo(addrinfo * &result) {
		::freeaddrinfo(result);
		result = nullptr;
	}

	// Проверка

	bool ok(int s) {
		return (s != -1);
	}

	// Создание и подключение

	int socket(int family, int type, int protocol) {
		init();
		return ::socket(family, type, protocol);
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
		return ::accept(s, addr, (socklen_t *)addrlen);
	}

	// Чтение и запись

	int recv(int s, void *buf, int len, int flags) {
		return ::recv(s, buf, len, flags);
	}

	int recvfrom(int s, void *buf, int len, int flags, sockaddr *from, int *fromlen) {
		return ::recvfrom(s, buf, len, flags, from, (socklen_t *)fromlen);
	}

	int read(int s, void *buf, int len) {
		return ::read(s, (char *)buf, len);
	}

	int send(int s, const void *buf, int len, int flags) {
		return ::send(s, buf, len, flags);
	}

	int sendto(int s, const void *buf, int len, int flags, const sockaddr *to, int tolen) {
		return ::sendto(s, buf, len, flags, to, tolen);
	}

	int write(int s, const void *buf, int len) {
		return ::write(s, buf, len);
	}

	// Закрытие

	int close(int s) {
		return ::close(s);
	}

	// Исключение

	error::error(const char *what) : std::runtime_error(what) {}
	error::error(const std::string &what) : std::runtime_error(what) {}

} // namespace Socket
#endif
