#include "Socket.h"

#ifdef _WIN32
#include <mutex>

#include <WinBase.h>
#include <Winnt.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


// Инициализация библиотеки
static bool wsaReady = false;
static WSADATA wsaData;
static std::mutex initMutex;

static void init() {
    std::lock_guard<std::mutex> lock(initMutex);
    if (wsaReady) return;

    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != 0) {
        throw SocketError(res);
    }
    wsaReady = true;
}

// Проверка успешности выполнения функций
static bool isOk(int s) {
    return (s != INVALID_SOCKET) && (s != SOCKET_ERROR);
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
    handle = (int) ::socket(family, type, protocol);
    if (!isOk(handle)) {
        throw SocketError(WSAGetLastError());
    }
}

void Socket::bind(const sockaddr *addr, int addrlen) {
    int res = ::bind(handle, addr, addrlen);
    if (!isOk(res)) {
        throw SocketError(WSAGetLastError());
    }
}

void Socket::listen(int backlog) {
    int res = ::listen(handle, backlog);
    if (!isOk(res)) {
        throw SocketError(WSAGetLastError());
    }
}

void Socket::connect(const sockaddr *addr, int addrlen) {
    int res = ::connect(handle, addr, addrlen);
    if (!isOk(res)) {
        throw SocketError(WSAGetLastError());
    }
}

Socket Socket::accept() {
    return this->accept(nullptr, nullptr);
}

Socket Socket::accept(sockaddr *addr, int *addrlen) {
    int res = (int) ::accept(handle, addr, addrlen);
    if (!isOk(res)) {
        throw SocketError(WSAGetLastError());
    }
    return Socket(res);
}

// Чтение и запись

int Socket::recv(void *buf, int len, int flags) {
    int res = ::recv(handle, (char *)buf, len, flags);
    if (!isOk(res)) {
        throw SocketError(WSAGetLastError());
    }
    return res;
}

int Socket::recvfrom(void *buf, int len, int flags, sockaddr *from, int *fromlen) {
    int res = ::recvfrom(handle, (char *)buf, len, flags, from, fromlen);
    if (!isOk(res)) {
        throw SocketError(WSAGetLastError());
    }
    return res;
}

int Socket::read(void *buf, int len) {
    int res = ::recv(handle, (char *)buf, len, 0);
    if (!isOk(res)) {
        throw SocketError(WSAGetLastError());
    }
    return res;
}

int Socket::send(const void *buf, int len, int flags) {
    int res = ::send(handle, (const char *)buf, len, flags);
    if (!isOk(res)) {
        throw SocketError(WSAGetLastError());
    }
    return res;
}

int Socket::sendto(const void *buf, int len, int flags, const sockaddr *to, int tolen) {
    int res = ::sendto(handle, (const char *)buf, len, flags, to, tolen);
    if (!isOk(res)) {
        throw SocketError(WSAGetLastError());
    }
    return res;

}

int Socket::write(const void *buf, int len) {
    int res = ::send(handle, (const char *)buf, len, 0);
    if (!isOk(res)) {
        throw SocketError(WSAGetLastError());
    }
    return res;
}

// Закрытие

bool Socket::ok() {
    return handle && isOk(handle);
}

void Socket::close() {
    int res = ::closesocket(handle);
    if (!isOk(res)) {
        throw SocketError(WSAGetLastError());
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

static std::string getErrorString(int code) {
    char message[256];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, code, 
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPSTR) &message, sizeof(message), nullptr);
    return std::string(message);
}

SocketError::SocketError(int code) :
    std::runtime_error(getErrorString(code)), code(code) {}

#endif
