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

        int processorCoreNumber;
        int processorFrequency;
        int processorOccupation;
        int ramSize;
        int ramOccupation;

        int getProcessorCoreNumber(){ return processorCoreNumber; }
        int getProcessorFrequency(){ return processorFrequency;}
        int getprocessorOccupation(){ return processorOccupation;}
        int getramSize(){ return ramSize;}
        int getramOccupation(){ return ramOccupation;}

        std::string toString();

    


};
#endif