#ifndef HARDWARE_H
#define HARDWARE_H

#include <iostream>

/**
 * @brief Contains the hardware specification of a node.
 */
class Hardware
{
    private:
        /**
         * @brief Size of node's RAM
         */
        float ramSize;

        /**
         * @brief Occupation rate of node's RAM between 0 and 100
         */
        float ramOccupation;

        /**
         * @brief Precessor's frequency in MHz
         */
        float processorFrequency;

        /**
         * @brief Number of logical core
         */
        float processorCoreNumber;

        /**
         * @brief Occupation rate of CPU between 0 and 100
         */
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

        /**
         * @return Return RAM size
         */
        float findRamSize();

        /**
         * @return Return RAM occupation rate
         */
        float findRamOccupation() const;

        /**
         * @return Return CPU's frequency
         */
        float findProcessorFrequency();

        /**
         * @return Return number of logical CPU core
         */
        float findProcessorCoreNumber();

        /**
         * @return Return CUP Occupation rate
         */
        float findProcessorOccupation();
};
#endif