#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "utils.hpp"
#include "signal_handler.hpp"

static int32_t query (int connfd, const char* text);

#endif
