#include <full.h>

int main(void){

    float a[] = {1,2,3,4,5,6,7,8,9,
                        1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5,
                        9,2,7,4,5,6,3,8,1};
    Matrix<float> A(3,9,a);
    Matrix<float> B(9,3,MatrixArchetype::ID);


    cout << "[main] Doing calculation locally" << endl;
    Matrix<float> localC = A*B;
    localC.show();


    HiveEntropyNode* n = new HiveEntropyNode("127.0.0.1:6969");
    Distributor<float> dist(n);

    cout << "[main] Requesting distributed multiplication" << endl;

    std::string calc1 = dist.distributeMatrixMultiplication(A,B,MultiplicationMethod::CANNON);

    cout << "[main] Received calculationId="+calc1 << endl;
    cout << "[main] Doing unrelated work" << endl;
    
    cout << "[main] Getting my results..." << endl;
    Matrix<float> C = dist.get(calc1);
    C.show();




    // Matrix<float> C(3,3,MatrixArchetype::ZEROS);

    // try{
    //     //On crée le noeud et on enregistre le callback
    //     //HiveEntropyNode n("192.168.57.1:6969");
    //     HiveEntropyNode n("127.0.0.1:6969");
    //     n.registerResponseHandler(MessageHandler::handleMessage);


    //     n.queryNodeAvailability();
        
    //     std::string id="potato";
    //     GlobalContext<Matrix<float>>::registerObject(id, C);

    //     cout << "Sending..." << endl;
    //     MessageHandler::divideCalculInTasks(&n, A, B, id);

    //     cout << "result from nodes " << endl; 
    //     Matrix<float>* total = GlobalContext<Matrix<float>>::get("result"+id);
    //     total->show();
    //     cout << "result " << endl;
    //     C = A*B;
    //     C.show();
    //     cout << "end" << endl;
    //     n.keepAlive();
    // }
    // catch(const char* e){
    //     cout << std::string(e) << endl;
    // }

    return 0;
}