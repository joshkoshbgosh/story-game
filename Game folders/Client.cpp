#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <atomic>
#include <mutex>
#include <sstream>
#include "socket.h"
#include "Story.h"
#include <optional>

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

int sock = 0;

char buffer[BUFFER_SIZE] = {0};

struct sockaddr_in serv_addr;

std::mutex input_mutex;

Story::Client::ClientState clientState;

/* void clientTask(int client_id) { */
/*     if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) { */
/*         std::cerr << "\n Socket creation error for client " << client_id << std::endl; */
/*         return; */
/*     } */
/**/
/*     serv_addr.sin_family = AF_INET; */
/*     serv_addr.sin_port = htons(PORT); */
/**/
/*     if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) { */
/*         std::cerr << "\nInvalid address/ Address not supported for client " << client_id << std::endl; */
/*         close(sock); */
/*         return; */
/*     } */
/**/
/*     if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { */
/*         std::cerr << "\nConnection Failed for client " << client_id << std::endl; */
/*         close(sock); */
/*         return; */
/*     } */
/**/
/*     std::string input; */
/*     while (true) { */
/*         std::cout << "Client " << client_id << ": Enter message (Enter 'QUIT' to leave'): "; */
/**/
/*         std::string input_line; */
/*         std::getline(std::cin, input_line); */
/**/
/*         { */
/*             std::lock_guard<std::mutex> lock(input_mutex); */
/*             input = input_line; */
/*         } */
/**/
/*         if (input == "QUIT") { */
/*             std::cout << "Goodbye Player\n"; */
/*             close(sock); */
/*             break; */
/*         } */
/**/
/*         input += " "; */
/*         send(sock, input.c_str(), input.length(), 0);  */
/*         int valread = read(sock, buffer, BUFFER_SIZE); */
/*         std::cout << "Client " << client_id << ": Server response: " << buffer << std::endl; */
/*         std::string response(buffer); */
/*         try { */
/*           json jsonResponse = json::parse(response); */
/*           std::cout << jsonResponse.dump(4) << std::endl; */
/*         } catch (...) { */
/*           std::cerr << "Error parsing json response from server" << std::endl; */
/*         } */
/*         memset(buffer, 0, BUFFER_SIZE); */
/*     } */
/**/
/*     close(sock); */
/* } */

/* template<Story::Client::ActionType T> */
/* void dispatch(Story::Client::Action<T> action) { */
/*   clientState = Story::Client::handleAction(clientState, action);  */
/* } */

/*
sends a socket message to the server, 
waits for response, 
should receive a json string representing the game it should parse the string and store the created game in a global variable
*/ 
void createGame() {
  std::cout << "Creating game..." << std::endl;
  std::string message = "c";
  send(sock, message.c_str(), message.length(), 0);
  std::cout << "Sent create game socket message" << std::endl;

  int valread = read(sock, buffer, BUFFER_SIZE);
  std::cout << "Server response: " << buffer << std::endl;
  std::string response(buffer);
  try {
    json jsonResponse = json::parse(response);
    std::cout << jsonResponse.dump(4) << std::endl;
  } catch (...) {
    std::cerr << "Error parsing json response from server" << std::endl;
  }
  memset(buffer, 0, BUFFER_SIZE);
}

/*
receives parameter game_id
sends game_id to server
server looks up all its games and finds match
if match found, server should connect the client socket to that game thread
if match not found, server should return error
and client should display that error
*/
void joinGame(std::string game_id) {

}

int main(int argc, char const *argv[]) {
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "\n Socket creation error for client " << std::endl;
        return 0;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        std::cerr << "\nInvalid address/ Address not supported for client " << std::endl;
        close(sock);
        return 0;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "\nConnection Failed for client " << std::endl;
        close(sock);
        return 0;
    }

    std::string username;
    std::string createOrJoin;
    bool isCreateOrJoinSelected(false); 
    std::cout << "Welcome to the story game! What was your name again?" << std::endl;
    std::getline(std::cin, username);

    while (!isCreateOrJoinSelected) {
      std::cout << "Welcome " << username << "! Would you like to create or join a game? (c/j):" << std::endl;
      std::getline(std::cin, createOrJoin);
      
      if (createOrJoin == "c") {
        isCreateOrJoinSelected = true;
        createGame();
      } else if (createOrJoin == "j") {
        std::string game_id;
        std::cout << "Enter Game ID: " << std::endl;
        getline(std::cin, game_id);
        joinGame(game_id);
      } else {
        std::cout << "Invalid option" << std::endl;
      }
    }
     

    std::string input;
    while (true) {
        /* std::cout << "Client " << ": Enter message (Enter 'QUIT' to leave'): "; */
        /**/
        /* std::string input_line; */
        /* std::getline(std::cin, input_line); */
        /**/
        /* { */
        /*     std::lock_guard<std::mutex> lock(input_mutex); */
        /*     input = input_line; */
        /* } */
        /**/
        /* if (input == "QUIT") { */
        /*     std::cout << "Goodbye Player\n"; */
        /*     close(sock); */
        /*     break; */
        /* } */
        /**/
        /* input += " "; */
        /* send(sock, input.c_str(), input.length(), 0);  */
        /* int valread = read(sock, buffer, BUFFER_SIZE); */
        /* std::cout << "Server response: " << buffer << std::endl; */
        /* std::string response(buffer); */
        /* try { */
        /*   json jsonResponse = json::parse(response); */
        /*   std::cout << jsonResponse.dump(4) << std::endl; */
        /* } catch (...) { */
        /*   std::cerr << "Error parsing json response from server" << std::endl; */
        /* } */
        /* memset(buffer, 0, BUFFER_SIZE); */
    }
    /* close(sock); */
    return 0;
}
