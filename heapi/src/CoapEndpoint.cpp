#include "CoapEndpoint.h"

#include <iostream>
#include <time.h>
#include <arpa/inet.h>

CoapEndpoint::CoapEndpoint(std::string rootUri){

	coap_set_log_level(LOG_DEBUG);

	//Failsafe URI format
	if(rootUri.find("coap://")==std::string::npos)
		rootUri = "coap://"+rootUri;
	if(rootUri.find_last_of("/")==std::string::npos||rootUri.find_last_of("/")!=rootUri.length()-1)
		rootUri += "/";

	localRootUri = coap_new_uri(reinterpret_cast<const uint8_t*>(rootUri.c_str()),rootUri.length());

	char* uriBuf = (char*) malloc(localRootUri->host.length*sizeof(char));
	strncpy(uriBuf,reinterpret_cast<const char*>(localRootUri->host.s),localRootUri->host.length);



    coap_address_init(&localAddress);
	localAddress.addr.sin.sin_family = AF_INET;

	inet_pton(AF_INET, uriBuf, &localAddress.addr.sin.sin_addr);
	localAddress.addr.sin.sin_port = htons(localRootUri->port);

	context = coap_new_context(&localAddress);

	if (!context) 
        throw "An error occured while creating the CoapEndpoint";

	coap_context_set_block_mode(context,COAP_BLOCK_USE_LIBCOAP|COAP_BLOCK_SINGLE_BODY); 

	coap_register_option(context, COAP_OPTION_URI_PATH);
	coap_register_option(context, COAP_OPTION_PROXY_URI);
	coap_register_option(context, 2049);
	coap_register_option(context, 2051);
	coap_register_option(context, 2053);
	coap_register_option(context,2055);
	coap_register_option(context, 2057);

	if(coap_join_mcast_group_intf(context,"239.0.0.1",NULL)!=0)
		throw "Couldn't join multicast";

	keepAlive = true;
	loop = std::thread(&CoapEndpoint::run,this);
}

CoapEndpoint::~CoapEndpoint(){
	keepAlive = false;
	loop.join();
	coap_free_context(context);
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

	string sessId = destination;
	if(sessId.find("coap://")==0)
		sessId = sessId.substr(7,sessId.size()-1);
	if(sessId.find(":")!=std::string::npos)
		sessId = sessId.substr(0,sessId.find(":"));
	if(sessId.find(":")!=std::string::npos)
		sessId = sessId.substr(0,sessId.find("/"));

	coap_session_t* sess;

	coap_uri_t* destParts = coap_new_uri(reinterpret_cast<const uint8_t*>(destination.c_str()),destination.length());

	coap_address_t dstAddr;
	coap_address_init(&dstAddr);
	dstAddr.addr.sin.sin_family = AF_INET;

	inet_pton(AF_INET, sessId.c_str(), &dstAddr.addr.sin.sin_addr);
	dstAddr.addr.sin.sin_port = htons(destParts->port);

	sess = coap_session_get_by_peer(context,&dstAddr,0);
	if(!sess){
		sess = coap_new_client_session(context,&localAddress,&dstAddr,COAP_PROTO_UDP);
		//coap_session_set_max_retransmit(sess,10);
		coap_session_init_token(sess,8,reinterpret_cast<const uint8_t*>("01234567"));
	}
	
	coap_pdu_t* coapMessage = m.toCoapMessage(sess);

	if(coap_send_large(sess,coapMessage)==COAP_INVALID_MID)
		throw("Unable to send message");
}

void CoapEndpoint::waitForDeath(){
	loop.join();
}