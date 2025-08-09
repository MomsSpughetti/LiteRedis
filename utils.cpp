#include "utils.hpp"


void die (std::string str) {
    LOG_ERROR(str);
    exit(1);
}

void print (std::string str) {
    std::cout << str << std::endl;
}

std::string get_client_info_string(const struct sockaddr_in& client_addr) {
    char ip[INET_ADDRSTRLEN];

    // Convert IP to string
    if (!inet_ntop(AF_INET, &(client_addr.sin_addr), ip, INET_ADDRSTRLEN)) {
        return "Invalid IP address";
    }

    int port = ntohs(client_addr.sin_port);

    std::ostringstream oss;
    oss << "Client IP: " << ip << ", Port: " << port;

    return oss.str();
}