#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <map>

#include <coap2/coap.h>

#include "Matrix.h"

using namespace std;

///Possible purposes
#define PURPOSE_ASSISTANCE "assistance"
#define PURPOSE_RESULT "result"
#define PURPOSE_HEALTH "health"
#define PURPOSE_HARDWARE "hardware"

///Possible serialized types
#define SERIALIZED_TYPE_MATRIX "matrix"
#define SERIALIZED_TYPE_MATRICES "matrices"
#define SERIALIZED_TYPE_ROWCOL "rowcol"
#define SERIALIZED_TYPE_ELEMENT "element"

/**
 * @brief An enumeration of possible headers for a message to use.
 */
enum class Headers : int{
    PURPOSE=1,
    CALCULATION_ID=2,
    TASK_ID=3,
    INSERT_AT_X=4,
    INSERT_AT_Y=5,
    BORDER_SIZE=6,
    STEPS=7,
    ELEMENT_TYPE=8,
    PROXY_URI=9,
    ENCODING_TYPE=10,
    SERIALIZED_TYPE=11,
    ASSISTANCE_RESPONSE=12
};

/**
 * @brief An enumeration of possible message types as defined in RFC 7252.
 */
enum class MessageType : int{
    CONFIRMABLE=1,
    NON_CONFIRMABLE=2,
    ACK=3,
    RESET=4
};

/**
    * @brief An enumeration of supported HTTP Methods.
    * 
    */
enum class HttpMethod : int{
    GET=1,
    POST=2,
    PUT=3,
    DELETE=4,
    OK=5,
    NOT_FOUND=6,
    BAD_REQUEST=7
};

struct MessageData {
    coap_resource_t *resource;
    coap_session_t *session;
    coap_pdu_t *request;
    coap_binary_t *token;
    coap_pdu_t *response;
};

/**
 * @brief Represents an application-level message that can transit through the network.
 */
class Message{
    public:
        /**
         * @brief Construct a new Message object.
         */
        Message();

        /**
         * @brief Construct a new Message object from a CoAP PDU structure.
         * 
         * @param session The current session.
         * @param pdu The pdu structure of the message.
         * @param token The optional token to use.
         */
        Message(coap_session_t* sess, coap_pdu_t* pdu);

        /**
         * @brief Construct a new Message object from CoAP message data.
         *
         * @param resource
         * @param session The current session.
         * @param request
         * @param token The optional token to use.
         * @param response
         */
        explicit Message(coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request, coap_binary_t *token,
                         coap_pdu_t *response);

        /**
         * @brief Destroy the Message object
         */
        ~Message();

        /**
         * @brief Converts the Message object to a coap_pdu_t structure.
         * 
         * @return coap_pdu_t the structure that will eventually be sent through the network.
         */
        coap_pdu_t* toCoapMessage(coap_session_t* sess);

        /**
         * @brief Set the destination of the Message.
         * 
         * @param dest the destination.
         */
        void setDest(string dest);

        /**
         * @brief Adds a header attribute to the message.
         * 
         * @param key The key of the header to add to the message.
         * @param val The value for the referenced header.
         */
        void addHeader(Headers key, string val);

        /**
         * @brief Set the CoAP type for the message.
         * 
         * @param type The target type of the message.
         */
        void setType(MessageType type);

        /**
         * @brief Get the destination of the message.
         * 
         * @return string the destination of the message.
         */
        string getDest();

        /**
         * @brief Get the headers of the message.
         * 
         * @return vector<Headers, string> A map between headers and their values.
         */
        map<Headers,string> getHeaders();

        /**
         * @brief Get the type of the message.
         * 
         * @return MessageType the type of the message.
         */
        MessageType getType();

        /**
         * @brief Get the Content of the message.
         * 
         * @return string the content of the message.
         */
        string getContent();

        /**
         * @brief Set the Content of the message.
         * 
         * @param content the content to put in the message.
         */
        void setContent(string content);

        /**
         * @brief Set the Http Method of the message.
         * 
         * @param m the Http Method to put in the message.
         */
        void setHttpMethod(HttpMethod m);

        /**
         * @brief Fills a response PDU with message data.
         */
        void fillResponse();

        /**
         * @brief Get the Http Method of the message.
         * 
         * @return HttpMethod the Http Method of the message.
         */
        HttpMethod getHttpMethod();
    private:
        string dest;
        HttpMethod httpMethod;
        map<Headers,string> headers;
        string content;
        MessageType type;
        MessageData data;
};

#endif