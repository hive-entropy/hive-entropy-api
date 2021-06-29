#include "Hardware.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>

void execUnixCMD(const char *cmd, char *result) {
    FILE *fp = nullptr;
    char str[sizeof(float) + 1] = {0};

    fp = popen(cmd, "r");
    if (fp == nullptr) {
        printf("Failed to run command\n");
    }

    /* Read the output a line at a time - output it. */
    while (fgets(result, sizeof(float) * sizeof(float), fp) != nullptr) {
        std::cout << " fgets [" << str << "]" << std::endl;
    }
    std::cout << "for : " << cmd << " result : " << result << std::endl;
    /* close */
    pclose(fp);
}

Hardware::Hardware() {
    processorCoreNumber = findProcessorCoreNumber();
    processorFrequency = findProcessorFrequency();
    processorOccupation = findProcessorOccupation();
    ramSize = findRamSize();
    ramOccupation = findRamOccupation();
}

Hardware::Hardware(std::string infos) {
    size_t pos = 0;
    std::string delimiter = ":";
    std::string token;
    float infosList[5];
    for (float & i : infosList) {
        pos = infos.find(delimiter);
        token = infos.substr(0, pos);
        i = std::stof(token);
        infos.erase(0, pos + delimiter.length());
    }
    processorCoreNumber = infosList[0];
    processorFrequency = infosList[1];
    processorOccupation = infosList[2];
    ramSize = infosList[3];
    ramOccupation = infosList[4];
}

std::string Hardware::toString() const {
    std::string infos = std::to_string(processorCoreNumber) + ":" + std::to_string(processorFrequency) + ':' +
                        std::to_string(processorOccupation) + ':' +
                        std::to_string(ramSize) + ':' +
                        std::to_string(ramOccupation);
    return infos;
}

float Hardware::findProcessorCoreNumber() {
    char socket[sizeof(float)] = {0}, core[sizeof(float)] = {0};

    execUnixCMD("nproc | tr -d '\n'", core);
    /* Open the command for reading. */


    float numberOfCore = std::stof(core);
    return numberOfCore;
}

float Hardware::findProcessorFrequency() {
    char numberOfLine[2] = {0}, frequency[10000] = {0};
    execUnixCMD("lscpu | awk '$0 ~ /MHz/ {count ++} END {print count}' | tr -d '\n'", numberOfLine);
    std::cout << "number of line=" << numberOfLine << std::endl;

    if (std::strcmp(numberOfLine, "3") == 0) {
        execUnixCMD("lscpu | awk '$0 ~ /MHz/ {print $NF } ' | sed -n '2p'", frequency);
    } else if (std::strcmp(numberOfLine, "2") == 0) {
        execUnixCMD("lscpu | grep MHz | sed -n '1p' | cut -d':' -f2 | tr -d ' '", frequency);
    } else if (std::strcmp(numberOfLine, "1") == 0) {
        execUnixCMD("lscpu | awk '$0 ~ /MHz/ {print $NF } ' | sed -n '1p'", frequency);
    } else {
        std::cout << "wrong number of line=" << numberOfLine << std::endl;
    }
    std::cout << "frequency" << frequency << std::endl;
    float cpuFrequency = std::stof(frequency);
    return cpuFrequency;
}

float Hardware::findProcessorOccupation() {
    char occupation[10] = {0};
    execUnixCMD("iostat | sed -n '4p' | awk '{print $6}'", occupation);
    std::cout << "ProcessorOccupation=" << occupation << std::endl;
    std::string occupationRate_str = occupation;
    replace(occupationRate_str.begin(), occupationRate_str.end(), ',', '.');
    float cpuOccupation = 100 - stof(occupationRate_str);
    return cpuOccupation;
}

float Hardware::findRamSize() {
    char ram[10] = {0};
    execUnixCMD("free -m | grep Mem | awk '{print $2}'", ram);
    std::cout << "ram=" << ram;
    float ramSize = std::stof(ram);
    return ramSize;
}

float Hardware::findRamOccupation() const {
    char usedRam[10] = {0};
    execUnixCMD("free -m | grep Mem | awk '{print $4}'", usedRam);
    std::cout << "usedRam=" << usedRam;
    return std::stof(usedRam) * 100 / ramSize;
}

float Hardware::getProcessorCoreNumber() const {
    return processorCoreNumber;
}

float Hardware::getProcessorFrequency() const {
    return processorFrequency;
}

float Hardware::getProcessorOccupation() const {
    return processorOccupation;
}

float Hardware::getRamSize() const {
    return ramSize;
}

float Hardware::getRamOccupation() const {
    return ramOccupation;
}

int Hardware::getRate() const {
    return (ramSize * ramOccupation / 100 + processorCoreNumber * processorOccupation / 100) * 10 / 16;
}

