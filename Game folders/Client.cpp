#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

std::atomic<bool> has_input(false); // Flag to track if any client has entered input
std::mutex input_mutex;


void clientTask(int client_id) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "\n Socket creation error for client " << client_id << std::endl;
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        std::cerr << "\nInvalid address/ Address not supported for client " << client_id << std::endl;
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "\nConnection Failed for client " << client_id << std::endl;
        return;
    }

    std::string input;
    while (true) {
        std::lock_guard<std::mutex> lock(input_mutex);
        if (!has_input) {
            std::cout << "Any client: Enter message (Enter 'QUIT' to leave'): ";
            has_input=true;
        } else {
            std::cout << "Client " << client_id << ": Enter message (Enter 'QUIT' to leave'): ";
        }

        std::getline(std::cin, input);
        has_input = true;

        // Check if the user wants to quit
        if (input == "QUIT") {
            std::cout <<"Goodbye Player\n";
            break; // Exit the loop
            
        }

         // Add a space at the end of the input
        input += " ";

        send(sock, input.c_str(), input.length(), 0);
        valread = read(sock, buffer, BUFFER_SIZE);
        std::cout << "Client " << client_id << ": Server response: " << buffer << std::endl;
        memset(buffer, 0, BUFFER_SIZE);
    }

    close(sock); // Close the socket before exiting
}

int main(int argc, char const *argv[]) {
    std::vector<std::thread> client_threads;

    // Number of clients to run concurrently
    int num_clients = 5;

    // Create threads for each client
    for (int i = 0; i < num_clients; ++i) {
        client_threads.emplace_back(clientTask, i + 1);
    }

    // Join threads
    for (auto& thread : client_threads) {
        thread.join();
    }

    return 0;
}