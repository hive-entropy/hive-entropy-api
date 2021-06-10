#include "Hardware.h"
#include <sys/sysinfo.h>
#include <sstream>
#include <iostream>
#include <string>  
#include <string.h>
#include <algorithm>
using namespace std;


void execUnixCMD(const char* cmd, char * result){
    FILE *fp;
    fp = popen(cmd, "r");
    if (fp == NULL) {
    printf("Failed to run command\n" );
    }

    /* Read the output a line at a time - output it. */
    while (fgets(result, sizeof(float)*sizeof(char), fp) != NULL) {
        cout << "get : " << result << endl;
    }
    std::cout << "for : " << cmd << " result : " << result << endl;
    /* close */
    pclose(fp);
}

Hardware::Hardware(){
    processorCoreNumber = findProcessorCoreNumber();
    processorFrequency = findProcessorFrequency();
    processorOccupation = findProcessorOccupation();
    ramSize = findRamSize();
    ramOccupation = findRamOccupation();
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
        infosList[i] = std::stof(token);
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

float Hardware::findProcessorCoreNumber(){
    
    char socket[sizeof(float)], core[sizeof(float)];

    execUnixCMD("lscpu | awk '/socket/{print $4}'", socket);
    execUnixCMD("nproc", core);
    /* Open the command for reading. */
    

    float numberOfSocket = stof(socket);
    float numberOfCore = stof(core);
     return numberOfSocket * numberOfCore;
}

float Hardware::findProcessorFrequency(){

    char numberOfLine[2], frequency[10000];
    execUnixCMD("lscpu | awk '$0 ~ /MHz/ {count ++} END {print count}' | tr -d '\n'", numberOfLine);
    cout << "number of line=" << numberOfLine << endl;
    
    if (strcmp(numberOfLine, "3") == 0)
    {
        execUnixCMD("lscpu | awk '$0 ~ /MHz/ {print $NF } ' | sed -n '2p'", frequency);
    }
    else if (strcmp(numberOfLine, "2") == 0)
    {
        execUnixCMD("lscpu | grep MHz | sed -n '1p' | cut -d':' -f2 | tr -d ' '", frequency);
    }
    else if (strcmp(numberOfLine, "1") == 0)
    {
        execUnixCMD("lscpu | awk '$0 ~ /MHz/ {print $NF } ' | sed -n '1p'", frequency);
    }else{
        cout << "wrong number of line=" << numberOfLine << endl;
    }
    cout << "frequency" << frequency << endl;
    float cpuFrequency = stof(frequency);
     return cpuFrequency;
}

float Hardware::findProcessorOccupation(){

    char occupation[10];
    execUnixCMD("iostat | sed -n '4p' | awk '{print $6}'", occupation);
    cout << "ProcessorOccupation=" << occupation << endl;
    string occupationRate_str = occupation;
    replace(occupationRate_str.begin(), occupationRate_str.end(), ',', '.');
    float cpuOccupation = 100 - stof(occupationRate_str);
    return cpuOccupation;
}

float Hardware::findRamSize(){
    char ram[10];
    execUnixCMD("free -m | grep Mem | awk '{print $2}'", ram);
    cout << "ram=" << ram;
    float ramSize = stof(ram);
    return ramSize;
}

float Hardware::findRamOccupation(){
    char usedRam[10];
    execUnixCMD("free -m | grep Mem | awk '{print $4}'", usedRam);
    cout << "usedRam=" << usedRam;
    float ramOccupation = stof(usedRam) *100 / ramSize;
    return ramOccupation;
}

float Hardware::getProcessorCoreNumber(){
    return processorCoreNumber;
}

float Hardware::getProcessorFrequency(){
    return processorFrequency;
}

float Hardware::getProcessorOccupation(){
    return processorOccupation;
}

float Hardware::getRamSize(){
    return ramSize;
}

float Hardware::getRamOccupation(){
    return ramOccupation;
}

int Hardware::getRate(){
    return (ramSize * ramOccupation/100 +  processorCoreNumber * processorOccupation/100) * 10 / 16;
}

