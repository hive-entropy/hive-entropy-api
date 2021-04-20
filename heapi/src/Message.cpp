#include <arpa/inet.h>
#include "Message.h"

Message::Message(){
    headers = std::map<Headers,std::string>();
}

Message::Message(coap_session_t* sess, coap_pdu_t* pdu){
    //TYPE
    switch(pdu->type){
        case COAP_MESSAGE_CON:
            type = MessageType::CONFIRMABLE;
        break;
        case COAP_MESSAGE_NON:
            type = MessageType::NON_CONFIRMABLE;
        break;
        case COAP_MESSAGE_ACK:
            type = MessageType::ACK;
        break;
        case COAP_MESSAGE_RST:
            type = MessageType::RESET;
        break;
    }

    //METHOD
     switch(pdu->code){
        case COAP_REQUEST_GET:
            httpMethod = HttpMethod::GET;
        break;
        case COAP_REQUEST_POST:
            httpMethod = HttpMethod::POST;
        break;
        case COAP_REQUEST_PUT:
            httpMethod = HttpMethod::PUT;
        break;
        case COAP_REQUEST_DELETE:
            httpMethod = HttpMethod::DELETE;
        break;
        case COAP_RESPONSE_CODE_OK:
            httpMethod = HttpMethod::OK;
        break;
        case COAP_RESPONSE_CODE_NOT_FOUND:
            httpMethod = HttpMethod::NOT_FOUND;
        break;
        case COAP_RESPONSE_CODE_BAD_REQUEST:
            httpMethod = HttpMethod::BAD_REQUEST;
        break;
    }

    //OPTIONS
    coap_opt_iterator_t iter;
    coap_option_iterator_init(pdu,&iter,COAP_OPT_ALL);

    headers = std::map<Headers,std::string>();

    coap_opt_t* current = coap_option_next(&iter);
    while(current!=NULL){
        
        std::string optVal(reinterpret_cast<const char*>(coap_opt_value(current)),coap_opt_length(current));
        
        switch(iter.type){
            case 2048:
                headers.insert(std::pair<Headers,std::string>(Headers::PURPOSE,optVal));
            break;
            case 2049:
                headers.insert(std::pair<Headers,std::string>(Headers::CALCULATION_ID, optVal));
            break;
            case 2050:
                headers.insert(std::pair<Headers,std::string>(Headers::TASK_ID, optVal));
            break;
            case 2051:
                headers.insert(std::pair<Headers,std::string>(Headers::INSERT_AT_X, optVal));
            break;
            case 2052:
                headers.insert(std::pair<Headers,std::string>(Headers::INSERT_AT_Y, optVal));
            break;
            case 2053:
                headers.insert(std::pair<Headers,std::string>(Headers::BORDER_SIZE, optVal));
            break;
            case 2054:
                headers.insert(std::pair<Headers,std::string>(Headers::STEPS, optVal));
            break;
            case 2055:
                headers.insert(std::pair<Headers,std::string>(Headers::ELEMENT_TYPE, optVal));
            break;
            case 2056:
                headers.insert(std::pair<Headers,std::string>(Headers::ENCODING_TYPE, optVal));
            break;
            case 2057:
                headers.insert(std::pair<Headers,std::string>(Headers::SERIALIZED_TYPE, optVal));
            break;
            case 2058:
                headers.insert(std::pair<Headers,std::string>(Headers::ASSISTANCE_RESPONSE, optVal));
            break;
            case COAP_OPTION_PROXY_URI:
                headers.insert(std::pair<Headers,std::string>(Headers::PROXY_URI, optVal));
            break;
        }

        current = coap_option_next(&iter);
    }

    //CONTENT

    size_t bodySize;
    uint8_t * data;
    if(coap_get_data(pdu,&bodySize,&data)){
        std::string tempContent(reinterpret_cast<const char*>(data),bodySize);
        if(tempContent.length()>0){
            content = tempContent;
        }
    }

    //DEST
    char dest_buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,&(sess->addr_info.local.addr.sin.sin_addr),dest_buf,INET_ADDRSTRLEN);
    std::string tempHost(dest_buf);
    dest = tempHost+":"+to_string(htons(sess->addr_info.local.addr.sin.sin_port));
}

Message::~Message(){}

std::map<Headers,std::string> Message::getHeaders(){
    return headers;
}

string Message::getDest(){
    return dest;
}

MessageType Message::getType(){
    return type;
}

HttpMethod Message::getHttpMethod(){
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
    this->httpMethod = method;
}

void Message::setType(MessageType type){
    this->type = type;
}

coap_pdu_t* Message::toCoapMessage(coap_session_t* sess){

    // if(!httpMethod||!type)
    //     throw "You must specify HTTP Method and Message type";

    coap_new_pdu(sess);

    uint8_t coapType, coapMethod;

    switch (type){
        case MessageType::CONFIRMABLE:
            coapType = COAP_MESSAGE_CON;
        break;
        case MessageType::NON_CONFIRMABLE:
            coapType = COAP_MESSAGE_NON;
        break;
        case MessageType::ACK:
            coapType = COAP_MESSAGE_ACK;
        break;
        case MessageType::RESET:
            coapType = COAP_MESSAGE_ACK;
        break;
    }

    switch (httpMethod){
        case HttpMethod::GET:
            coapMethod = COAP_REQUEST_GET;
        break;
        case HttpMethod::POST:
            coapMethod = COAP_REQUEST_POST;
        break;
        case HttpMethod::PUT:
            coapMethod = COAP_REQUEST_PUT;
        break;
        case HttpMethod::DELETE:
            coapMethod = COAP_REQUEST_DELETE;
        break;
        case HttpMethod::OK:
            coapMethod = COAP_RESPONSE_CODE_OK;
        break;
        case HttpMethod::NOT_FOUND:
            coapMethod = COAP_RESPONSE_CODE_NOT_FOUND;
        break;
        case HttpMethod::BAD_REQUEST:
            coapMethod = COAP_RESPONSE_CODE_BAD_REQUEST;
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
    for(std::pair<Headers,std::string> el : headers){
        uint16_t opt_num;
        switch(el.first){
            case Headers::PURPOSE:
                opt_num = 2048;
            break;
            case Headers::CALCULATION_ID:
                opt_num = 2049;
            break;
            case Headers::TASK_ID:
                opt_num = 2050;
            break;
            case Headers::INSERT_AT_X:
                opt_num = 2051;
            break;
            case Headers::INSERT_AT_Y:
                opt_num = 2052;
            break;
            case Headers::BORDER_SIZE:
                opt_num = 2053;
            break;
            case Headers::STEPS:
                opt_num = 2054;
            break;
            case Headers::ELEMENT_TYPE:
                opt_num = 2055;
            break;
            case Headers::ENCODING_TYPE:
                opt_num = 2056;
            break;
            case Headers::SERIALIZED_TYPE:
                opt_num = 2057;
            break;
            case Headers::ASSISTANCE_RESPONSE:
                opt_num = 2058;
            break;
            case Headers::PROXY_URI:
                opt_num = COAP_OPTION_PROXY_URI;
            break;
        }

        if (!coap_insert_optlist(&optlist_chain,coap_new_optlist(opt_num, el.second.size(),reinterpret_cast<const uint8_t*>(el.second.c_str()))))
            throw "Couldn't insert passed option";
    }

    //====ADD OPTLIST====
    if (!coap_add_optlist_pdu(transformed, &optlist_chain))
        throw "Could not add option list to message";

    return transformed;
}