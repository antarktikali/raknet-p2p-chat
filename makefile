CC=g++
RAKNETDIR=./RakNet/Source
CFLAGS=-c -I$(RAKNETDIR) -std=c++11
LFLAGS=-lpthread $(RAKNETDIR)/*.o

all: raknet-p2p-chat

raknet-p2p-chat: raknet main.o P2PClient.o
	$(CC) $(LFLAGS) main.o P2PClient.o -o raknet-p2p-chat

raknet: $(patsubst $(RAKNETDIR)/%.cpp, $(RAKNETDIR)/%.o, $(wildcard $(RAKNETDIR)/*.cpp))

%.o: %.cpp makefile
	$(CC) -c $< -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

P2PClient.o: P2PClient.cpp
	$(CC) $(CFLAGS) P2PClient.cpp

clean:
	rm -rf *.o $(RAKNETDIR)/*.o raknet-p2p-chat

