#include "Peer.h"

Peer::Peer(){}

Peer::Peer(Hardware h, std::string address, std::chrono::steady_clock::duration latency) : hardware(h), address(address), latency(latency) {
    timestamp = std::chrono::steady_clock::now();
}

void Peer::setAddress(std::string address){
    this->address = address;
}

void Peer::setLatency(std::chrono::steady_clock::duration latency){
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

std::chrono::steady_clock::duration Peer::getLatency(){
    return latency;
}

std::chrono::steady_clock::time_point Peer::getTimestamp(){
    return timestamp;
}

void Peer::refresh(){
    timestamp = std::chrono::steady_clock::now();
}

bool Peer::operator==(const Peer& other){
    if(address==other.address)
        return true;
    return false;
}