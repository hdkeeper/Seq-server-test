CC = clang++ -lpthread -ggdb
#CC = g++ -pthread -ggdb
CPP_FILES = Seq-server.cpp ClientHandler.cpp Socket-linux.cpp
H_FILES = ClientHandler.h Socket.h
BIN_OUT = seq-server

$(BIN_OUT): $(H_FILES) $(CPP_FILES)
	$(CC) $(CPP_FILES) -o $(BIN_OUT)

clean:
	rm $(BIN_OUT)
