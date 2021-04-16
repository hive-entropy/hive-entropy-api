#include "../include/Message.h"

Message::Message(){
    headers = std::map<Headers,std::string>();
}

Message::~Message(){}

std::map<Message::Headers,std::string> Message::getHeaders(){
    return headers;
}

string Message::getDest(){
    return dest;
}

Message::MessageType Message::getType(){
    return type;
}

Message::HttpMethod Message::getHttpMethod(){
    return httpMethod;
}

std::string Message::getContent(){
    return content;
}


void Message::addHeader(Headers h, string value){
    headers.insert(std::pair<Headers,std::string>(h,value));
}

void Message::setContent(std::string content){
    this->content = content;
}

void Message::setDest(std::string dest){
    this->dest = dest;
}

void Message::setHttpMethod(HttpMethod method){
    this->httpMethod = httpMethod;
}

void Message::setType(MessageType type){
    this->type = type;
}

coap_pdu_t* Message::toCoapMessage(coap_session_t* sess){

    //TODO Require at least Type and Method

    coap_new_pdu(sess);

    uint8_t coapType, coapMethod;

    switch (type){
        case CONFIRMABLE:
            coapType = COAP_MESSAGE_CON;
        break;
        case NON_CONFIRMABLE:
            coapType = COAP_MESSAGE_NON;
        break;
        case ACK:
            coapType = COAP_MESSAGE_ACK;
        break;
        case RESET:
            coapType = COAP_MESSAGE_ACK;
        break;
    }

    switch (type){
        case GET:
            coapMethod = COAP_REQUEST_GET;
        break;
        case POST:
            coapMethod = COAP_REQUEST_POST;
        break;
        case PUT:
            coapMethod = COAP_REQUEST_PUT;
        break;
        case DELETE:
            coapMethod = COAP_REQUEST_DELETE;
        break;
    }

    coap_optlist_t *optlist_chain = NULL;

    coap_pdu_t* transformed = coap_pdu_init(coapType,coapMethod,coap_new_message_id(sess),coap_session_max_pdu_size(sess));

    //====TOKEN====
    size_t toklen;
    uint8_t token_buf[1024];
    coap_session_new_token(sess,&toklen,token_buf);
    if(coap_add_token(transformed,toklen,token_buf)==0)
        throw "Unable to add token to the message";

    //====URI====
    size_t urilen;
    uint8_t buf[256];
    uint8_t* uri_buf = buf;
    int count = coap_split_path(reinterpret_cast<const uint8_t*>(dest.c_str()), dest.length(), uri_buf, &urilen);
    while (count--) {
      if (!coap_insert_optlist(&optlist_chain,
                               coap_new_optlist(COAP_OPTION_URI_PATH,
                        coap_opt_length(uri_buf), coap_opt_value(uri_buf))))
        throw "Couldn't insert URI option";
      uri_buf += coap_opt_size(uri_buf);
    }

    //====CONTENT DECLARATION====
    if(!content.empty())
        if(coap_add_data(transformed,content.length(),reinterpret_cast<const uint8_t*>(content.c_str()))==0)
            throw "Unable to add content to the message";


    //====ADD OPTIONS====
    for(std::map<Headers,std::string>::iterator it = headers.begin();it !=headers.end(); ++it){
        uint16_t opt_num;
        switch(it->first){
            case PURPOSE:
                opt_num = 2048;
            break;
            case CALCULATION_ID:
                opt_num = 2049;
            break;
            case TASK_ID:
                opt_num = 2050;
            break;
            case INSERT_AT_X:
                opt_num = 2051;
            break;
            case INSERT_AT_Y:
                opt_num = 2052;
            break;
            case BORDER_SIZE:
                opt_num = 2053;
            break;
            case STEPS:
                opt_num = 2054;
            break;
            case ELEMENT_TYPE:
                opt_num = 2055;
            break;
            case PROXY_URI:
                opt_num = COAP_OPTION_PROXY_URI;
            break;
        }

        if (!coap_insert_optlist(&optlist_chain,coap_new_optlist(COAP_OPTION_URI_PATH,it->second.size(),reinterpret_cast<const uint8_t*>(it->second.c_str()))));
            throw "Couldn't insert passed option";
    }

    //====ADD OPTLIST====
    if (!coap_add_optlist_pdu(transformed, &optlist_chain))
        throw "Could not add option list to message";

    return transformed;
}