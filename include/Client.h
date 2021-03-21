#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "Socket.h"

class Client{
    private:
        Socket sock;
    protected:
        string read();
        void write(string message);
        void connect();
    public:
        Client(std::string file);
        void run();
        ~Client();
};

#endif