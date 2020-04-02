CC=g++
CFLAGS=-I../includes -lpthread -lboost_system  -lboost_thread -std=c++17
DEPS = chatter.hpp
OBJ = chatter.o client.o
OBJ2 = chatter.o tcpServer.o
%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)
all: cppclient tcpServer
cppclient: $(OBJ)
		$(CC) -o $@  $^ $(CFLAGS)
tcpServer: $(OBJ2)
	$(CC) -o $@  $^ $(CFLAGS)

clean:
	rm -f *.o
.PHONY: all clean
