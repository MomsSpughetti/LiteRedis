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




void fd_set_non_blocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}


void handle_read(Conn* conn) {

    // 1. perform a non-blocking read - the socket was set to non blocking using fd_set_non_blocking
    uint8_t buf[64 * 1024];
    ssize_t rv = read(conn->fd, buf, sizeof(buf));

    if (rv <= 0) { // IO error or EOF
        conn->want_close = true;
        return;
    }

    // 2. append new data to connection incoming-buffer
    buf_append(conn->incoming, buf, (size_t)rv);

    // 3. try parsing the data accumulated buffer according to the protocol
    // 4. Process the parsed message
    // 5. Remove the mesage from Conn::incoming

    try_one_request(conn);

    // ...

    /**
     * Here we make sure that the status of each connection is either reading ot writing, not both
     * That's the protocol we defined.
     * why?
     * The priority is given for writing
     */
    if (conn->outgoing.size() > 0) {
        conn->want_read = false;
        conn->want_write = true;
    }

}

void buf_append(std::vector<uint8_t> &buf, const uint8_t *data, size_t len) {
    buf.insert(buf.end(), data, data + len);
}

void buf_consume (std::vector<uint8_t> &buf, size_t len) {
    buf.erase(buf.begin(), buf.begin() + len);
}


void handle_write(Conn *conn) {
    assert(conn->outgoing.size() > 0);
    int rv = write(conn->fd, conn->outgoing.data(), conn->outgoing.size());
    if (rv <= 0) {
        conn->want_close = true;
        return;
    }

    // remove the written data from Conn::outgoing buffer
    buf_consume(conn->outgoing, rv);

    // ...

    if (conn->outgoing.size() == 0) {
        // The priority is given for writing
        conn->want_read = true;
        conn->want_write = false;
    }
}

/**
 * @details Process one request if there is enough data
 */
bool try_one_request(Conn* conn) {
    // retrieve len
    // Protocol: Message header
    uint32_t packet_body_len = 0;

    if (conn->incoming.size() < k_msg_size_len) {
        // need more data
        return false;
    }

    memcpy(&packet_body_len, conn->incoming.data(), k_msg_size_len);

    // process body
    // Protocol: Body
    if (packet_body_len > k_max_msg_len) {
        LOG_ERROR("Message too long!");
        conn->want_close = true;
        return false;
    } else if (packet_body_len > (conn->incoming.size() - k_max_msg_len)) {
        // not enough data
        return false;
    }

    // Process request
    const uint8_t* request = &conn->incoming[k_msg_size_len];
    print("\nClient says: \n" + std::string((char*)conn->incoming.data() + k_msg_size_len, packet_body_len));

    // Generate response for client
    uint8_t reply[] = "\nI got your message:\n";
    uint32_t response_body_len = sizeof(reply) - 1 + packet_body_len;
    buf_append(conn->outgoing, (const uint8_t*)&response_body_len, k_msg_size_len);
    buf_append(conn->outgoing, reply, sizeof(reply) - 1);
    buf_append(conn->outgoing, request, packet_body_len);

    // remove the processed message from Conn::incoming
    buf_consume(conn->incoming, k_msg_size_len + packet_body_len);
    return true; // success
}

/**
 * After this call, the connection socket of fd status will as wasnt_read:
 * Waiting for the server to read its message.
 * The socket will also be set to non-blocking.
 * @param fd The fd where the server is listening
 * @return Conn struct - A new object representing the new connection and its current status
 */
Conn* handle_accept(int fd) {
    // accept
    LOG_INFO("Waiting for a new request.");
    struct sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr*)&client_addr, &addrlen); // The addrlen argument is both the input and output size
    if (connfd < 0) {
        return nullptr;;
    }
    LOG_INFO("A new connection request received from: " + get_client_info_string(client_addr));

    // set new connection fd to be non-blocking (Makes sense only for sockets fds)
    fd_set_non_blocking(connfd);

    // Add connection to fd2conn
    Conn* conn = new Conn();
    conn->fd = connfd;
    conn->want_read = true;

    return conn;

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

    // Mapping from fd to the corresponding connection object
    std::vector<Conn *> fd2conn;

    // the event loop
    std::vector<struct pollfd> poll_args;
    
    while (true) {
        // Prepare arguments for poll
        poll_args.clear();
        // listening socket at index 0
        struct pollfd pfd = {fd, POLLIN, 0};
        poll_args.push_back(pfd);

        // handle connection sockets
        for (Conn *conn : fd2conn) {
            if (!conn) {
                continue;
            }

            struct pollfd pfd = {conn->fd, POLLERR, 0};

            if (conn->want_read) {
                pfd.events |= POLLIN;
            }

            if (conn->want_write) {
                pfd.events |= POLLOUT;
            }

            poll_args.push_back(pfd);
        }

        // wait for readiness
        int rv = poll(poll_args.data(), (nfds_t)poll_args.size(), -1);
        if (rv < 0 && errno == EINTR) {die("poll() failed!");
            continue;
        }

        if (rv < 0) {
            die("poll() failed!");
        }

        // handle the listening socket
        if (poll_args[0].revents) {
            if (Conn *conn = handle_accept(fd)) {
                // insert to the map
                if ((size_t)conn->fd >= fd2conn.size()) {
                    fd2conn.resize(conn->fd + 1);
                }

                fd2conn[conn->fd] = conn;
            }
        }

        // handle connection sockets

        for (int i = 1; i < poll_args.size(); ++i) {
            uint32_t ready = poll_args[i].revents;
            Conn* conn = fd2conn[poll_args[i].fd];
            if (ready & POLLIN) {
                handle_read(conn); // application logic
            }

            if (ready & POLLOUT) {
                handle_write(conn); // application logic
            }

            // close the socket from socket error or pplication logic
            if ((ready & POLLERR) || conn->want_close) {
                LOG_INFO("Closed the connection with: " + get_client_info_string_from_conn_fd(conn->fd));
                close(conn->fd);
                fd2conn[conn->fd] = nullptr;
                delete conn;
            }
        }



    }
}


/**
 * Temp:
 */
int _main (int argc, char* argv[]) {

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
