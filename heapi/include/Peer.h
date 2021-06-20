#ifndef  PEER_H 
#define PEER_H

#include <chrono>
#include <string>
#include <memory>

class Hardware;
class Peer{
    private:
        /**
         * Hardware object containing the characteristics of this peer. These are unique to each peer.
         */
        std::shared_ptr<Hardware> hardware;
        std::string address;
        std::chrono::steady_clock::duration latency{};
        std::chrono::steady_clock::time_point timestamp;

    public:
        // Constructors
        Peer() = default;
        Peer(Peer const &peer) = default;
        Peer(Hardware const &_hardware, std::string const &address, std::chrono::steady_clock::duration const &latency);
        ~Peer() = default;

        // Getters
        Hardware getHardware() const;
        std::string getAddress() const;
        std::chrono::steady_clock::duration getLatency() const;
        std::chrono::steady_clock::time_point getTimestamp() const;

        // Setters
        void setHardware(Hardware const &_hardware);
        void setAddress(std::string const &_address);
        void setLatency(std::chrono::steady_clock::duration const &_latency);

        // Operators
        bool operator==(Peer const &other) const;

        // Methods
        void refresh();
};



#endif