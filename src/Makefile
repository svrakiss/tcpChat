CC=g++
CFLAGS=-I/home/des/git/ece312/includes -pthread -lboost_system -std=c++11
# DEPS = chatter.h
OBJ = chatter.o client.o
%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)
cppclient: $(OBJ)
	$(CC) -o $@  $^ $(CFLAGS)