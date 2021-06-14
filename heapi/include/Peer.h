#ifndef  PEER_H 
#define PEER_H

#include "Hardware.h"

#include <chrono>

class Peer{
    private:
        Hardware hardware;
        std::string address;
        std::chrono::steady_clock::duration latency;
        std::chrono::steady_clock::time_point timestamp;

    public:
        Peer();
        Peer(Hardware h, std::string address, std::chrono::steady_clock::duration latency);
        ~Peer() = default;

        void setHardware(Hardware h);
        void setAddress(std::string address);
        void setLatency(std::chrono::steady_clock::duration latency);

        Hardware getHardware();
        std::string getAddress();
        std::chrono::steady_clock::duration getLatency();
        std::chrono::steady_clock::time_point getTimestamp();

        void refresh();

        bool operator==(const Peer& other);
};

#endif