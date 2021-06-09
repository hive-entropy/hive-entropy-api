#ifndef  PEER_H 
#define PEER_H

#include "Hardware.h"
#include <ctime>

class Peer{
    private:
        Hardware hardware;
        std::string address;
        float latency;
        std::time_t timestamp;

    public:
        Peer();
        Peer(Hardware h, std::string address, float latency);
        ~Peer() = default;

        void setHardware(Hardware h);
        void setAddress(std::string address);
        void setLatency(float latency);

        Hardware getHardware();
        std::string getAddress();
        float getLatency();
        std::time_t getTimestamp();

        void refresh();

        bool operator==(const Peer& other);
};

#endif