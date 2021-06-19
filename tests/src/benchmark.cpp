#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include "catch.hpp"
#include "Matrix.h"
#include "HiveEntropyNode.h"
#include "Distributor.h"
#include <random>
#include <time.h>

#define _COTE 800

TEST_CASE("Benchmarking"){

    srand((unsigned) time(NULL));

    Matrix<float> a(_COTE,_COTE);

    for(int i=0; i< _COTE; i++){
        for(int j=0; j<_COTE; j++){
            a.put(i,j,(float) rand());
        }
    }

    float b_tab[] = {1,2,1,2,4,2,1,2,1};
    Matrix<float> blur(3,3,b_tab);

    std::shared_ptr<HiveEntropyNode> n = std::make_shared<HiveEntropyNode>("192.168.1.35:6969");
    Distributor<float> dist(n);
    dist.configure(Parameter::ASSISTANCE_MAX_PARTICIPANTS,4);
    BENCHMARK("Distributed RowColumn"){
        std::string id = dist.distributeMatrixConvolution(a,blur);
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