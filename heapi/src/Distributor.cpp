#include "Distributor.h"
#include "HiveEntropyNode.h"
#include "Hardware.h"
#include "Peer.h"
#include "Block.h"
#include "Matrix.h"

#include <chrono>
#include <utility>
using namespace std::chrono_literals;

/*/////////////////////////////////////
//  Template explicit instanciation  //
/////////////////////////////////////*/
template
class Distributor<int>;

template
class Distributor<float>;

template
class Distributor<double>;

template
class Distributor<unsigned short>;

template
class Distributor<unsigned char>;

/*/////////////////////
//  Implementations  //
/////////////////////*/

template<typename T>
std::map<std::string, std::mutex> Distributor<T>::locks = std::map<std::string, std::mutex>();

template<typename T>
std::map<std::string, std::condition_variable> Distributor<T>::cvs = std::map<std::string, std::condition_variable>();

template<typename T>
std::map<std::string, std::vector<Block>> Distributor<T>::pendingBlocks = std::map<std::string, std::vector<Block>>();

template<typename T>
std::map<Parameter, std::string> Distributor<T>::settings = std::map<Parameter, std::string>();

template<typename T>
std::vector<Peer> Distributor<T>::peers = std::vector<Peer>();

template<typename T>
std::map<std::string, Matrix<T>> Distributor<T>::storedPartialResults = std::map<std::string, Matrix<T>>();

template<typename T>
std::mutex Distributor<T>::addressLock;

template<typename T>
std::condition_variable Distributor<T>::addressCv;

template<typename T>
std::chrono::steady_clock::time_point Distributor<T>::lastHardwareCheck = std::chrono::steady_clock::now();

template<typename T>
std::vector<Peer> Distributor<T>::healthyNodes = std::vector<Peer>();

template<typename T>
Distributor<T>::Distributor(std::shared_ptr<HiveEntropyNode> const &n) : node(n) {
    node->registerResponseHandler(handleResponse);
    configure(Parameter::ASSISTANCE_TIMEOUT, 5);
    configure(Parameter::ASSISTANCE_MAX_PARTICIPANTS, 20);
    configure(Parameter::RESULT_TIMEOUT, 10);
    configure(Parameter::HEALTH_TIMEOUT, 2);
    configure(Parameter::MAX_THREADS, 4);
    configure(Parameter::FRESHNESS, 10);
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S.%e] [%!] (%l) %v");

    /*Peer p;
    p.setAddress("192.168.1.35:9999");
    peers.push_back(p);*/
}

template<typename T>
Distributor<T>::~Distributor() {
    std::unique_lock<std::mutex> lock(destructionLock);
    destructionAsked = true;
}

template<typename T>
std::string Distributor<T>::distributeMatrixMultiplication(Matrix<T> a, Matrix<T> b, MultiplicationMethod m) {
    // Generate an unique id for the computation
    std::string uid = generateUID();
    spdlog::info("Obtained following UID={}", uid);

    // Create the matrix that will store the computation results
    Matrix<T> result(a.getRows(), b.getColumns());
    // Store the matrix in a map with its uid, to be fetched later
    storedPartialResults.insert(std::pair<std::string, Matrix<T>>(uid, result));
    spdlog::info("Created result for UID={}", uid);

    std::thread splitter(&Distributor<T>::splitMatrixMultiplicationTask, this, uid, a, b, m);
    splitter.detach();
    spdlog::info("Created splitter thread for UID={}", uid);

    node->resolveNodeIdentities();
    lastHardwareCheck = std::chrono::steady_clock::now();
    spdlog::info("Queried node assistance");

    return uid;
}

template<typename T>
std::string Distributor<T>::distributeMatrixConvolution(Matrix<T> a, Matrix<T> b) {
    std::string uid = generateUID();

    spdlog::info("Obtained following UID={}", uid);

    Matrix<T> result(a.getRows() - (b.getRows() / 2) * 2, a.getColumns() - (b.getColumns() / 2) * 2);
    storedPartialResults.insert(std::pair<std::string, Matrix<T>>(uid, result));
    spdlog::info("Created result for UID={}", uid);

    std::thread splitter(&Distributor<T>::splitMatrixConvolutionTask, this, uid, a, b);
    splitter.detach();
    spdlog::info("Created splitter thread for UID={}", uid);

    node->resolveNodeIdentities();
    lastHardwareCheck = std::chrono::steady_clock::now();
    spdlog::info("Queried node assistance");

    return uid;
}

template<typename T>
void Distributor<T>::splitMatrixMultiplicationTask(std::string uid, Matrix<T> a, Matrix<T> b, MultiplicationMethod m) {
    spdlog::info("Waiting on assistance response lock");
    std::unique_lock<std::mutex> lock(addressLock);
    /*bool timedOut = addressCv.wait_for(lock, std::stoi(settings[Parameter::ASSISTANCE_TIMEOUT]) * 1000ms, [] {
        return static_cast<int>(peers.size()) >= std::stoi(settings[Parameter::ASSISTANCE_MAX_PARTICIPANTS]);
    });*/


    std::unique_lock<std::mutex> uidLock(locks[uid]);
    if (m == MultiplicationMethod::ROW_COLUMN) {
        int nodeCount = peers.size();
        int remainder = a.getRows() * b.getColumns() % nodeCount;
        int packetsPerNode = (a.getRows() * b.getColumns() - remainder) / nodeCount;
        spdlog::info("Sending {} packets to {} nodes and calculating {} packets locally", packetsPerNode, nodeCount,
                     remainder);

        int counter = 0;
        for (int i = 0; i < a.getRows(); i++) {
            for (int j = 0; j < b.getColumns(); j++) {
                Row<T> first(a.getColumns(), i, a.getRow(i));
                Column<T> second(b.getRows(), j, b.getColumn(j));
                if (counter > packetsPerNode * nodeCount) {
                    storedPartialResults.at(uid).put(i, j, first * second);
                } else {
                    Block block(peers[counter % nodeCount], i, j, i, j);
                    pendingBlocks[uid].push_back(block);
                    node->sendMatrixMultiplicationTask(peers[counter % nodeCount].getAddress(), first, second, uid);
                    spdlog::info("Sent packet ({},{}) to node {}", i, j, peers[counter % nodeCount].getAddress());
                }
                counter++;
            }
        }
    } else if (m == MultiplicationMethod::CANNON) {
        //----> Count assisters
        int nodeCount = peers.size();

        //----> Find closest perfect square that divides all dimensions
        int gridSize = 1;
        for (int i = 1; i <= std::min({a.getRows(), b.getColumns(), a.getColumns()}); ++i) {
            if (a.getRows() % i == 0 && b.getRows() % i == 0 && a.getColumns() % i == 0) {
                gridSize = i;
                if (sqrt(nodeCount) <= i)
                    break;
            }
        }
        spdlog::info("Splitting matrix in a {0} by {0} checkerboard", gridSize);

        //Iterate through steps
        int counter = 0;
        int taskId = 0;
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                taskId++;

                for (int k = 0; k < gridSize; k++) {
                    node->sendMatrixMultiplicationTask(peers[counter % nodeCount].getAddress(),
                                                       a.getSubmatrix(i * a.getRows() / gridSize,
                                                                      k * a.getColumns() / gridSize,
                                                                      (i + 1) * a.getRows() / gridSize - 1,
                                                                      (k + 1) * a.getColumns() / gridSize - 1),
                                                       b.getSubmatrix(k * b.getRows() / gridSize,
                                                                      j * b.getColumns() / gridSize,
                                                                      (k + 1) * b.getRows() / gridSize - 1,
                                                                      (j + 1) * b.getColumns() / gridSize - 1),
                                                       i * a.getRows() / gridSize, j * b.getColumns() / gridSize,
                                                       gridSize, std::to_string(taskId), uid);
                    spdlog::info("Sent packet for result ({},{}) to node {}", i, j,
                                 peers[counter % nodeCount].getAddress());
                }


                Block block(peers[counter % nodeCount], i * a.getRows() / gridSize,
                            (i + 1) * a.getRows() / gridSize - 1, j * b.getColumns() / gridSize,
                            (j + 1) * b.getColumns() / gridSize - 1);
                pendingBlocks[uid].push_back(block);
                counter++;
            }
        }
    } else if (m == MultiplicationMethod::MULTIPLE_ROW_COLUMN) {
        //----> Count assisters
        int nodeCount = peers.size();

        //----> Find closest perfect square that divides all dimensions
        int gridSize = 1;
        for (int i = 1; i <= std::min({a.getRows(), b.getColumns(), a.getColumns()}); ++i) {
            if (a.getRows() % i == 0 && b.getRows() % i == 0 && a.getColumns() % i == 0) {
                gridSize = i;
                if (sqrt(nodeCount) <= i)
                    break;
            }
        }
        spdlog::info("Splitting matrix in a {0} by {0} checkerboard", gridSize);


        //Iterate through checkerboard
        int counter = 0;
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                node->sendMatrixMultiplicationTask(peers[counter % nodeCount].getAddress(),
                                                   a.getSubmatrix(i * a.getRows() / gridSize, 0,
                                                                  (i + 1) * a.getRows() / gridSize - 1,
                                                                  a.getColumns() - 1),
                                                   b.getSubmatrix(0, j * b.getColumns() / gridSize, b.getRows() - 1,
                                                                  (j + 1) * b.getColumns() / gridSize - 1),
                                                   i * a.getRows() / gridSize, j * b.getColumns() / gridSize, uid);
                Block block(peers[counter % nodeCount], i * a.getRows() / gridSize,
                            (i + 1) * a.getRows() / gridSize - 1, j * b.getColumns() / gridSize,
                            (j + 1) * b.getColumns() / gridSize - 1);
                pendingBlocks[uid].push_back(block);
                spdlog::info("Sent packet ({},{}) to node {}", i, j, peers[counter % nodeCount].getAddress());
                counter++;
            }
        }
    }

    std::thread obs(&Distributor<T>::observer, this, uid, a, b, m);
    obs.detach();
}

template<typename T>
void Distributor<T>::splitMatrixConvolutionTask(std::string uid, Matrix<T> a, Matrix<T> b) {
    spdlog::info("Waiting on assistance response lock");
    std::unique_lock<std::mutex> lock(addressLock);
    /*bool timedOut = addressCv.wait_for(lock, std::stoi(settings[Parameter::ASSISTANCE_TIMEOUT]) * 1000ms, [] {
        return static_cast<int>(peers.size()) >= std::stoi(settings[Parameter::ASSISTANCE_MAX_PARTICIPANTS]);
    });*/

    std::unique_lock<std::mutex> uidLock(locks[uid]);

    int nodeCount = peers.size();

    bool splitAlongRow = (a.getRows() > a.getColumns());
    int majorLength = (splitAlongRow) ? a.getRows() : a.getColumns();
    int remainder = majorLength % nodeCount; // FIXME: Throws an error if no node is found
    int packetSize = (majorLength - remainder) / nodeCount; // FIXME: Same problem as above

    int borderSizeV = b.getRows() / 2;
    int borderSizeH = b.getColumns() / 2;

    spdlog::info("Sending packets to {} nodes and calculating {} packets locally", nodeCount, remainder);
    if (splitAlongRow) {
        if (remainder > 0) {
            Matrix<T> localResult = a.getSubmatrix(0, 0, remainder + borderSizeV - 1, a.getColumns() - 1).convolve(b,
                                                                                                                   EdgeHandling::Crop,
                                                                                                                   1);
            storedPartialResults.at(uid).putSubmatrix(0, 0, localResult);
        }

        int counter = 0;
        for (int i = remainder + borderSizeV; i < a.getRows() - borderSizeV; i += packetSize) {
            node->sendMatrixConvolutionTask(peers[counter % nodeCount].getAddress(), a.getSubmatrix(i - borderSizeV, 0,
                                                                                                    std::min({i +
                                                                                                              packetSize -
                                                                                                              1 +
                                                                                                              borderSizeV,
                                                                                                              a.getRows() -
                                                                                                              1}),
                                                                                                    a.getColumns() - 1),
                                            b, uid, i - borderSizeV, 0);
            Block block(peers[counter % nodeCount], i - borderSizeV, i + packetSize - 1, 0, a.getColumns() - 1);
            pendingBlocks[uid].push_back(block);
            spdlog::info("Sent packet ({},{}) to node {}", i, 0, peers[counter % nodeCount].getAddress());
            counter++;
        }
    } else {
        if (remainder > 0) {
            Matrix<T> localResult = a.getSubmatrix(0, 0, a.getRows() - 1, remainder + borderSizeH - 1).convolve(b,
                                                                                                                EdgeHandling::Crop,
                                                                                                                1);
            storedPartialResults.at(uid).putSubmatrix(0, 0, localResult);
        }

        int counter = 0;
        for (int i = remainder + borderSizeH; i < a.getRows() - borderSizeH; i += packetSize) {
            node->sendMatrixConvolutionTask(peers[counter % nodeCount].getAddress(),
                                            a.getSubmatrix(0, std::max({0, i - borderSizeH}), a.getRows() - 1, std::min(
                                                    {i + packetSize - 1 + borderSizeH, a.getColumns() - 1})), b, uid, 0,
                                            i - borderSizeH);
            Block block(peers[counter % nodeCount], 0, a.getRows() - 1, i - borderSizeH, i + packetSize - 1);
            pendingBlocks[uid].push_back(block);
            spdlog::info("Sent packet ({},{}) to node {}", 0, i, peers[counter % nodeCount].getAddress());
            counter++;
        }
    }
}

template<typename T>
void Distributor<T>::observer(std::string uid, Matrix<T> a, Matrix<T> b, MultiplicationMethod m) {
    while (!destructionAsked) {
        std::this_thread::sleep_for(std::chrono::seconds(std::stoi(settings[Parameter::RESULT_TIMEOUT])));
        std::unique_lock<std::mutex> dLock(destructionLock);

        std::unique_lock<std::mutex> lock(locks[uid]);
        if (pendingBlocks[uid].empty())
            break;

        for (Block block : pendingBlocks[uid]) {
            if (block.getTimestamp() + std::chrono::seconds(std::stoi(settings[Parameter::RESULT_TIMEOUT])) >
                std::chrono::steady_clock::now()) {
                auto responsible = block.getResponsible();
                if (responsible != nullptr) {
                    std::unique_lock<std::mutex> addrLock(addressLock);
                    bool alive = cvs[uid].wait_for(lock,
                                                   std::chrono::seconds(std::stoi(settings[Parameter::HEALTH_TIMEOUT])),
                                                   [this, responsible] {
                                                       return std::find(healthyNodes.begin(), healthyNodes.end(),
                                                                        *responsible) != healthyNodes.end();
                                                   });
                    if (alive) {
                        std::find(peers.begin(), peers.end(), *responsible)->refresh();
                        healthyNodes.erase(std::find(healthyNodes.begin(), healthyNodes.end(), *responsible));
                        switch (m) {
                            case CANNON: {
                                for (int k = 0; k < a.getRows() / (block.getEndRow() - block.getStartRow()); k++) {
                                    node->sendMatrixMultiplicationTask(
                                            responsible->getAddress(),
                                            a.getSubmatrix(
                                                    block.getStartRow(),
                                                    k * a.getColumns() / a.getRows() /
                                                    (block.getEndRow() - block.getStartRow()),
                                                    block.getEndRow(),
                                                    (k + 1) * a.getColumns() / a.getRows() /
                                                    (block.getEndRow() - block.getStartRow()) - 1
                                            ),
                                            b.getSubmatrix(
                                                    k * b.getRows() / a.getRows() /
                                                    (block.getEndRow() - block.getStartRow()),
                                                    block.getStartCol(),
                                                    (k + 1) * b.getRows() / a.getRows() /
                                                    (block.getEndRow() - block.getStartRow()) - 1,
                                                    block.getEndCol()
                                            ),
                                            block.getStartRow(),
                                            block.getStartCol(),
                                            a.getRows() / (block.getEndRow() - block.getStartRow()),
                                            block.getTaskId(),
                                            uid
                                    );
                                }
                                break;
                            }
                            case ROW_COLUMN: {
                                Row<T> r(a.getColumns(), block.getStartRow(), a.getRow(block.getStartRow()));
                                Column<T> c(b.getRows(), block.getStartCol(), b.getColumn(block.getEndCol()));
                                node->sendMatrixMultiplicationTask(responsible->getAddress(), r, c, uid);
                                break;
                            }
                            case MULTIPLE_ROW_COLUMN: {
                                node->sendMatrixMultiplicationTask(
                                        responsible->getAddress(),
                                        a.getSubmatrix(
                                                block.getStartRow(),
                                                0,
                                                block.getEndRow(),
                                                a.getColumns() - 1
                                        ),
                                        b.getSubmatrix(
                                                0,
                                                block.getStartCol(),
                                                b.getRows() - 1,
                                                block.getEndCol()
                                        ),
                                        block.getStartRow(),
                                        block.getStartCol(),
                                        uid
                                );
                                break;
                            }
                        }
                        block.refresh();
                    } else {
                        switch (m) {
                            case CANNON: {
                                Peer p = peers.at((int) random() % peers.size());
                                for (int k = 0; k < a.getRows() / (block.getEndRow() - block.getStartRow()); k++) {
                                    node->sendMatrixMultiplicationTask(
                                            p.getAddress(),
                                            a.getSubmatrix(
                                                    block.getStartRow(),
                                                    k * a.getColumns() / a.getRows() /
                                                    (block.getEndRow() - block.getStartRow()),
                                                    block.getEndRow(),
                                                    (k + 1) * a.getColumns() / a.getRows() /
                                                    (block.getEndRow() - block.getStartRow()) - 1
                                            ),
                                            b.getSubmatrix(
                                                    k * b.getRows() / a.getRows() /
                                                    (block.getEndRow() - block.getStartRow()),
                                                    block.getStartCol(),
                                                    (k + 1) * b.getRows() / a.getRows() /
                                                    (block.getEndRow() - block.getStartRow()) - 1,
                                                    block.getEndCol()
                                            ),
                                            block.getStartRow(),
                                            block.getStartCol(),
                                            a.getRows() / (block.getEndRow() - block.getStartRow()),
                                            block.getTaskId(),
                                            uid
                                    );
                                }
                                break;
                            }
                            case ROW_COLUMN: {
                                Row<T> r(a.getColumns(), block.getStartRow(), a.getRow(block.getStartRow()));
                                Column<T> c(b.getRows(), block.getStartCol(), b.getColumn(block.getEndCol()));
                                node->sendMatrixMultiplicationTask(peers[(int) random() % peers.size()].getAddress(), r,
                                                                   c, uid);
                                break;
                            }
                            case MULTIPLE_ROW_COLUMN: {
                                node->sendMatrixMultiplicationTask(
                                        peers[(int) random() % peers.size()].getAddress(),
                                        a.getSubmatrix(
                                                block.getStartRow(),
                                                0,
                                                block.getEndRow(),
                                                a.getColumns() - 1
                                        ),
                                        b.getSubmatrix(
                                                0,
                                                block.getStartCol(),
                                                b.getRows() - 1,
                                                block.getEndCol()
                                        ),
                                        block.getStartRow(),
                                        block.getStartCol(),
                                        uid
                                );
                                break;
                            }
                        }
                        block.refresh();
                    }
                } else {
                    switch (m) {
                        case CANNON: {
                            Peer p = peers.at((int) random() % peers.size());
                            for (int k = 0; k < a.getRows() / (block.getEndRow() - block.getStartRow()); k++) {
                                node->sendMatrixMultiplicationTask(
                                        p.getAddress(),
                                        a.getSubmatrix(
                                                block.getStartRow(),
                                                k * a.getColumns() / a.getRows() /
                                                (block.getEndRow() - block.getStartRow()),
                                                block.getEndRow(),
                                                (k + 1) * a.getColumns() / a.getRows() /
                                                (block.getEndRow() - block.getStartRow()) - 1
                                        ),
                                        b.getSubmatrix(
                                                k * b.getRows() / a.getRows() /
                                                (block.getEndRow() - block.getStartRow()),
                                                block.getStartCol(),
                                                (k + 1) * b.getRows() / a.getRows() /
                                                (block.getEndRow() - block.getStartRow()) - 1,
                                                block.getEndCol()
                                        ),
                                        block.getStartRow(),
                                        block.getStartCol(),
                                        a.getRows() / (block.getEndRow() - block.getStartRow()),
                                        block.getTaskId(),
                                        uid
                                );
                            }
                            break;
                        }
                        case ROW_COLUMN: {
                            Row<T> r(a.getColumns(), block.getStartRow(), a.getRow(block.getStartRow()));
                            Column<T> c(b.getRows(), block.getStartCol(), b.getColumn(block.getEndCol()));
                            node->sendMatrixMultiplicationTask(peers[(int) random() % peers.size()].getAddress(), r, c,
                                                               uid);
                            break;
                        }
                        case MULTIPLE_ROW_COLUMN: {
                            node->sendMatrixMultiplicationTask(
                                    peers[(int) random() % peers.size()].getAddress(),
                                    a.getSubmatrix(
                                            block.getStartRow(),
                                            0,
                                            block.getEndRow(),
                                            a.getColumns() - 1
                                    ),
                                    b.getSubmatrix(
                                            0,
                                            block.getStartCol(),
                                            b.getRows() - 1,
                                            block.getEndCol()
                                    ),
                                    block.getStartRow(),
                                    block.getStartCol(),
                                    uid
                            );
                            break;
                        }
                    }
                    block.refresh();
                }
            }
        }
    }
}

template<typename T>
Matrix<T> Distributor<T>::get(std::string id) {
    spdlog::info("Waiting on release lock for UID={}", id);
    std::unique_lock<std::mutex> lock(locks[id]);
    cvs[id].wait(lock, [this, id] {
        if (pendingBlocks.find(id) != pendingBlocks.end())
            return pendingBlocks[id].empty();
        else return false;
    });

    spdlog::info("Getting result");
    Matrix<T> result = storedPartialResults.at(id);

    storedPartialResults.erase(id);
    cvs.erase(id);
    locks.erase(id);
    pendingBlocks.erase(id);

    return result;
}

template<typename T>
void Distributor<T>::waitOn(std::string id) {
    spdlog::info("Waiting on release lock for UID={}", id);
    std::unique_lock<std::mutex> lock(locks[id]);
    cvs[id].wait(lock, [this, id] {
        if (pendingBlocks.find(id) != pendingBlocks.end())
            return pendingBlocks[id].empty();
        else return false;
    });
}

template<typename T>
void Distributor<T>::configure(Parameter p, std::string value) {
    settings[p] = value;
}

template<typename T>
void Distributor<T>::configure(Parameter p, int value) {
    settings[p] = std::to_string(value);
}

template<typename T>
coap_response_t
Distributor<T>::handleResponse(coap_session_t *session, const coap_pdu_t *sent, const coap_pdu_t *received,
                               coap_mid_t id) {
    Message m(session, received);
    if (m.getHeaders().find(Headers::PURPOSE) != m.getHeaders().end()) {
        std::string purpose = m.getHeaders()[Headers::PURPOSE];
        //spdlog::info("Received message with {} purpose",purpose);
        if (purpose == PURPOSE_ASSISTANCE) {
            handleAssistanceResponse(m);
        } else if (purpose == PURPOSE_HARDWARE) {
            handleHardwareResponse(m);
        } else if (purpose == PURPOSE_RESULT) {
            handleResultResponse(m);
        } else if (purpose == PURPOSE_HEALTH) {
            handleHealthResponse(m);
        } else {
            //spdlog::warn("Unrecognized purpose {}. Skipping handling...",purpose);
        }
    }

    return COAP_RESPONSE_OK;
}


template<typename T>
void Distributor<T>::handleResultResponse(Message m) {
    std::string uid = m.getHeaders()[Headers::CALCULATION_ID];
    std::string object = m.getHeaders()[Headers::SERIALIZED_TYPE];

    spdlog::info("Waiting ownership of lock for UID={}", uid);
    std::unique_lock<std::mutex> lock(locks[uid]);

    if (object == SERIALIZED_TYPE_MATRIX) {
        Matrix<T> result = Serializer::unserializeMatrix<T>(m.getContent());

        spdlog::info("Extracted submatrix:");
        result.show();
        spdlog::info("Inserting...");
        if (storedPartialResults.find(uid) != storedPartialResults.end()) {
            storedPartialResults.at(uid).putSubmatrix(std::stoi(m.getHeaders()[Headers::INSERT_AT_X]),
                                                      std::stoi(m.getHeaders()[Headers::INSERT_AT_Y]), result);

            Block block(std::stoi(m.getHeaders()[Headers::INSERT_AT_X]),
                        std::stoi(m.getHeaders()[Headers::INSERT_AT_Y]));
            std::vector<Block>::iterator it = std::find(pendingBlocks[uid].begin(), pendingBlocks[uid].end(), block);
            if (it != pendingBlocks[uid].end()) {
                spdlog::info("Removing result from pending");
                pendingBlocks.at(uid).erase(it);
            }
        }
    } else if (object == SERIALIZED_TYPE_ELEMENT) {
        T element;

        if (typeid(T) == typeid(int)) {
            element = std::stoi(m.getContent());
        } else if (typeid(T) == typeid(float)) {
            element = std::stof(m.getContent());
        } else if (typeid(T) == typeid(double)) {
            element = std::stod(m.getContent());
        } else {
            return;
        }

        spdlog::info("Extracted element {}. Inserting...", element);
        if (storedPartialResults.find(uid) != storedPartialResults.end()) {
            storedPartialResults.at(uid).put(std::stoi(m.getHeaders()[Headers::INSERT_AT_X]),
                                             std::stoi(m.getHeaders()[Headers::INSERT_AT_Y]), element);
            Block block(std::stoi(m.getHeaders()[Headers::INSERT_AT_X]),
                        std::stoi(m.getHeaders()[Headers::INSERT_AT_Y]));

            std::vector<Block>::iterator it = std::find(pendingBlocks[uid].begin(), pendingBlocks[uid].end(), block);
            if (it != pendingBlocks[uid].end()) {
                spdlog::info("Removing result from pending");
                pendingBlocks.at(uid).erase(it);
                spdlog::info("{} blocks remaining to receive...", pendingBlocks.at(uid).size());
            }
        }
    }

    spdlog::info("Releasing ownership of lock for UID={}", uid);
    lock.unlock();
    spdlog::info("Notifying waiting threads");
    cvs[uid].notify_all();
}

template<typename T>
void Distributor<T>::handleAssistanceResponse(Message m) {
    std::string address = m.getPeer();

    spdlog::info("Waiting for ownerhip of peer lock");
    std::unique_lock<std::mutex> lock(addressLock);
    Peer p;
    p.setAddress(address);
    if (!std::count(peers.begin(), peers.end(), p)) {
        peers.push_back(p);
    }

    spdlog::info("Releasing ownership of peer lock");
    lock.unlock();
    spdlog::info("Notifying waiting threads");
    addressCv.notify_all();
}

template<typename T>
void Distributor<T>::handleHardwareResponse(Message m) {
    std::string address = m.getPeer();
    Hardware hw(m.getContent());
    spdlog::info("Received hardware specification {}", hw.toString());
    std::chrono::steady_clock::duration delay = std::chrono::steady_clock::now() - lastHardwareCheck;

    Peer peer(hw, address, delay);
    peers.push_back(peer);
}

template<typename T>
void Distributor<T>::handleHealthResponse(Message m) {
    std::unique_lock<std::mutex> lock(addressLock);
    Peer p;
    p.setAddress(m.getPeer());

    if (std::find(peers.begin(), peers.end(), p) != peers.end())
        healthyNodes.push_back(*std::find(peers.begin(), peers.end(), p));
    lock.unlock();
    addressCv.notify_all(); // FIXME: The uid used to notify the end of the second process isn't correct
}

template<typename T>
std::string Distributor<T>::generateUID() {
    if (settings.find(Parameter::UID_SEED) != settings.end()) {
        uint64_t seed[] = {1};
        char *seedTable = reinterpret_cast<char *>(seed);
        settings[Parameter::UID_SEED] = std::string(seedTable, 8);
    }

    char currUIDStr[settings[Parameter::UID_SEED].size()];
    strncpy(currUIDStr, settings[Parameter::UID_SEED].c_str(), settings[Parameter::UID_SEED].size());

    uint64_t *currUID = reinterpret_cast<uint64_t *>(currUIDStr);
    (*currUID) += 1;
    settings[Parameter::UID_SEED] = std::string(reinterpret_cast<char *>(currUID), 8);
    return settings[Parameter::UID_SEED];
}