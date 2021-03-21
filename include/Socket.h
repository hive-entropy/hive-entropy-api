#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/socket.h>
#include <sys/un.h>
#include <string>

using namespace std;

class Socket
{
private:
    sockaddr_un sockAddr;
    int fd;
    char buffer[1024];
    string path;
public:
    Socket(string path);
    ~Socket();
    void connect();
    void bind();
    void send(string message);
    string receive();
    void init();
};


#endif