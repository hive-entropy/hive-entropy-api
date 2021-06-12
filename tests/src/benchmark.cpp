#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include "catch.hpp"
#include "Matrix.h"
#include "HiveEntropyNode.h"
#include "Distributor.h"
#include <random>
#include <time.h>

#define _COTE 100

TEST_CASE("Benchmarking"){

    srand((unsigned) time(NULL));

    Matrix<float> a(_COTE,_COTE);
    Matrix<float> b(_COTE,_COTE);

    for(int i=0; i< _COTE; i++){
        for(int j=0; j<_COTE; j++){
            a.put(i,j,(float) rand()/(float)(RAND_MAX/255));
            b.put(i,j,(float) rand()/(float)(RAND_MAX/255));
        }
    }

    HiveEntropyNode* n = new HiveEntropyNode("192.168.1.35:6969");
    BENCHMARK("Distributed RowColumn"){
        Distributor<float> dist(n);
        dist.configure(Parameter::ASSISTANCE_MAX_PARTICIPANTS,4);
        std::string id = dist.distributeMatrixMultiplication(a,b,MultiplicationMethod::MULTIPLE_ROW_COLUMN);
        return dist.get(id);
    };

    // BENCHMARK("Distributed Cannon"){
    //     HiveEntropyNode* n = new HiveEntropyNode("192.168.1.35:6969");
    //     Distributor<float> dist(n);
    //     dist.configure(Parameter::ASSISTANCE_MAX_PARTICIPANTS,4);
    //     std::string id = dist.distributeMatrixMultiplication(a,b,MultiplicationMethod::CANNON);
    //     return dist.get(id);
    // };

}