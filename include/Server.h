#ifndef SERVER_H
#define SERVER_H

#include "Socket.h"

class Server{
private:
    Socket sock;
    int client;
protected:
    string read();
    void write(string message);
    int listen();
public:
    Server(std::string file);
    void run();
    ~Server();
};


#endif