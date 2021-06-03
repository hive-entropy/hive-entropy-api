#ifndef HARDWARE_H
#define HARDWARE_H

#include <iostream>

class Hardware
{
    private:
        float processorCoreNumber;
        float processorFrequency;
        float processorOccupation;
        float ramSize;
        float ramOccupation;

    public:
        Hardware();
        Hardware(std::string infos);
        Hardware(const Hardware& other) = default;
        ~Hardware();

        float findProcessorCoreNumber();
        float findProcessorFrequency();
        float findProcessorOccupation();
        float findRamSize();
        float findRamOccupation();

        float getProcessorCoreNumber();
        float getProcessorFrequency();
        float getProcessorOccupation();
        float getRamSize();
        float getRamOccupation();

        std::string toString();
};
#endif