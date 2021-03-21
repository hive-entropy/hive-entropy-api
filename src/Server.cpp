#include "../include/Server.h"

Server::Server(std::string file) : sock(file){
}


Server::~Server(){
    delete &sock;
}


string Server::read(){
    return "";
}


void Server::write(string message){
}


int Server::listen(){
    return 0;
}


void Server::run(){
}
