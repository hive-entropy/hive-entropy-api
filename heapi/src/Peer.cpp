#include "Peer.h"

Peer::Peer(){}

Peer::Peer(Hardware h, std::string address, float latency) : hardware(h), address(address), latency(latency) {}

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