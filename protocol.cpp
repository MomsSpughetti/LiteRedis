
#include "protocol.h"
#include <sstream>

/**
 * @brief Reads all n bytes from fd to buf, uses read().
 * @param fd file descriptor
 * @param buf pointer to a buffer of size n at least.
 * @param n Number of bytes to read.
 */
int32_t read_full (int fd, char *buf, size_t n) {

    while (n > 0) {
        LOG_INFO("Now calling read()");
        ssize_t rv = read(fd, buf, n);
        LOG_INFO("Now read() Finished");
        if (rv <= 0) {
            return -1; // error
        }

        assert((size_t)rv <= n);
        assert((size_t)rv > 0);

        LOG_INFO("[read_full] Updating buf from: " + address_to_string(buf));
        buf += rv;
        LOG_INFO("to: " + address_to_string(buf));
        LOG_INFO("[read_full] Updating n from: " + std::to_string(n));
        n -= (size_t)rv;
        LOG_INFO("to: " + std::to_string(n));
    }

    return 0;
}

/**
 * @brief Writes all n bytes from fd to buf, uses read().
 * @param fd file descriptor
 * @param buf pointer to a buffer of size n at least.
 * @param n Number of bytes to read.
 */
int32_t write_all (int fd, const char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0) {
            return -1; // error
        }
        
        assert((size_t)rv <= n);
        buf += rv;
        n -= (size_t)rv;
    }
    return 0;
}


int32_t send_msg(int fd, const char* buf) {
    // send to server
    uint32_t msg_len = (uint32_t)strlen(buf);
    if (msg_len > k_max_msg_len) {
        return -1;
    }

    char wbuf[k_msg_size_len + msg_len];
    memcpy(wbuf, &msg_len, k_msg_size_len);
    memcpy(wbuf + k_msg_size_len, buf, msg_len);
    if (int32_t err = write_all(fd, wbuf, k_msg_size_len + msg_len)) {
        return err;
    }

    return 0;
}

/**
 * @param rbuf a buffer of length (k_max_msg_len + k_msg_size_len)
 * @param msg_len_ a pointer where the len of the message body will be saved
 */
int32_t read_msg(int fd, char* rbuf, uint32_t* msg_len_) {
    // read message len
    errno = 0;
    int32_t err = read_full(fd, rbuf, k_msg_size_len);
    if (err) {
        LOG_ERROR(errno == 0 ? "EOF" : "[read_msg]: reading message length failed");
        return err;
    }

    // read
    uint32_t msg_len = 0;
    memcpy(&msg_len, rbuf, k_msg_size_len);
    if (msg_len > k_max_msg_len) {
        LOG_ERROR("[read_msg]: Message too long: " + std::to_string(msg_len));
        return -1;
    }

    // reply body
    err = read_full(fd, rbuf + k_msg_size_len, msg_len);
    if (err) {
        LOG_ERROR("[read_msg]: reading message failed");
        return err;
    }

    *msg_len_ = msg_len;

    return 0;
}