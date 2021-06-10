#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H 

#include "HiveEntropyNode.h"
#include "Hardware.h"
#include "Peer.h"
#include "Block.h"

#include <mutex>
#include <condition_variable>
#include <set>

#include <bits/stdc++.h>

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

template<typename T>
class Distributor{
    public:
        Distributor(HiveEntropyNode* n);
        ~Distributor();

        std::string distributeMatrixMultiplication(Matrix<T> a, Matrix<T> b, MultiplicationMethod m=ROW_COLUMN);
        std::string distributeMatrixConvolution(Matrix<T> a, Matrix<T> b);

        Matrix<T> get(std::string id);
        void waitOn(std::string id);

        void configure(Parameter p, std::string value);
        void configure(Parameter p, int value);
    private:
        HiveEntropyNode* node;
        
        static std::map<std::string,std::mutex> locks; //Cleanup
        static std::mutex addressLock;

        static std::map<std::string,std::condition_variable> cvs; //Cleanup
        static std::condition_variable addressCv;

        static std::map<std::string,std::vector<Block>> pendingBlocks; //Cleanup

        static std::map<Parameter,std::string> settings;
        static std::vector<Peer> peers;
        static std::map<std::string, Matrix<T>> storedPartialResults; //Cleanup

        static std::time_t lastHardwareCheck;
        static std::vector<Peer> healthyNodes;

        static coap_response_t handleResponse(coap_session_t *session, const coap_pdu_t *sent, const coap_pdu_t *received, coap_mid_t id);
        static void handleResultResponse(Message m);
        static void handleAssistanceResponse(Message m);
        static void handleHardwareResponse(Message m);
        static void handleHealthResponse(Message m);

        void splitMatrixMultiplicationTask(std::string uid, Matrix<T> a, Matrix<T> b, MultiplicationMethod m);

        void observer(std::string uid, Matrix<T> a, Matrix<T> b, MultiplicationMethod m);

        std::string generateUID();
};

template<typename T>
std::map<std::string,std::mutex> Distributor<T>::locks = std::map<std::string,std::mutex>();

template<typename T>
std::map<std::string,std::condition_variable> Distributor<T>::cvs = std::map<std::string,std::condition_variable>();

template<typename T>
std::map<std::string,std::vector<Block>> Distributor<T>::pendingBlocks = std::map<std::string,std::vector<Block>>();

template<typename T>
std::map<Parameter,std::string> Distributor<T>::settings = std::map<Parameter,std::string>();

template<typename T>
std::vector<Peer> Distributor<T>::peers = std::vector<Peer>();

template<typename T>
std::map<std::string, Matrix<T>> Distributor<T>::storedPartialResults = std::map<std::string, Matrix<T>>();

template<typename T>
std::mutex Distributor<T>::addressLock;

template<typename T>
std::condition_variable Distributor<T>::addressCv;

template<typename T>
std::time_t Distributor<T>::lastHardwareCheck;

template<typename T>
std::vector<Peer> Distributor<T>::healthyNodes = std::vector<Peer>();

template<typename T>
Distributor<T>::Distributor(HiveEntropyNode* n) : node(n){
    node->registerResponseHandler(handleResponse);
    configure(Parameter::ASSISTANCE_TIMEOUT,5);
    configure(Parameter::ASSISTANCE_MAX_PARTICIPANTS,12);
    configure(Parameter::RESULT_TIMEOUT,10);
    configure(Parameter::HEALTH_TIMEOUT,2);
    configure(Parameter::MAX_THREADS,4);
    configure(Parameter::FRESHNESS,10);
}

template<typename T>
Distributor<T>::~Distributor(){
    //Something something free memory
}

template<typename T>
std::string Distributor<T>::distributeMatrixMultiplication(Matrix<T> a, Matrix<T> b, MultiplicationMethod m){
    std::string uid = generateUID();

    cout << "[distribute] Obtained uid="+uid << endl;

    Matrix<T> result(a.getRows(),b.getColumns());
    storedPartialResults.insert(std::pair<std::string,Matrix<T>>(uid,result));
    cout << "[distribute] Created result to use for calculation" << endl;

    std::thread splitter(&Distributor<T>::splitMatrixMultiplicationTask,this,uid,a,b,m);
    splitter.detach();
    cout << "[distribute] Initiated splitter thread" << endl;

    node->resolveNodeIdentities();
    lastHardwareCheck = std::time(nullptr);

    cout << "[distribute] Queried node availability" << endl;

    return uid;
}

template<typename T> 
void Distributor<T>::splitMatrixMultiplicationTask(std::string uid, Matrix<T> a, Matrix<T> b, MultiplicationMethod m){
    
    cout << "[splitter] Waiting on lock for answers" << endl;
    std::unique_lock<std::mutex> lock(addressLock);
    bool timedOut = addressCv.wait_for(lock,std::stoi(settings[Parameter::ASSISTANCE_TIMEOUT])*1000ms,[]{return peers.size()>=std::stoi(settings[Parameter::ASSISTANCE_MAX_PARTICIPANTS]);});

    if(m==MultiplicationMethod::ROW_COLUMN){
        int nodeCount = peers.size();
        int remainder = a.getRows()*b.getColumns() % nodeCount;
        int packetsPerNode = (a.getRows()*b.getColumns() - remainder) / nodeCount;

        cout << "[splitter] Sending "+to_string(packetsPerNode)+" packets to "+to_string(nodeCount)+" nodes, and calculating "+to_string(remainder)+" packets locally" << endl;

        int counter = 0;
        for(int i=0;i<a.getRows();i++){
            for(int j=0;j<b.getColumns();j++){
                Row<T> first(a.getColumns(),i,a.getRow(i));
                Column<T> second(b.getRows(),j,b.getColumn(j));
                if(counter>packetsPerNode*nodeCount){
                    storedPartialResults.at(uid).put(i,j,first*second);
                }
                else{
                    Block block(&peers[counter%nodeCount],i,j,i,j);
                    pendingBlocks[uid].push_back(block);
                    node->sendMatrixMultiplicationTask(peers[counter%nodeCount].getAddress(),first,second,uid);
                    //cout << "[splitter] Sent packet ("+to_string(i)+", "+to_string(j)+") to node "+peers[counter%nodeCount] << endl;
                }
                counter++;
            }
        }
    }
    else if(m==MultiplicationMethod::CANNON){
        //----> Count assisters
        int nodeCount = peers.size();

        //----> Find closest perfect square that divides all dimensions
        int gridSize = 1;
        for(int i=1; i <= std::min({a.getRows(), b.getColumns(), a.getColumns()}); ++i) {
            if(a.getRows()%i==0 && b.getRows()%i==0 && a.getColumns()%i==0){
                gridSize = i;
                if(sqrt(nodeCount) <= i)
                    break;
            }
        }
        
        //Iterate through steps
        int counter = 0;
        int taskId = 0;
        for(int i=0; i<gridSize; i++){
            for(int j=0;j<gridSize;j++){
                taskId++;
                
                for(int k=0; k<gridSize;k++){
                    node->sendMatrixMultiplicationTask(peers[counter%nodeCount].getAddress(),a.getSubmatrix(i*a.getRows()/gridSize,k*a.getColumns()/gridSize,(i+1)*a.getRows()/gridSize-1,(k+1)*a.getColumns()/gridSize-1),b.getSubmatrix(k*b.getRows()/gridSize,j*b.getColumns()/gridSize,(k+1)*b.getRows()/gridSize-1,(j+1)*b.getColumns()/gridSize-1),i*a.getRows()/gridSize,j*b.getColumns()/gridSize,gridSize,to_string(taskId),uid);
                }


                Block block(&peers[counter%nodeCount],i*a.getRows()/gridSize,(i+1)*a.getRows()/gridSize-1,j*b.getColumns()/gridSize, (j+1)*b.getColumns()/gridSize-1);
                pendingBlocks[uid].push_back(block);
                counter++;
            }
        }
    }
    else if(m==MultiplicationMethod::MULTIPLE_ROW_COLUMN){
        //----> Count assisters
        int nodeCount = peers.size();

        //----> Find closest perfect square that divides all dimensions
        int gridSize = 1;
        for(int i=1; i <= std::min({a.getRows(), b.getColumns(), a.getColumns()}); ++i) {
            if(a.getRows()%i==0 && b.getRows()%i==0 && a.getColumns()%i==0){
                gridSize = i;
                if(sqrt(nodeCount) <= i)
                    break;
            }
        }
        
        //Iterate through checkerboard
        int counter = 0;
        for(int i=0; i<gridSize; i++){
            for(int j=0;j<gridSize;j++){ 
                node->sendMatrixMultiplicationTask(peers[counter%nodeCount].getAddress(),a.getSubmatrix(i*a.getRows()/gridSize,0,(i+1)*a.getRows()/gridSize-1,a.getColumns()-1),b.getSubmatrix(0,j*b.getColumns()/gridSize,b.getRows()-1,(j+1)*b.getColumns()/gridSize-1),i*a.getRows()/gridSize,j*b.getColumns()/gridSize,uid);
                Block block(&peers[counter%nodeCount],i*a.getRows()/gridSize, (i+1)*a.getRows()/gridSize-1,  j*b.getColumns()/gridSize, (j+1)*b.getColumns()/gridSize-1);
                pendingBlocks[uid].push_back(block);
                counter++;
            }
        }
    }

    std::thread obs(&Distributor<T>::observer,this,uid,a,b,m);
    obs.detach();
}


template<typename T>
void Distributor<T>::observer(std::string uid, Matrix<T> a, Matrix<T> b, MultiplicationMethod m){
    while(true){
        std::unique_lock<std::mutex> lock(locks[uid]);

        if(pendingBlocks[uid].empty())
            break;

        for(Block block : pendingBlocks[uid]){
            if(block.getTimestamp()+Parameter::RESULT_TIMEOUT>std::time(nullptr)){
                Peer* responsible = block.getResponsible();
                if(responsible!=nullptr){
                    std::unique_lock<std::mutex> addrLock(addressLock);
                    bool alive = cvs[uid].wait_for(lock,std::chrono::seconds(std::stoi(settings[Parameter::HEALTH_TIMEOUT])),[this,responsible]{
                        return std::find(healthyNodes.begin(),healthyNodes.end(),*responsible)!=healthyNodes.end();
                    });
                    if(alive){
                        std::find(peers.begin(),peers.end(),*responsible)->refresh();
                        healthyNodes.erase(std::find(healthyNodes.begin(),healthyNodes.end(),*responsible));
                        switch(m){
                            case CANNON: {
                                for(int k=0; k<a.getRows()/(block.getEndRow()-block.getStartRow());k++){
                                    node->sendMatrixMultiplicationTask(
                                        responsible->getAddress(),
                                        a.getSubmatrix(
                                            block.getStartRow(),
                                            k*a.getColumns()/a.getRows()/(block.getEndRow()-block.getStartRow()),
                                            block.getEndRow(),
                                            (k+1)*a.getColumns()/a.getRows()/(block.getEndRow()-block.getStartRow())-1
                                        ),
                                        b.getSubmatrix(
                                            k*b.getRows()/a.getRows()/(block.getEndRow()-block.getStartRow()),
                                            block.getStartCol(),
                                            (k+1)*b.getRows()/a.getRows()/(block.getEndRow()-block.getStartRow())-1,
                                            block.getEndCol()
                                        ),
                                        block.getStartRow(),
                                        block.getStartCol(),
                                        a.getRows()/(block.getEndRow()-block.getStartRow()),
                                        block.getTaskId(),
                                        uid
                                    );
                                }
                            break;
                            }
                            case ROW_COLUMN:{
                                Row<T> r(a.getColumns(),block.getStartRow(),a.getRow(block.getStartRow()));
                                Column<T> c(b.getRows(),block.getStartCol(),b.getColumn(block.getEndCol()));
                                node->sendMatrixMultiplicationTask(responsible->getAddress(),r,c,uid);
                            break;
                            }
                            case MULTIPLE_ROW_COLUMN:{
                                node->sendMatrixMultiplicationTask(
                                    responsible->getAddress(),
                                    a.getSubmatrix(
                                        block.getStartRow(),
                                        0,
                                        block.getEndRow(),
                                        a.getColumns()
                                    ),
                                    b.getSubmatrix(
                                        0,
                                        block.getStartCol(),
                                        b.getRows(),
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
                    else{
                        switch(m){
                            case CANNON:{
                                Peer p = peers.at((int) random()%peers.size());
                                for(int k=0; k<a.getRows()/(block.getEndRow()-block.getStartRow());k++){
                                    node->sendMatrixMultiplicationTask(
                                        p.getAddress(),
                                        a.getSubmatrix(
                                            block.getStartRow(),
                                            k*a.getColumns()/a.getRows()/(block.getEndRow()-block.getStartRow()),
                                            block.getEndRow(),
                                            (k+1)*a.getColumns()/a.getRows()/(block.getEndRow()-block.getStartRow())-1
                                        ),
                                        b.getSubmatrix(
                                            k*b.getRows()/a.getRows()/(block.getEndRow()-block.getStartRow()),
                                            block.getStartCol(),
                                            (k+1)*b.getRows()/a.getRows()/(block.getEndRow()-block.getStartRow())-1,
                                            block.getEndCol()
                                        ),
                                        block.getStartRow(),
                                        block.getStartCol(),
                                        a.getRows()/(block.getEndRow()-block.getStartRow()),
                                        block.getTaskId(),
                                        uid
                                    );
                                }
                            break;
                            }
                            case ROW_COLUMN:{
                                Row<T> r(a.getColumns(),block.getStartRow(),a.getRow(block.getStartRow()));
                                Column<T> c(b.getRows(),block.getStartCol(),b.getColumn(block.getEndCol()));
                                node->sendMatrixMultiplicationTask(peers[(int) random()%peers.size()].getAddress(),r,c,uid);
                            break;
                            }
                            case MULTIPLE_ROW_COLUMN:
                            {
                                node->sendMatrixMultiplicationTask(
                                    peers[(int) random()%peers.size()].getAddress(),
                                    a.getSubmatrix(
                                        block.getStartRow(),
                                        0,
                                        block.getEndRow(),
                                        a.getColumns()
                                    ),
                                    b.getSubmatrix(
                                        0,
                                        block.getStartCol(),
                                        b.getRows(),
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
                else{
                    switch(m){
                            case CANNON:{
                                Peer p = peers.at((int) random()%peers.size());
                                for(int k=0; k<a.getRows()/(block.getEndRow()-block.getStartRow());k++){
                                    node->sendMatrixMultiplicationTask(
                                        p.getAddress(),
                                        a.getSubmatrix(
                                            block.getStartRow(),
                                            k*a.getColumns()/a.getRows()/(block.getEndRow()-block.getStartRow()),
                                            block.getEndRow(),
                                            (k+1)*a.getColumns()/a.getRows()/(block.getEndRow()-block.getStartRow())-1
                                        ),
                                        b.getSubmatrix(
                                            k*b.getRows()/a.getRows()/(block.getEndRow()-block.getStartRow()),
                                            block.getStartCol(),
                                            (k+1)*b.getRows()/a.getRows()/(block.getEndRow()-block.getStartRow())-1,
                                            block.getEndCol()
                                        ),
                                        block.getStartRow(),
                                        block.getStartCol(),
                                        a.getRows()/(block.getEndRow()-block.getStartRow()),
                                        block.getTaskId(),
                                        uid
                                    );
                                }
                            break;
                            }
                            case ROW_COLUMN:{
                                Row<T> r(a.getColumns(),block.getStartRow(),a.getRow(block.getStartRow()));
                                Column<T> c(b.getRows(),block.getStartCol(),b.getColumn(block.getEndCol()));
                                node->sendMatrixMultiplicationTask(peers[(int) random()%peers.size()].getAddress(),r,c,uid);
                            break;
                            }
                            case MULTIPLE_ROW_COLUMN:{
                                node->sendMatrixMultiplicationTask(
                                    peers[(int) random()%peers.size()].getAddress(),
                                    a.getSubmatrix(
                                        block.getStartRow(),
                                        0,
                                        block.getEndRow(),
                                        a.getColumns()
                                    ),
                                    b.getSubmatrix(
                                        0,
                                        block.getStartCol(),
                                        b.getRows(),
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
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(std::stoi(settings[Parameter::RESULT_TIMEOUT])));
    }
}


template<typename T>
std::string Distributor<T>::distributeMatrixConvolution(Matrix<T> a, Matrix<T> b){
    //LATER BOIIS
}

template<typename T>
Matrix<T> Distributor<T>::get(std::string id){
    cout << "[get] Waiting on lock" << endl;
    std::unique_lock<std::mutex> lock(locks[id]);
    cvs[id].wait(lock,[this,id]{
        if(pendingBlocks.find(id)!=pendingBlocks.end())
            return pendingBlocks[id].empty();
        else return false;
    });

    cout << "[get] Obtaining result" << endl;
    Matrix<T> result = storedPartialResults.at(id);

    //Cleanup operations...

    return result;
}

template<typename T>
void Distributor<T>::waitOn(std::string id){
    std::unique_lock<std::mutex> lock(locks[id]);
    cvs[id].wait(lock,[this,id]{
        if(pendingBlocks.find(id)!=pendingBlocks.end())
            return pendingBlocks[id].empty();
        else return false;
    });
}

template<typename T>
void Distributor<T>::configure(Parameter p, std::string value){
    settings[p] = value;
}

template<typename T>
void Distributor<T>::configure(Parameter p, int value){
    settings[p] = std::to_string(value);
}

template<typename T>
coap_response_t Distributor<T>::handleResponse(coap_session_t *session, const coap_pdu_t *sent, const coap_pdu_t *received, coap_mid_t id){
    Message m(session,received);

    std::string purpose = m.getHeaders()[Headers::PURPOSE];
    cout << "[receive] Received message with "+purpose+" purpose" << endl;

    if(purpose==PURPOSE_ASSISTANCE){
        handleAssistanceResponse(m);
    }
    else if(purpose==PURPOSE_HARDWARE){
        handleHardwareResponse(m);
    }
    else if(purpose==PURPOSE_RESULT){
        handleResultResponse(m);
    }
    else if(purpose==PURPOSE_ASSISTANCE){
        handleHealthResponse(m);
    }
    else{
        cout << "Unsupported message received. Skipping" << endl;
    }

    return COAP_RESPONSE_OK;
}


template<typename T>
void Distributor<T>::handleResultResponse(Message m){
    std::string uid = m.getHeaders()[Headers::CALCULATION_ID];
    std::string object = m.getHeaders()[Headers::SERIALIZED_TYPE];

    cout << "[handle-result] waiting on lock for uid="+uid << endl;
    std::unique_lock<std::mutex> lock(locks[uid]);

    if(object==SERIALIZED_TYPE_MATRIX){
        Matrix<T> result = Serializer::unserializeMatrix<T>(m.getContent());

        cout << "[handle-result] Extracted submatrix: "<< endl;
        result.show();
        cout << "[handle-result] Inserting..." << endl;
        if(storedPartialResults.find(uid)!=storedPartialResults.end()){
            storedPartialResults.at(uid).putSubmatrix(std::stoi(m.getHeaders()[Headers::INSERT_AT_X]), std::stoi(m.getHeaders()[Headers::INSERT_AT_Y]), result);

            Block block(std::stoi(m.getHeaders()[Headers::INSERT_AT_X]), std::stoi(m.getHeaders()[Headers::INSERT_AT_Y]));
            std::vector<Block>::iterator it = std::find(pendingBlocks[uid].begin(),pendingBlocks[uid].end(),block);
            if(it!=pendingBlocks[uid].end()){
                cout << "[handle-result] Removing result from pending" << endl;
                pendingBlocks.at(uid).erase(it);
            }
        }
    }
    else if(object==SERIALIZED_TYPE_ELEMENT){
        T element;

        if(typeid(T)==typeid(int)){
            element = std::stoi(m.getContent());
        }
        else if(typeid(T)==typeid(float)){
            element = std::stof(m.getContent());
        }
        else if(typeid(T)==typeid(double)){
            element = std::stod(m.getContent());
        }
        else{
            return;
        }

        cout << "[handle-result] Extracted element="+to_string(element)+". Inserting.." << endl;
        if(storedPartialResults.find(uid)!=storedPartialResults.end()){
            storedPartialResults.at(uid).put(std::stoi(m.getHeaders()[Headers::INSERT_AT_X]),std::stoi(m.getHeaders()[Headers::INSERT_AT_Y]),element);
            Block block(std::stoi(m.getHeaders()[Headers::INSERT_AT_X]),std::stoi(m.getHeaders()[Headers::INSERT_AT_Y]));

            std::vector<Block>::iterator it = std::find(pendingBlocks[uid].begin(),pendingBlocks[uid].end(),block);
            if(it!=pendingBlocks[uid].end()){
                cout << "[handle-result] Removing result from pending" << endl;
                pendingBlocks.at(uid).erase(it);
                cout << "[handle-result] "+std::to_string(pendingBlocks.at(uid).size())+" remaining to receive..." <<endl;
            }
        }
    }

    cout << "[handle-result] unlocking for uid="+uid << endl;
    lock.unlock();
    cout << "[handle-result] Notfiying waiting threads" << endl;
    cvs[uid].notify_all();
}

template<typename T>
void Distributor<T>::handleAssistanceResponse(Message m){
    std::string address = m.getPeer();

    cout << "[handle-assistance] Waiting on lock" << endl;
    std::unique_lock<std::mutex> lock(addressLock);
    Peer p;
    p.setAddress(address);
    if(!std::count(peers.begin(), peers.end(), p)){
        peers.push_back(p);
    }

    cout << "[handle-assistance] Unlocking" << endl;
    lock.unlock();
    cout << "[handle-assistance] Notifying waiting threads" << endl;
    addressCv.notify_all();
}

template<typename T>
void Distributor<T>::handleHardwareResponse(Message m){
    std::string address = m.getPeer();
    Hardware hw(m.getContent());
    cout << "[handle-hardware] received specifications:" << endl;
    cout << hw.toString() << std::endl;
    std::time_t delay = std::time(nullptr) - lastHardwareCheck;

    Peer peer(hw,address,delay);
    peers.push_back(peer);
}

template<typename T>
void Distributor<T>::handleHealthResponse(Message m){
    std::unique_lock<std::mutex> lock(addressLock);
    Peer p;
    p.setAddress(m.getPeer());

    if(std::find(peers.begin(),peers.end(),p)!=peers.end())
        healthyNodes.push_back(*std::find(peers.begin(),peers.end(),p));
    lock.unlock();
    addressCv.notify_all();
}

template<typename T>
std::string Distributor<T>::generateUID(){
    if(settings.find(Parameter::UID_SEED)!=settings.end()){
        uint64_t seed[] = {1};
        char* seedTable = reinterpret_cast<char*>(seed);
        settings[Parameter::UID_SEED] = std::string(seedTable,8);
    }

    char currUIDStr[settings[Parameter::UID_SEED].size()];
    strncpy(currUIDStr,settings[Parameter::UID_SEED].c_str(),settings[Parameter::UID_SEED].size());

    uint64_t* currUID = reinterpret_cast<uint64_t*>(currUIDStr);
    (*currUID) += 1;
    settings[Parameter::UID_SEED] = std::string(reinterpret_cast<char*>(currUID),8);
    return settings[Parameter::UID_SEED];
}

#endif