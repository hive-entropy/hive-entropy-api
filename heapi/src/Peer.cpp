#include "Peer.h"

Peer::Peer(){}

Peer::Peer(Hardware h, std::string address, float latency) : hardware(h), address(address), latency(latency) {
    timestamp = std::time(nullptr);
}

void Peer::setAddress(std::string address){
    this->address = address;
}

void Peer::setLatency(float latency){
    this->latency = latency;
}

void Peer::setHardware(Hardware h){
    this->hardware = hardware;
}

Hardware Peer::getHardware(){
    return hardware;
}

std::string Peer::getAddress(){
    return address;
}

float Peer::getLatency(){
    return latency;
}

std::time_t Peer::getTimestamp(){
    return timestamp;
}

void Peer::refresh(){
    timestamp = std::time(nullptr);
}

bool Peer::operator==(const Peer& other){
    if(address==other.address)
        return true;
    return false;
}