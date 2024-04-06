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

std::atomic<bool> has_input(false);
std::mutex input_mutex;
std::atomic<int> period_count(0);

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
        std::lock_guard<std::mutex> lock(input_mutex);
        std::cout << period_count << std::endl;

       
        std::cout << "Client " << client_id << ": Enter message (Enter 'QUIT' to leave'): ";

        std::getline(std::cin, input);
        has_input = true;

        if (input == "QUIT") {
            std::cout <<"Goodbye Player\n";
            close(sock);
            break;
        }


        for (char c : input) {
            if (c == '.') {
                period_count++;
                if (period_count >= 7) {
                    std::cout << "Client " << client_id << ": Requesting server shutdown..." << std::endl;
                    send(sock, "SHUTDOWN", 8, 0);
                    close(sock);
                    return;
                }
            }
        }

        input += " ";
        send(sock, input.c_str(), input.length(), 0);
        valread = read(sock, buffer, BUFFER_SIZE);
        std::cout << "Client " << client_id << ": Server response: " << buffer << std::endl;
        memset(buffer, 0, BUFFER_SIZE);
    }

    close(sock);
}

int main(int argc, char const *argv[]) {
    std::vector<std::thread> client_threads;
    int num_clients = 5;

    for (int i = 0; i < num_clients; ++i) {
        client_threads.emplace_back(clientTask, i + 1);
    }

    for (auto& thread : client_threads) {
        thread.join();
    }

    return 0;
}