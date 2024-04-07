#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H
#include "socket.h"
#include "Blockable.h"

typedef int TerminationException;

class SocketServer
{
private:
    int socketFD;
    sockaddr_in socketDescriptor;
public:
    SocketServer(int port);
    SocketModule::Socket Accept(void);
    void Shutdown(void);
    ~SocketServer(void);
};
#endif // SOCKETSERVER_H
