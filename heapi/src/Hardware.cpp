#include "hardware.h"
#include <sys/sysinfo.h>
#include <sstream>
#include <iostream>
#include<string>  
using namespace std;

#define UNIX
#ifdef UNIX
int getProcNumber = get_nprocs();
#endif

Hardware::Hardware(){
    processorCoreNumber = 8;
    processorFrequency = 2.5;
    processorOccupation = 0.2;
    ramSize = 8;
    ramOccupation = 0.3;
    

}

Hardware::Hardware(std::string infos){
    size_t pos = 0;
    std::string delimiter = ":";
    std::string token;
    float infosList[5];
    for (size_t i = 0; i < 5; i++)
    {
        pos = infos.find(delimiter);
        token = infos.substr(0, pos); 
        infosList[i] = stof(token);
        infos.erase(0, pos + delimiter.length());
    }
    processorCoreNumber = infosList[0];
    processorFrequency = infosList[1];
    processorOccupation = infosList[2];
    ramSize = infosList[3];
    ramOccupation = infosList[4];
}
Hardware::~Hardware(){}

std::string Hardware::toString(){
    std::string infos = to_string(processorCoreNumber) + ":" + to_string(processorFrequency) + ':' +
                        to_string(processorOccupation) + ':' +
                        to_string(ramSize) + ':' +
                        to_string(ramOccupation);
    return infos;
}

