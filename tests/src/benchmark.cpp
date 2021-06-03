#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include "catch.hpp"
#include "Matrix.h"
#include "HiveEntropyNode.h"
#include "Distributor.h"
#include <random>
#include <time.h>

#define _COTE 120

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

    BENCHMARK("Local"){
        return a*b;
    };

    HiveEntropyNode* n = new HiveEntropyNode("192.168.1.42:6969");
    BENCHMARK("Distributed RowColumn"){
        Distributor<float> dist(n);
        dist.configure(Parameter::ASSISTANCE_MAX_PARTICIPANTS,2);
        std::string id = dist.distributeMatrixMultiplication(a,b);
        return dist.get(id);
    };

    BENCHMARK("Naive Local"){
        Matrix<float> c(_COTE,_COTE);
        for(int i=0; i<_COTE; i++){
            for(int j=0; j<_COTE; j++){
                for(int k=0; k<_COTE; k++){
                    c.put(i,j,c.get(i,j)+a.get(i,k)*b.get(k,j));
                }
            }
        }
        return c;
    };

}