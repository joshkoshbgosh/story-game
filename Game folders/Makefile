all: Client Server

Client : Client.o socket.o Blockable.o Story.o
	g++ -o Client Client.o socket.o Blockable.o Story.o -pthread -l rt

Client.o : Client.cpp socket.h
	g++ -c Client.cpp -std=c++17

Server : Server.o thread.o socket.o socketserver.o Blockable.o Story.o
	g++ -o Server Server.o thread.o socket.o socketserver.o Blockable.o Story.o -pthread -l rt

Blockable.o : Blockable.h Blockable.cpp
	g++ -c Blockable.cpp -std=c++17

Server.o : Server.cpp thread.h socketserver.h
	g++ -c Server.cpp -std=c++17

thread.o : thread.cpp thread.h
	g++ -c thread.cpp -std=c++17

socket.o : socket.cpp socket.h
	g++ -c socket.cpp -std=c++17

socketserver.o : socketserver.cpp socket.h socketserver.h
	g++ -c socketserver.cpp -std=c++17

Story.o : Story.cpp Story.h
	g++ -c Story.cpp -std=c++17
