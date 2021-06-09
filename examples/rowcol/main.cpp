#include <HiveEntropyAPI/full.h>

#define _COTE 10

int main(void){

    srand((unsigned) time(NULL));

    Matrix<float> a(_COTE,_COTE);
    Matrix<float> b(_COTE,_COTE);

    for(int i=0; i< _COTE; i++){
        for(int j=0; j<_COTE; j++){
            a.put(i,j,(float) rand()/(float)(RAND_MAX/10));
            b.put(i,j,(float) rand()/(float)(RAND_MAX/10));
        }
    }


    cout << "[main] Doing calculation locally" << endl;
    Matrix<float> localC = a*b;
    localC.show();


    HiveEntropyNode* n = new HiveEntropyNode("192.168.1.35:6969");
    Distributor<float> dist(n);

    cout << "[main] Requesting distributed multiplication" << endl;

    std::string calc1 = dist.distributeMatrixMultiplication(a,b,MultiplicationMethod::MULTIPLE_ROW_COLUMN);

    cout << "[main] Received calculationId="+calc1 << endl;
    cout << "[main] Doing unrelated work" << endl;
    
    cout << "[main] Getting my results..." << endl;
    Matrix<float> C = dist.get(calc1);
    C.show();

    return 0;
}