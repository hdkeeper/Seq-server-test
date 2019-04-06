#include "Socket.h"

#ifdef __linux__
#include <mutex>
#include <csignal>
#include <cstring>
#include <cerrno>
#include <unistd.h>


// Обработка сигналов
static bool signalReady = false;
static std::mutex initMutex;

void init() {
	std::lock_guard<std::mutex> lock(initMutex);
	if (signalReady) return;
	signal(SIGPIPE, SIG_IGN);
	signalReady = true;
}

// Проверка успешности выполнения функций
static bool isOk(int s) {
    return (s != -1);
}

// Создание

Socket::Socket() {
	init();
    this->handle = 0;
}

Socket::Socket(int handle) {
    init();
    this->handle = handle;
}

Socket::Socket(int family, int type, int protocol) {
    init();
    handle = ::socket(family, type, protocol);
    if (!isOk(handle)) {
        throw SocketError(std::errno);
    }
}

void Socket::bind(const sockaddr *addr, int addrlen) {
    int res = ::bind(handle, addr, addrlen);
    if (!isOk(res)) {
        throw SocketError(std::errno);
    }
}

void Socket::listen(int backlog) {
    int res = ::listen(handle, backlog);
    if (!isOk(res)) {
        throw SocketError(std::errno);
    }
}

void Socket::connect(const sockaddr *addr, int addrlen) {
    int res = ::connect(handle, addr, addrlen);
    if (!isOk(res)) {
        throw SocketError(std::errno);
    }
}

Socket Socket::accept() {
    return this->accept(nullptr, nullptr);
}

Socket Socket::accept(sockaddr *addr, int *addrlen) {
    int res = ::accept(handle, addr, (socklen_t *)addrlen);
    if (!isOk(res)) {
        throw SocketError(std::errno);
    }
    return Socket(res);
}

// Чтение и запись

int Socket::recv(void *buf, int len, int flags) {
    int res = ::recv(handle, buf, len, flags);
    if (!isOk(res)) {
        throw SocketError(std::errno);
    }
    return res;
}

int Socket::recvfrom(void *buf, int len, int flags, sockaddr *from, int *fromlen) {
    int res = ::recvfrom(handle, (char *)buf, len, flags, from, (socklen_t *)fromlen);
    if (!isOk(res)) {
        throw SocketError(std::errno);
    }
    return res;
}

int Socket::read(void *buf, int len) {
    int res = ::read(handle, buf, len);
    if (!isOk(res)) {
        throw SocketError(std::errno);
    }
    return res;
}

int Socket::send(const void *buf, int len, int flags) {
    int res = ::send(handle, buf, len, flags);
    if (!isOk(res)) {
        throw SocketError(std::errno);
    }
    return res;
}

int Socket::sendto(const void *buf, int len, int flags, const sockaddr *to, int tolen) {
    int res = ::sendto(handle, buf, len, flags, to, tolen);
    if (!isOk(res)) {
        throw SocketError(std::errno);
    }
    return res;
}

int Socket::write(const void *buf, int len) {
    int res = ::write(handle, buf, len);
    if (!isOk(res)) {
        throw SocketError(std::errno);
    }
    return res;
}

// Закрытие

bool Socket::ok() {
	return handle && isOk(handle);
}

void Socket::close() {
    int res = ::close(handle);
    if (!isOk(res)) {
        throw SocketError(std::errno);
    }
    handle = 0;
}

// Адреса

void Socket::getAddrInfo(const char *node, const char *service, const addrinfo *hints, addrinfo **result) {
	init();
	int res = getaddrinfo(node, service, hints, result);
    if (res != 0) {
        throw SocketError(res);
    }
}

void Socket::freeAddrInfo(addrinfo * &result) {
	if (result) {
		freeaddrinfo(result);
	}
	result = nullptr;
}

// Исключения

SocketError::SocketError(const char *what) :
    std::runtime_error(what), code(0) {}

SocketError::SocketError(const std::string &what) :
    std::runtime_error(what), code(0) {}

SocketError::SocketError(int code) :
	std::runtime_error(std::strerror(code)), code(code) {}

#endif
