#include "server.hpp"
#include <signal.h>

/**
 * @brief Handles a single request. A single packet is expected to have the following shape:
 * 
 * [  size  , msg], when size is 4B long.
 */
static int32_t one_request (int connfd) {

    // read from client
    char rbuf[k_max_msg_len + k_msg_size_len];
    uint32_t msg_len = 0;
    uint32_t rv = read_msg(connfd, rbuf, &msg_len);
    if (rv) {
        return rv;
    }

    // handle client request
    print("Client says:" + std::string(&rbuf[k_msg_size_len], msg_len));

    LOG_INFO("Replying to client.");
    // reply to client using same protocol
    const char reply[] = "Hi client, I received your message.";
    // send to server
    return send_msg(connfd, reply);
}

/**
 * Simple read - write
 */
void do_something (int connfd) {

    //read
    char rbuf[64] = {};
    ssize_t n = recv(connfd, rbuf, sizeof(rbuf) - 1, 0); // is -1 needed?
    if (n < 0) {
        LOG_ERROR("read() error");
        return;
    }

    print("Client says: \n" + std::string(rbuf, n));

    // write
    char wbuf[] = "world";
    send(connfd, wbuf, strlen(wbuf), 0);

}


int main (int argc, char* argv[]) {

    // Default to ERROR level
    SimpleLogger::instance().setLevel(LogLevel::ERROR);

    // Check for --debug flag anywhere in args
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--debug") {
            SimpleLogger::instance().setLevel(LogLevel::INFO);
            break;
        }
    }

    // Ignoring SIGPIPE globally using the modern recommended way
    ignore_sig_pipe();

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {};


    addr.sin_family = AF_INET;

    /**
     * htons - host to network short (16 bit)
     * htonl -  .    .    .    long (32 bit)
     */

    addr.sin_port = htons(1234); // port - host format to big endian format
    addr.sin_addr.s_addr = htonl(0); // IP 0.0.0.0

    // bind
    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) { die("bind() failed"); }

    // listen
    rv = listen(fd, SOMAXCONN);
    if (rv) { die("listen() failed");}

    // Accept connections
    while (true) {
        // accept
        LOG_INFO("Waiting for a new request.");
        struct sockaddr_in client_addr = {};
        socklen_t addrlen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr*)&client_addr, &addrlen); // The addrlen argument is both the input and output size

        if (connfd < 0) {
            continue;
        }
        LOG_INFO("A new connection request received from: " + get_client_info_string(client_addr));

        while (true) {
            LOG_INFO("Waiting for the client to send a message ...");
            uint32_t err = one_request(connfd);
            if (err) {
                break;
            }
        }

        close(connfd);
        LOG_INFO("Closed the connection with: " + get_client_info_string(client_addr));

    }

    return 0;
}
