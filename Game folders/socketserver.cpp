#include "socketserver.h"
#include "Blockable.h"
#include <strings.h>
#include <iostream>
#include <errno.h>
#include <algorithm>
	
SocketServer::SocketServer(int port)
{
    // The first call has to be to socket(). This creates a UNIX socket.
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0)
        throw std::string("Unable to open the socket server");

    // The second call is to bind().  This identifies the socket file
    // descriptor with the description of the kind of socket we want to have.
    bzero((char*)&socketDescriptor,sizeof(sockaddr_in));
    socketDescriptor.sin_family = AF_INET;
    socketDescriptor.sin_port = htons(port);
    socketDescriptor.sin_addr.s_addr = INADDR_ANY;
    if (bind(socketFD,(sockaddr*)&socketDescriptor,sizeof(socketDescriptor)) < 0)
        throw std::string("Unable to bind socket to requested port");

    // Set up a maximum number of pending connections to accept
    listen(socketFD,5);
    /* SetFD(socketFD); */
    this->socketFD = socketFD;
    // At this point, the object is initialized.  So return.
}
SocketServer::~SocketServer(void)
{
    Shutdown();
}

SocketModule::Socket SocketServer::Accept(void)
{
    SocketModule::Socket sock(socketFD);
    Sync::Event terminator = sock.terminator;
    Sync::FlexWait waiter(2,sock,&terminator);
    Sync::Blockable * result = waiter.Wait();

    if (result == &terminator)
    {
        terminator.Reset();
        throw TerminationException(2);
    }

    if (result == &sock)
    {
        int connectionFD = accept(sock.GetFD(),NULL,0);
        if (connectionFD < 0)
        {
            throw std::string("Unexpected error in the server");
        }
        return SocketModule::Socket(connectionFD);
    }
    else
        throw std::string("Unexpected error in the server");
}

void SocketServer::Shutdown(void)
{
    SocketModule::Socket sock(this->socketFD);
    close(this->socketFD);
    shutdown(this->socketFD,SHUT_RDWR);
    sock.terminator.Trigger();
}

