#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "utils.hpp"
#include "signal_handler.hpp"

static int32_t query (int connfd, const char* text);
static int32_t pipelined_query(int connfd, std::vector<std::string>& queries);

#endif
