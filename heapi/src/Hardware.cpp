#include "hardware.h"
#include <sys/sysinfo.h>
#include <sstream>
#include <iostream>
#include <string>  
#include <algorithm>
using namespace std;

#define UNIX
#ifdef UNIX
int getProcNumber = get_nprocs();
#endif

Hardware::Hardware(){
    processorCoreNumber = getProcessorCoreNumber();
    //processorCoreNumber = 5;
    processorFrequency = getProcessorFrequency();
    //cout << endl << processorFrequency << endl;
    //processorOccupation = 0.2;
    processorOccupation = getprocessorOccupation();
    ramSize = getramSize();
    ramOccupation = getRamOccupation();
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
void execUnixCMD(const char* cmd, char * result){
    FILE *fp;
    fp = popen(cmd, "r");
    if (fp == NULL) {
    printf("Failed to run command\n" );
    }

    /* Read the output a line at a time - output it. */
    while (fgets(result, sizeof(result), fp) != NULL) {
    }
    /* close */
    pclose(fp);


}
float Hardware::getProcessorCoreNumber(){
    
    char socket[sizeof(float)], core[sizeof(float)];

    execUnixCMD("lscpu | awk '/socket/{print $4}'", socket);
    execUnixCMD("nproc", core);
    /* Open the command for reading. */
    

    float numberOfSocket = stof(socket);
    float numberOfCore = stof(core);
     return numberOfSocket * numberOfCore;
}

float Hardware::getProcessorFrequency(){

    char frequency[10];

    execUnixCMD("lscpu | awk -F '@' '/@ /{print $2}' | cut -c2-5", frequency);
    
    float cpuFrequency = stof(frequency);
     return cpuFrequency;
}
float Hardware::getprocessorOccupation(){

    char occupation[10];
    execUnixCMD("iostat | grep -P '^\\s*\\d' | awk '{print $6} '", occupation);
    string occupationRate_str = occupation;
    replace(occupationRate_str.begin(), occupationRate_str.end(), ',', '.');
    float cpuOccupation =100 - stof(occupationRate_str);
    return cpuOccupation;
}

float Hardware::getramSize(){
    char ram[10];
    execUnixCMD("free -m | grep Mem | awk '{print $2}'", ram);
    float ramSize = stof(ram);
    return ramSize;
}

float Hardware::getRamOccupation(){
    char usedRam[10];
    execUnixCMD("free -m | grep Mem | awk '{print $4}'", usedRam);
    float ramOccupation = stof(usedRam) *100 / ramSize;
    return ramOccupation;
}
