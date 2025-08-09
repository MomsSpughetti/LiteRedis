#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "utils.hpp"
#include "protocol.h"
#include "logger.hpp"
#include "signal_handler.hpp"
#include <poll.h>
#include <fcntl.h>

/* Structs */
struct Conn {
    int fd = -1;
    // Application's intention, for event loop
    // question: who is responsible for setting these?
    bool want_read = false;
    bool want_write = false;
    bool want_close = false;
    // buffered input & output
    std::vector<uint8_t> incoming;
    std::vector<uint8_t> outgoing;
};

/* Functions */
void do_something (int);
static int32_t one_request (int connfd);
Conn* handle_accept(int fd);
void handle_read(Conn* conn);
void handle_write(Conn* conn);
void fd_set_non_blocking(int fd);
void buf_append(std::vector<uint8_t> &buf, const uint8_t *data, size_t len);
bool try_one_request(Conn *conn);
void buf_consume(std::vector<uint8_t> &buf, size_t len);
#endif