g++ -Wall -Wextra -O2 -g -fno-stack-protector server.cpp utils.cpp protocol.cpp signal_handler.cpp -o server
g++ -Wall -Wextra -O2 -g -fno-stack-protector client.cpp utils.cpp protocol.cpp signal_handler.cpp -o client
