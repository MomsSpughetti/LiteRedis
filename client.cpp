#include "client.hpp"
#include "protocol.h"

static int32_t query (int connfd, const char* text) {

    print("Sending this to server: " + std::string(text));
    // send to server
    uint32_t rv = send_msg(connfd, text);
    if (rv) {
        return rv;
    }

    char rbuf[k_max_msg_len + k_msg_size_len];
    uint32_t msg_len;
    rv = read_msg(connfd, rbuf, &msg_len);
    if (rv) {
        return rv;
    }

    // handle client request
    print("Server says:" + std::string(rbuf + k_msg_size_len, msg_len));
    return 0;
}

int main () {

    // ignore Broken Pipe Error
    ignore_sig_pipe();
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket() call failed");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1

    int rv = connect(fd, (const struct sockaddr*)&addr, sizeof(addr));
    if (rv < 0) {
        die("connect() call failed");
    }

    char msg[] = "Hello server";
    int32_t err = query(fd, msg);
    if (err) {
        goto L_DONE;
    }
    err = query(fd, msg);
    if (err) {
        goto L_DONE;
    }
    err = query(fd, msg);
    if (err) {
        goto L_DONE;
    }

L_DONE:
    close(fd);
    return 0;
}