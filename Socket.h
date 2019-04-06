#pragma once
#include <string>
#include <stdexcept>

#ifdef _WIN32
#include <WinSock2.h>
#elif __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#endif


class Socket {
public:
    int handle = 0;

    // Создание
    Socket();
    Socket(int handle);
    Socket(int family, int type, int protocol);

    // Подключение
    void bind(const sockaddr *addr, int addrlen);
	void listen(int backlog);
	void connect(const sockaddr *addr, int addrlen);
	Socket accept();
    Socket accept(sockaddr *addr, int *addrlen);

    // Чтение и запись
   	int recv(void *buf, int len, int flags = 0);
	int recvfrom(void *buf, int len, int flags, sockaddr *from, int *fromlen);
	int read(void *buf, int len);
	int send(const void *buf, int len, int flags = 0);
	int sendto(const void *buf, int len, int flags, const sockaddr *to, int tolen);
	int write(const void *buf, int len);
 
    // Закрытие
    bool ok();
	void close();

    // Адреса
	static void getAddrInfo(const char *node, const char *service, const addrinfo *hints, addrinfo **result);
	static void freeAddrInfo(addrinfo * &result);
};

class SocketError : public std::runtime_error {
public:
    SocketError(const char *what);
    SocketError(const std::string &what);
    SocketError(int code);
    int code;
};
