CC=g++
CFLAGS=-I../includes  -std=c++17
LIBS=-lpthread -lboost_system  -lboost_thread -lncurses
DEPS = chatter.hpp
OBJ = chatter.o client.o
OBJ2 = chatter.o tcpServer.o
%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)
all: cppclient tcpServer
cppclient: $(OBJ)
		$(CC) -o $@  $^ $(CFLAGS) $(LIBS)
tcpServer: $(OBJ2)
	$(CC) -o $@  $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o
.PHONY: all clean
