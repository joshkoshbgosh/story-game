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

//Game vector
std::vector<std::shared_ptr<Story::Shared::Game>> gameVector = {}; 



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
        std::string name = received_message.substr(1,4);
        std::cout << "We have received this message: " << name << std::endl;

        // Check if the received message is "QUIT"
        /* if (received_message == "QUIT") { */
        /*     std::lock_guard<std::mutex> lock(shutdown_mutex); */
        /*     shutdown_requested = true; */
        /*     break; */
        /* } */

        //If user creates a new message
        if(received_message.length() >= 5 && received_message.substr(0,1) == "c"){
            std::string username = received_message.substr(1,4);
            std::cout << "Received create game from "<< username << std::endl;
            //gameThreads.emplace_back(new GameThread(clientSocket));
            /* running = false; */

            Story::Shared::Game newGame = {
            "game1_id",          // Game ID
            Story::Shared::GameStatus::PENDING,  // Game Status
            {{username}}            // Vector of Users
            };

            // Add the new game to the game vector
            gameVector.push_back(std::make_shared<Story::Shared::Game>(newGame));
        }

        //If user decides to join an existing game
        if (received_message.length() >= 7 && received_message.substr(0, 2) == "j:"){
            std::cout << "Received Join Game " << std::endl;
            std::string game_id = received_message.substr(6);
            std::string username = received_message.substr(2,5);
            std::cout << username << "is joining game ID:" << game_id << std::endl;

            //Search the games vector for the existing game
            auto it = std::find_if(gameVector.begin(), gameVector.end(), [&game_id](const std::shared_ptr <Story::Shared::Game>& game) {
            return game->id == game_id;
            });

            // Check if the game was found
            if (it != gameVector.end()) {
            // Add the username to the user vector in the game
            (*it)->users.push_back({received_message.substr(6)});
            std::cout << received_message.substr(6) << " is joining game ID: " << game_id << std::endl;
            } else {
              std::cout << "Error: Game with ID " << game_id << " not found!" << std::endl;
            }


            /*
            should receive message that includes game id
            And look up and see if any of the game objects have a matching game id
            send error message if no game exist
            if game exists, you should pass the client socket to the game thread
            Then game thread should update the state and broadcast the new gamestate to users
            Alternative to socket to thread is to create game thread when all users have entered the game
            map game_id to sockets for each user and start game when all users are ready
            */
          }
            
  
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
