#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H 

#include <set>
#include <mutex>
#include <string>
#include <memory>
#include <coap3/coap.h>
#include <bits/stdc++.h>
#include <spdlog/spdlog.h>
#include <condition_variable>

enum MultiplicationMethod{
    CANNON,
    ROW_COLUMN,
    MULTIPLE_ROW_COLUMN
};

enum Parameter{
    ASSISTANCE_TIMEOUT,
    ASSISTANCE_MAX_PARTICIPANTS,
    UID_SEED,
    MAX_THREADS,
    FRESHNESS,
    RESULT_TIMEOUT,
    HEALTH_TIMEOUT
};

class Peer;
class Block;
class Message;
class HiveEntropyNode;
template<typename T>
class Matrix;
template<typename T>
class Distributor{
    private:
        std::shared_ptr<HiveEntropyNode> node;

        static std::map<std::string,std::mutex> locks; //Cleanup
        static std::mutex addressLock;

        static std::map<std::string,std::condition_variable> cvs; //Cleanup
        static std::condition_variable addressCv;

        static std::map<std::string,std::vector<Block>> pendingBlocks; //Cleanup

        static std::map<Parameter,std::string> settings;
        static std::vector<Peer> peers;
        static std::map<std::string, Matrix<T>> storedPartialResults; //Cleanup

        static std::chrono::steady_clock::time_point lastHardwareCheck;
        static std::vector<Peer> healthyNodes;

        bool destructionAsked;
        std::mutex destructionLock;

        static void handleResultResponse(Message m);
        static void handleHealthResponse(Message m);
        static void handleHardwareResponse(Message m);
        static void handleAssistanceResponse(Message m);
        static coap_response_t handleResponse(coap_session_t *session, const coap_pdu_t *sent, const coap_pdu_t *received, coap_mid_t id);

        void splitMatrixMultiplicationTask(std::string uid, Matrix<T> a, Matrix<T> b, MultiplicationMethod m);
        void splitMatrixConvolutionTask(std::string uid, Matrix<T> a, Matrix<T> b);

        void observer(std::string uid, Matrix<T> a, Matrix<T> b, MultiplicationMethod m);

        std::string generateUID();

    public:
        // Constructors
        explicit Distributor(std::shared_ptr<HiveEntropyNode> const &n);
        ~Distributor();

        // Getters
        Matrix<T> get(std::string id);

        // Methods
        std::string distributeMatrixMultiplication(Matrix<T> a, Matrix<T> b, MultiplicationMethod m=ROW_COLUMN);
        std::string distributeMatrixConvolution(Matrix<T> a, Matrix<T> b);

        void waitOn(std::string id);
        void configure(Parameter p, std::string value);
        void configure(Parameter p, int value);
};

#endif