#include "../include/Client.h"

string Client::read(){
    return sock.receive();
}


void Client::write(string message){
    return sock.send(message);
}


void Client::connect(){

}


Client::Client(std::string file) : sock(file){
    
}


void Client::run(){

}


Client::~Client(){
    delete &sock;
}