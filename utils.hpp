#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <cassert>
#include "logger.hpp"
#include <arpa/inet.h>   
#include <netinet/in.h>  
#include <cstring>       
#include <sstream>

/**
 * Prints a message
 * Kills the process
 */
void die (std::string);

/**
 * Prints the received string using cout
 * Prints a new line at the end
 */
void print (std::string&&);

/**
 * Prints the received string using cerr
 * Prints a new line at the end
 */
void error (std::string);

std::string get_client_info_string(const struct sockaddr_in& client_addr);
std::string get_client_info_string_from_conn_fd(int fd);
std::string address_to_string(const void* addr);
std::string want_read_write(int read, int write);
#endif
