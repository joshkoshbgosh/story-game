#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <atomic> // Add atomic header

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

std::string message;
std::mutex message_mutex;
std::atomic<int> period_count(0); // Make period_count atomic
bool shutdown_requested = false;
std::mutex shutdown_mutex;

void clientHandler(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    int valread;

    while (true) {
        valread = read(client_socket, buffer, BUFFER_SIZE);
        std::cout << period_count << std::endl; //Period count stays 0. It needs to change
        if (valread <= 0)
            break;

        {
            std::lock_guard<std::mutex> lock(message_mutex);
            message += buffer;
        }

        // Trim trailing spaces from the received message
        std::string received_message(buffer, valread);
        received_message = received_message.substr(0, received_message.find_last_not_of(" ") + 1);

        // Check if the received message is "SHUTDOWN"
        if (received_message == "SHUTDOWN") {
            std::lock_guard<std::mutex> lock(shutdown_mutex);
            shutdown_requested = true;
            break;
        }

        // Count periods in the received buffer
        period_count += std::count(buffer, buffer + valread, '.');


        send(client_socket, message.c_str(), message.length(), 0);
        memset(buffer, 0, BUFFER_SIZE);
    }

    close(client_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //Lets user know the game is running
    std::cout << "The Game is now running"<< std::endl;

    std::vector<std::thread> client_threads;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        client_threads.emplace_back(clientHandler, new_socket);

        // Check if shutdown is requested
        {
            std::lock_guard<std::mutex> lock(shutdown_mutex);
            if (shutdown_requested) {
                std::cout << "Shutting down server..." << std::endl;
                break;
            }
        }
    }

    // Close server socket
    close(server_fd);

    // Wait for client threads to finish
    for (auto& thread : client_threads) {
        thread.join();
    }

    return 0;
}