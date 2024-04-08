#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include "thread.h"
#include "socketserver.h"
#include "socket.h"
#include <mutex>
#include <algorithm>
#include <atomic> 


constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

std::string message;
std::mutex message_mutex;
bool shutdown_requested = false;
std::mutex shutdown_mutex;

void clientHandler(SocketModule::Socket theSocket) {
    SocketModule::ByteArray bytes;
    
    while (true) {
        int read = theSocket.Read(bytes);
        if (read <= 0)
            break;

        {
            std::lock_guard<std::mutex> lock(message_mutex);
            // Append the new message to the existing message with a space in between
            message += bytes.ToString() + " ";
        }

        // Trim trailing spaces from the received message
        std::string received_message = bytes.ToString();
        received_message = received_message.substr(0, received_message.find_last_not_of(" ") + 1);

        // Check if the received message is "QUIT"
        if (received_message == "QUIT") {
            std::lock_guard<std::mutex> lock(shutdown_mutex);
            shutdown_requested = true;
            break;
        }


        // Send the entire message back to the client
        {
            std::lock_guard<std::mutex> lock(message_mutex);
            SocketModule::ByteArray response(message);
            theSocket.Write(response);
        }
    }

    theSocket.Close();
}

int main() {
    SocketServer theServer(PORT);

    //Lets user know the game is running
    std::cout << "The Game is now running"<< std::endl;

    std::vector<std::thread> client_threads;

    while (true) {
        try
        {
            SocketModule::Socket newSocket = theServer.Accept();
            std::cout << "Received a socket connection!" << std::endl;

            client_threads.emplace_back(clientHandler, newSocket);
        }
        catch(TerminationException e)
        {
            std::cout << "The socket server is no longer listening. Exiting now." << std::endl;
            break;
        }
        catch(std::string s)
        {
            std::cout << "thrown " << s << std::endl;
            break;
        }
        catch(...)
        {
            std::cout << "caught  unknown exception" << std::endl;
            break;
        }

        // Check if shutdown is requested
        {
            std::lock_guard<std::mutex> lock(shutdown_mutex);
            if (shutdown_requested) {
                std::cout << "Shutting down server..." << std::endl;
                break;
            }
        }
    }

    // Wait for client threads to finish
    for (auto& thread : client_threads) {
        thread.join();
    }

    return 0;
}
