#include "../include/CoapEndpoint.h"

#include <iostream>
#include <time.h>
#include <arpa/inet.h>

CoapEndpoint::CoapEndpoint(std::string rootUri){
	localRootUri = coap_new_uri(reinterpret_cast<const uint8_t*>(rootUri.c_str()),rootUri.length());

	char uriBuf[localRootUri->host.length];
	strncpy(uriBuf,reinterpret_cast<const char*>(localRootUri->host.s),localRootUri->host.length);



    coap_address_init(localAddress);
	localAddress->addr.sin.sin_family = AF_INET;

	inet_pton(AF_INET, uriBuf, &localAddress->addr.sin.sin_addr);
	localAddress->addr.sin.sin_port = htons(localRootUri->port);

	context = coap_new_context(localAddress);

	if (!context) 
        throw "An error occured while creating the CoapEndpoint";

	keepAlive = true;
	loop = std::thread(&CoapEndpoint::run,this);
}

CoapEndpoint::~CoapEndpoint(){
	keepAlive = false;
	loop.join();
}

void CoapEndpoint::run(){
	while (keepAlive){
		int result = coap_io_process(context,COAP_IO_WAIT);
		if ( result < 0 )
			throw "Unable to query the result";
	}    
}

void CoapEndpoint::addResourceHandler(std::string path, coap_request_t method,coap_method_handler_t h){
	coap_str_const_t *uri = coap_new_str_const(reinterpret_cast<const uint8_t*>(path.c_str()),path.length());
	if(uri==NULL)
		throw "Could not transform URI to a CoAP string";

	coap_resource_t* resource = coap_resource_init(uri,COAP_RESOURCE_FLAGS_RELEASE_URI);
	if(resource==NULL)
		throw "Could not create a resource for that URI";
	coap_add_resource(context,resource);

	coap_register_handler(resource,method,h);
	registeredResources.insert(std::pair<std::pair<string,coap_request_t>,coap_resource_t*>(std::pair<std::string,coap_request_t>(path,method),resource));
}

void CoapEndpoint::registerResponseHandler(coap_response_handler_t h){
	coap_register_response_handler(context,h);
}

void CoapEndpoint::unregisterResponseHandler(){
	coap_register_response_handler(context,NULL);
}

void CoapEndpoint::unregisterResourceHandler(std::string path, coap_request_t method){
	coap_resource_t* r = registeredResources.at(std::pair<std::string,coap_request_t>(path,method));
	coap_register_handler(r,method,NULL);
	coap_delete_resource(context,r);
	registeredResources.erase(std::pair<std::string,coap_request_t>(path,method));
}

void CoapEndpoint::send(Message m){
	string destination = m.getDest();
	coap_session_t* sess;

	if(activeSessions.find(destination)==activeSessions.end()){
		coap_uri_t* destParts = coap_new_uri(reinterpret_cast<const uint8_t*>(destination.c_str()),destination.length());

		coap_address_t* dstAddr;
		coap_address_init(dstAddr);
		dstAddr->addr.sin.sin_family = AF_INET;

		char uriBuf[destParts->host.length];
		strncpy(uriBuf,reinterpret_cast<const char*>(destParts->host.s),destParts->host.length);

		inet_pton(AF_INET, uriBuf, &dstAddr->addr.sin.sin_addr);
		dstAddr->addr.sin.sin_port = htons(destParts->port);

		sess = coap_new_client_session(context,localAddress,dstAddr,COAP_PROTO_UDP);
		activeSessions.insert(pair<string,coap_session_t*>(destination,sess));

		coap_session_init_token(sess,8,reinterpret_cast<const uint8_t*>("01234567"));
	}
	else{
		sess = activeSessions.at(destination);
	}
	
	coap_pdu_t* coapMessage = m.toCoapMessage(sess);

	if(coap_send(sess,coapMessage)==COAP_INVALID_MID)
		throw("Unable to send message");
}

void CoapEndpoint::endSession(string uri){
	coap_session_free(activeSessions.at(uri));
	activeSessions.erase(uri);
}