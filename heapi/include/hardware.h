#ifndef HARDWARE_H
#define HARDWARE_H

#include <iostream>

class Hardware
{
    private:
        

    public:
        Hardware();
        Hardware(std::string infos);
        ~Hardware();

        float processorCoreNumber;
        float processorFrequency;
        float processorOccupation;
        float ramSize;
        float ramOccupation;

        float getProcessorCoreNumber();
        float getProcessorFrequency();
        float getprocessorOccupation();
        float getramSize();
        float getRamOccupation();

        std::string toString();

    


};
#endif