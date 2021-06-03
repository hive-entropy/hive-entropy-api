#ifndef  PEER_H 
#define PEER_H

#include "Hardware.h"

class Peer{
    private:
        Hardware hardware;
        std::string address;
        float latency;

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
};

#endif