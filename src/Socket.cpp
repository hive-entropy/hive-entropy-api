#include "../include/Socket.h"

#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <string>

Socket::Socket(string path) : path(path) {
   if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){

       throw "Couldn't create socket";
   }
}

Socket::~Socket(){
    close(fd);
}

void Socket::init(){
    sockAddr.sun_family = AF_UNIX;
    strncpy(sockAddr.sun_path, path.c_str(), sizeof(sockAddr.sun_path) - 1);
     if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) != 0){
		//Throw exception
	 }
}

void Socket::connect(){
	if (::connect(fd, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) == -1){
    	//Throw exception
  	}
}

void Socket::bind(){
	if (::bind(fd, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) == -1) {
    	//Throw exception
  	}
}

void Socket::send(string message){
    unsigned long length = message.length();
    unsigned long netLength = htonl(length);
    bool headerSent =  (::send(fd, reinterpret_cast<const char*>(& netLength), sizeof(netLength), 0) == sizeof(netLength));
    bool messageSent =  (::send(fd, reinterpret_cast<const char*>(message.c_str()), length, 0) == length);
}

string Socket::receive(){
    std::vector<unsigned char> *buffer = new std::vector<unsigned char>;
    unsigned long expectedSize;
	int pending = recv(fd, reinterpret_cast<char*>(&expectedSize), sizeof(expectedSize), 0);
	if ( pending <= 0 || pending != sizeof(unsigned short) ){
		//Throw exception
	}
	
	expectedSize = ntohs(expectedSize);
	buffer->resize(expectedSize);
	int receivedSize = 0;
	do {
		int ret = recv(fd, reinterpret_cast<char*>(&buffer[receivedSize]), (expectedSize - receivedSize) * sizeof(unsigned char), 0);
		if ( ret <= 0 ){
			//Throw exception
			buffer->clear();
		}
		else{
			receivedSize += ret;
		}
	} while ( receivedSize < expectedSize);
    return std::string(buffer->begin(),buffer->end());
}