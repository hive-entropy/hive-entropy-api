#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <vector>

#include "Matrix.h"
#include "../lib/libcoap/include/coap2/pdu.h"

using namespace std;

class Message{
    public:
        Message();
        coap_pdu_t toCoapMessage();

        void setDest(string dest);
        void setOrigin(string origin);
        void addHeader(string key, string val);
        void setType(string type);

    private:
        string origin;
        string dest;
        vector<string,string> headers;
        string content;
        string type;
};

#endif