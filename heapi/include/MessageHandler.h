#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <coap2/coap.h>
#include "MessageHandler.h"
#include "Message.h"
#include "Serializer.h"
#include "GlobalContext.h"
#include "HiveEntropyNode.h"

#include <string>
#include <list>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

#define SEC 10

class MessageHandler
{
public:
    MessageHandler();
    ~MessageHandler();

    static void lockTask(string nodeAdress, string calculationId, string taskId){
        std::map<string, bool>* adressList = GlobalContext<std::map<std::string, bool>>::get("adress");
        adressList->at(nodeAdress) = false;
    }
    static void unlockTask(string nodeAdress, string calculationId, string taskId){
        std::map<std::string, bool>* adressList = GlobalContext<std::map<std::string, bool>>::get("adress");
        adressList->at(nodeAdress) = true;
    }

    static void handleResult(Message m){
        cout << "handle result : " << m.getPeer() << endl;
        
        // WAIT TO HANDLE cannon
        unlockTask(m.getPeer(), m.getHeaders()[Headers::CALCULATION_ID], m.getHeaders()[Headers::TASK_ID]);
        float c = strtof(m.getContent().c_str(), NULL);
        Matrix<float>* result = GlobalContext<Matrix<float>>::get("result"+m.getHeaders()[Headers::CALCULATION_ID]);
        if(result!=nullptr){
            result->put(stoi(m.getHeaders()[Headers::INSERT_AT_X]),stoi(m.getHeaders()[Headers::INSERT_AT_Y]),c);
        }
        
    }

    static void handleAssistance(Message m){
        string peer = m.getPeer();
        std::map<string, bool>* adressList =  GlobalContext<std::map<string, bool>>::get("adress");
        adressList->insert(pair<string, bool>(peer, true));
        cout << peer << "ready to assist" << peer << endl;
    }
    static void handleArdware(){}
    static void handleHealth(){}
    
    static coap_response_t handleMessage(coap_context_t *context, coap_session_t *session, coap_pdu_t *sent, coap_pdu_t *received, const coap_mid_t id){
        cout << "got message : " ;
        Message m(session,received);

        std::string purpose = m.getHeaders()[Headers::PURPOSE];
        cout << purpose << endl;
        
        if (purpose == PURPOSE_RESULT){ handleResult(m); }
        else if(purpose == PURPOSE_ASSISTANCE){ handleAssistance(m);}
        
        return COAP_RESPONSE_OK;
    }

    static bool checkResponses(std::string calculationId){
        bool isListEmpty = true;
        std::list<std::string>* taskList = GlobalContext<std::list<std::string>>::get(calculationId);

        for (std::list<std::string>::iterator it=taskList->begin(); it != taskList->end(); ++it)
            isListEmpty = false;

        return isListEmpty;
    }

    static bool waitResponses(std::string calculationId){
        bool allTasksDone = false;
        int sec = 0;
        while(sec < SEC && !allTasksDone){
            allTasksDone =  checkResponses(calculationId);
            sec++;
            usleep(1000);
        }
        std::cout << "all tasks done : " << allTasksDone << std::endl;
        return allTasksDone;
    }

    static std::string getAvailableNode(){
        std::string availableAdress;
        int sec = 0;
        std::map<std::string, bool>* adressList = GlobalContext<std::map<std::string, bool>>::get("adress");

        while(sec < SEC)
        for (std::map<std::string, bool>::iterator it=adressList->begin(); it != adressList->end(); ++it){
            if(it->second == true){
                it->second = false;
                return it->first;
            }
            usleep(1000);
            sec++;
        }

        return "";
    }

    static bool divideCalculInTasksCannon(HiveEntropyNode* node, Matrix<float> A, Matrix<float> B, std::string calculationId){
        return true;

    }

    static bool divideCalculInTasks(HiveEntropyNode* node, Matrix<float> A, Matrix<float> B, std::string calculationId){
        // test row col pr l'instant

        std::list<std::string> tasklist;
        int taskId = 0, sec = 0;
        std::string nodeAdress = "";
        Matrix<float> result(9,9);
        GlobalContext<Matrix<float>>::registerObject("result" + calculationId,result);

        for (int i = 0; i < A.getRows(); i++)
        {
            for (int j = 0; j < B.getColumns(); j++)
            {
                while (sec<SEC && nodeAdress == "")
                {
                    nodeAdress = getAvailableNode();
                    sec++;

                }
                if (sec>=SEC)
                {
                    return false;
                }

                tasklist.push_back(""+taskId);
                //MessageHandler::lockTask(nodeAdress, calculationId, ""+taskId);
                node->sendMatrixMultiplicationTask("coap://"+nodeAdress,Row<float>(9,i,A.getRow(i)),Column<float>(9,j,B.getColumn(j)),calculationId, ""+taskId);

                nodeAdress = "";
                sec = 0;
                taskId++;
            }

        }

        GlobalContext<std::list<std::string>>::registerObject(calculationId,tasklist);
        return true;
    }
};

#endif