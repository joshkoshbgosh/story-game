#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <sstream>
#include "socket.h"

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

std::mutex input_mutex;

void clientTask(int client_id) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "\n Socket creation error for client " << client_id << std::endl;
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        std::cerr << "\nInvalid address/ Address not supported for client " << client_id << std::endl;
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "\nConnection Failed for client " << client_id << std::endl;
        close(sock);
        return;
    }

    std::string input;
    while (true) {
        std::cout << "Client " << client_id << ": Enter message (Enter 'QUIT' to leave'): ";

        std::string input_line;
        std::getline(std::cin, input_line);

        {
            std::lock_guard<std::mutex> lock(input_mutex);
            input = input_line;
        }

        if (input == "QUIT") {
            std::cout << "Goodbye Player\n";
            close(sock);
            break;
        }

        input += " ";
        send(sock, input.c_str(), input.length(), 0);
        int valread = read(sock, buffer, BUFFER_SIZE);
        std::cout << "Client " << client_id << ": Server response: " << buffer << std::endl;
        memset(buffer, 0, BUFFER_SIZE);
    }

    close(sock);
}

int main(int argc, char const *argv[]) {
    std::vector<std::thread> client_threads;

    for (int i = 0; i < 5; ++i) {
        client_threads.emplace_back(clientTask, i + 1);
    }

    for (auto& thread : client_threads) {
        thread.join();
    }

    return 0;
}
