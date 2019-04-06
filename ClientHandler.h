#pragma once
#include <cstdint>
#include <string>

#include "Socket.h"


class ClientHandler {
public:
    ClientHandler(Socket &socket);
    ~ClientHandler();
    void main();

private:
    Socket socket;
    constexpr static int SEQ_COUNT = 3;
    uint64_t start[ClientHandler::SEQ_COUNT];
    uint64_t step[ClientHandler::SEQ_COUNT];
    uint64_t cur[ClientHandler::SEQ_COUNT];

    bool executeCommand(const std::string &cmd);
    bool setupSequence(int idx, uint64_t start, uint64_t step);
    bool exportSequence();
};
