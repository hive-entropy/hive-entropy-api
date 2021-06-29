#include "Peer.h"
#include "Hardware.h"

#include <chrono>
#include <memory>

Peer::Peer(Hardware const &_hardware, std::string const &address, std::chrono::steady_clock::duration const &latency) : address(address), latency(latency) {
    hardware = std::make_shared<Hardware>(_hardware);
    timestamp = std::chrono::steady_clock::now();
}

void Peer::setAddress(std::string const &_address){
    address = _address;
}

void Peer::setLatency(std::chrono::steady_clock::duration  const &_latency){
    latency = _latency;
}

void Peer::setHardware(Hardware const &_hardware){
    hardware = std::make_shared<Hardware>(_hardware);
}

Hardware Peer::getHardware() const {
    return *hardware;
}

std::string Peer::getAddress() const {
    return address;
}

std::chrono::steady_clock::duration Peer::getLatency() const {
    return latency;
}

std::chrono::steady_clock::time_point Peer::getTimestamp() const {
    return timestamp;
}

void Peer::refresh(){
    timestamp = std::chrono::steady_clock::now();
}

bool Peer::operator==(const Peer& other) const{
    return address == other.address;
}
