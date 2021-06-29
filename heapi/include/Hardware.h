#ifndef HARDWARE_H
#define HARDWARE_H

#include <iostream>

class Hardware
{
    private:
        float ramSize;
        float ramOccupation;
        float processorFrequency;
        float processorCoreNumber;
        float processorOccupation;

    public:
        // Constructors
        Hardware();
        explicit Hardware(std::string infos);
        Hardware(const Hardware& other) = default;
        ~Hardware() = default;

        // Getters
        int getRate() const;
        float getRamSize() const;
        float getRamOccupation() const;
        float getProcessorFrequency() const;
        float getProcessorCoreNumber() const;
        float getProcessorOccupation() const;

        // Output
        std::string toString() const;

        // Methods
        float findRamSize();
        float findRamOccupation() const;
        float findProcessorFrequency();
        float findProcessorCoreNumber();
        float findProcessorOccupation();
};
#endif