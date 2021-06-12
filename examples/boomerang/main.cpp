#include <HiveEntropyAPI/full.h>
#include <chrono>

#define _COTE 2500

static std::chrono::steady_clock::time_point measure;

static coap_response_t handleResponse(coap_session_t *session, const coap_pdu_t *sent, const coap_pdu_t *received, coap_mid_t id){
    Message m(session,received);

    std::string purpose = m.getHeaders()[Headers::PURPOSE];
    cout << "[receive] Received message with "+purpose+" purpose" << endl;

    if(purpose=="around"){
        cout << "Chronometre: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-measure).count() << endl;
    }
    else{
        cout << "Unsupported message received. Skipping" << endl;
    }

    return COAP_RESPONSE_OK;
}

int main(void){
    srand((unsigned) time(NULL));

    Matrix<float> a(_COTE,_COTE);

    for(int i=0; i< _COTE; i++){
        for(int j=0; j<_COTE; j++){
            a.put(i,j,(float) rand()/(float)(RAND_MAX/10));
        }
    }

    HiveEntropyNode n("192.168.1.35:6969");
    n.registerResponseHandler(handleResponse);
    
    Message m;
    m.setDest("coap://192.168.1.35:9999/boomerang");
    m.setContent(Serializer::serialize(a));
    m.setType(MessageType::CONFIRMABLE);
    m.setHttpMethod(HttpMethod::PUT);
    measure = std::chrono::steady_clock::now();
    n.send(m);

    n.keepAlive();

    return 0;
}