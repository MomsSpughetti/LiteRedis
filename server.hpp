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

/* Functions */
void do_something (int);
static int32_t one_request (int connfd);


#endif