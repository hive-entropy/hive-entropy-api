#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <coap2/coap.h>
#include "MessageHandler.h"
#include "Message.h"
#include "Serializer.h"
#include "GlobalContext.h"

#include <string>
#include <list>
#include <iostream>

class MessageHandler
{
public:
    MessageHandler();
    ~MessageHandler();

    static void lockTask(std::string calculationId, std::string taskId){
        GlobalContext<std::list<std::string>>::get(calculationId)->push_back(taskId);
    }
    static void unlockTask(std::string calculationId,std::string taskId){
        GlobalContext<std::list<std::string>>::get(calculationId)->remove(taskId);
    }

    static void handleResult(Message m){
        cout << "handle result" << endl;
        if(m.getHeaders()[Headers::PURPOSE]== PURPOSE_RESULT && m.getHeaders()[Headers::SERIALIZED_TYPE]=="matrix"){
            if(m.getHeaders()[Headers::ELEMENT_TYPE]==typeid(float).name()){
                unlockTask(m.getHeaders()[Headers::CALCULATION_ID], m.getHeaders()[Headers::TASK_ID]);

                Matrix<float> c = Serializer::unserializeMatrix<float>(m.getContent());
                Matrix<float>* total = GlobalContext<Matrix<float>>::get(m.getHeaders()[Headers::CALCULATION_ID]);
                if(total!=nullptr){
                    total->putSubmatrix(stoi(m.getHeaders()[Headers::INSERT_AT_X]),stoi(m.getHeaders()[Headers::INSERT_AT_Y]),c);
                }
            }
        }
    }

    static void handleAssistance(){}
    static void handleArdware(){}
    static void handleHealth(){}


    
    static coap_response_t handleMessage(coap_context_t *context, coap_session_t *session, coap_pdu_t *sent, coap_pdu_t *received, const coap_mid_t id){
        cout << "got message" << endl;
        Message m(session,received);

        

        std::string purpose = m.getHeaders()[Headers::PURPOSE];
        
        if (purpose == PURPOSE_RESULT){ handleResult(m); }
        else if(purpose == PURPOSE_ASSISTANCE){ handleAssistance();}
        
        return COAP_RESPONSE_OK;
    }

};


#endif