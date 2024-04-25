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
#include "Story.h"

std::vector<Story::Shared::User> gameUsers = {
  {"user1_id"},
  {"user2_id"}
};

Story::Shared::Game exampleGame = {
  "game1_id",          // Game ID
  Story::Shared::GameStatus::ACTIVE,  // Game Status
  gameUsers            // Vector of Users
};

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

bool shutdown_requested = false;
std::mutex shutdown_mutex;


class GameThread : public Thread {
  private:
    SocketModule::Socket gameSocket;

  public:
    GameThread(SocketModule::Socket const & socket) : Thread(true),gameSocket(socket) {}
    ~GameThread() override {
      std::cout << "GameThread destructor" << std::endl;
    }

    long ThreadMain() override {
        std::cout << "Game Thread started!" << std::endl;
        bool running(true);
        bool initialGameSent(false);

        while (running) {
          if (!initialGameSent) {
            json j = exampleGame;
            SocketModule::ByteArray ba(j.dump(-1,' ', true));
            gameSocket.Write(ba);
            initialGameSent = true;
          }
        } 
        // Game logic here
        // For example, handle game communication using the gameSocket

        // After game logic is done, close the socket
        gameSocket.Close();

        return 0;
    }
};

std::vector<GameThread *> gameThreads = {};

class ClientThread : public Thread {
  private:
    SocketModule::Socket clientSocket;

  public:
    ClientThread(SocketModule::Socket const & socket) : Thread(true),clientSocket(socket) {}
    ~ClientThread() override {
      std::cout << "ClientThread destructor" << std::endl;
    }
    long ThreadMain() override {
      std::cout << "New client handler thread started" << std::endl;
      SocketModule::ByteArray bytes;
      std::string message("");
      bool running(true);
      
      while (running) {
        int read = clientSocket.Read(bytes);
        if (read <= 0)
            break;

        {
            /* std::lock_guard<std::mutex> lock(message_mutex); */
            message += bytes.ToString();
        }

        // Trim trailing spaces from the received message
        std::string received_message = bytes.ToString();
        received_message = received_message.substr(0, received_message.find_last_not_of(" ") + 1);
        std::cout << "We have received this message: " << received_message << std::endl;

        // Check if the received message is "QUIT"
        /* if (received_message == "QUIT") { */
        /*     std::lock_guard<std::mutex> lock(shutdown_mutex); */
        /*     shutdown_requested = true; */
        /*     break; */
        /* } */

        if(received_message == "c"){
            std::cout << "Received create game" << std::endl;
            gameThreads.emplace_back(new GameThread(clientSocket));
            /* running = false; */
        }

        /*
            if received message starts with 'j:'
            extract the game_id after ':'
            do a lookup against the existing games
            (recommendation: have a global variable that is a vector of shared pointers to game objects)
            server write back to client
        */
      }
      std::cout << "Client handler thread complete" << std::endl;       
      return 0;
    }
};

std::vector<ClientThread *> clientThreads = {};

int main() {
    json j = exampleGame;
    std::cout << j.dump(4) << std::endl;

    SocketServer theServer(PORT);

    std::cout << "Server Started" << std::endl;


    while (true) {
        try
        {
            SocketModule::Socket newSocket = theServer.Accept();
            std::cout << "Received a socket connection!" << std::endl;

            clientThreads.emplace_back(new ClientThread(newSocket));
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
            std::cout << "caught unknown exception" << std::endl;
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
    for (auto& thread : clientThreads) {
      delete thread;
    }
    for (auto& thread : gameThreads) {
      delete thread;
    }

    return 0;
}
