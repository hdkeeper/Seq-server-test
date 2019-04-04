#pragma once
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


namespace Socket {
	// Адреса
	int getaddrinfo(const char *node, const char *service, const addrinfo *hints, addrinfo **result);
	void freeaddrinfo(addrinfo * &result);

	// Проверка
	bool ok(int s);

	// Создание и подключение
	int socket(int family, int type, int protocol);
	int bind(int s, const sockaddr *addr, int addrlen);
	int listen(int s, int backlog);
	int connect(int s, const sockaddr *addr, int addrlen);
	int accept(int s, sockaddr *addr, int *addrlen);

	// Чтение и запись
	int recv(int s, void *buf, int len, int flags);
	int recvfrom(int s, void *buf, int len, int flags, sockaddr *from, int *fromlen);
	int read(int s, void *buf, int len);
	int send(int s, const void *buf, int len, int flags);
	int sendto(int s, const void *buf, int len, int flags, const sockaddr *to, int tolen);
	int write(int s, const void *buf, int len);

	// Закрытие
	int close(int s);

	// Исключение
	class error : public std::runtime_error {
	public:
		error(const char *what);
		error(const std::string &what);
	};

}
