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

        float getProcessorCoreNumber(){ return processorCoreNumber; }
        float getProcessorFrequency(){ return processorFrequency;}
        float getprocessorOccupation(){ return processorOccupation;}
        float getramSize(){ return ramSize;}
        float getramOccupation(){ return ramOccupation;}

        std::string toString();

    


};
#endif