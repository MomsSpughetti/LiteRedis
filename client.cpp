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
    print("\nServer says:\n" + std::string(rbuf + k_msg_size_len, msg_len));
    return 0;
}

/**
 * @details A function for sending pipelined requests
 * @param queries vector<string>
 * @param connfd The file descriptor of the connection with the server
 */
static int32_t pipelined_query (int connfd, std::vector<std::string>& queries) {

    for (auto& query : queries) {
        print("Sending this to server: " + query);

        uint32_t rv = send_msg(connfd, query.data());
        if (rv) {
            return rv;
        }

    }

    char rbuf[k_max_msg_len + k_msg_size_len];
    uint32_t msg_len;

    for (uint32_t i = 0; i < queries.size(); ++i) {

        uint32_t rv = read_msg(connfd, rbuf, &msg_len);
        if (rv) {
            return rv;
        }

        print("Server says:\n" + std::string(rbuf + k_msg_size_len, msg_len));
    }
    return 0;
}

int main () {

    // ignore Broken Pipe Error
    //ignore_sig_pipe();
    
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

    std::vector<std::string> queries = {
        "How many hours Da Vincci spent on the Monalisa?",
        "What is the name of the assassinator of John f Keneddy?",
        std::string(k_max_msg_len - 100, 'a'),
        std::string(k_max_msg_len - 100, 'b'),
        std::string(k_max_msg_len - 100, 'c'),
        std::string(k_max_msg_len - 100, 'd'),
        std::string(k_max_msg_len - 100, 'e'),
        std::string(k_max_msg_len - 100, 'f'),
        "How old is the sun of out solar system?"
    };

    int32_t err = pipelined_query(fd, queries);

    if (err) {
        goto L_DONE;
    }

L_DONE:
    close(fd);
    return 0;
}