#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <map>

#include "Matrix.h"
#include "../lib/libcoap/include/coap2/coap.h.in"

using namespace std;

/**
 * @brief Represents an application-level message that can transit through the network.
 */
class Message{
    public:

        /**
         * @brief An enumeration of possible headers for a message to use.
         */
        enum Headers{
            PURPOSE,
            CALCULATION_ID,
            TASK_ID,
            INSERT_AT_X,
            INSERT_AT_Y,
            BORDER_SIZE,
            STEPS,
            ELEMENT_TYPE,
            PROXY_URI,
            ENCODING_TYPE,
            SERIALIZED_TYPE
        };

        /**
         * @brief An enumeration of possible message types as defined in RFC 7252.
         */
        enum MessageType{
            CONFIRMABLE,
            NON_CONFIRMABLE,
            ACK,
            RESET
        };

        /**
         * @brief An enumeration of supported HTTP Methods.
         * 
         */
        enum HttpMethod{
            GET,
            POST,
            PUT,
            DELETE
        };

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
         * @brief Set the Content oof the message.
         * 
         * @param content the content to put in the message.
         */
        void setContent(string content);

        void setHttpMethod(HttpMethod m);

        HttpMethod getHttpMethod();
    private:
        string dest;
        HttpMethod httpMethod;
        map<Headers,string> headers;
        string content;
        MessageType type;
};

#endif