/**
 * 
 * [ size , msg ], [ size , msg ], ...
 * 
 * 
 * A single request:
 * 
 *  < - 4B - >
 * [  size   , ...]
 */

#ifndef _PROTOCOL_HPP
#define _PROTOCOL_HPP

#include <stdlib.h>
#include <iostream>
#include "utils.hpp"
#include <cassert>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

const size_t k_max_msg_len = 4096;
const size_t k_msg_size_len = 4;

int32_t read_full (int fd, char *buf, size_t n);
int32_t write_all (int fd, const char *buf, size_t n);

/**
 * @param buf a string expected to end with '\0'
 */
int32_t send_msg(int fd, const char* buf);
int32_t read_msg(int fd, char* buf, uint32_t* msg_len_);

#endif