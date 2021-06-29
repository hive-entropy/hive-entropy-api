#ifndef  BLOCK_H
#define BLOCK_H

#include <chrono>
#include <string>
#include <memory>

class Peer;
class Block{
    private:
        /**
         * @brief The communicating node responsible for the computation of this block.
         */
        std::shared_ptr<Peer> responsible;

        /**
         * @brief The time at which the block was created, used to check if it expired.
         */
        std::chrono::steady_clock::time_point timestamp;

        /**
         * @brief The starting row in the broader result of the block.
         */
        int startRow{};

        /**
         * @brief The end row in the broader result of the block.
         * 
         */
        int endRow{};

        /**
         * @brief The starting column in the broader result of the block.
         * 
         */
        int startCol{};

        /**
         * @brief The end column in the broader result of the block.
         * 
         */
        int endCol{};

        /**
         * @brief The task identifier for the block, used only in Cannon.
         * 
         */
        std::string taskId;

    public:
        /**
         * @brief Default constructor for Block.
         * 
         */
        Block() = default;

        /**
         * @brief Copy constructor for Block.
         * 
         * @param block 
         */
        Block(Block const &block) = default;

        /**
         * @brief Constructs a block object using its responsible and bound coordinates in the broader result.
         * 
         * @param _responsible The communicating node responsible for the computation of this block.
         * @param _startRow The starting row in the broader result of the block.
         * @param _endRow The end row in the broader result of the block.
         * @param _startCol The starting column in the broader result of the block.
         * @param _endCol The end column in the broader result of the block.
         */
        Block(Peer const &_responsible, int const &_startRow, int const &_endRow, int const &_startCol, int const &_endCol);

        /**
         * @brief Construct a block object using only the starting coordinates in the broader result.
         * 
         * Usually utilized on reception of a message containing insertion coordinates, to compare them to an existing block with more details.
         * 
         * @param startRow The starting row in the broader result of the block.
         * @param startCol The starting column in the broader result of the block.
         */
        Block(int const &startRow, int const &startCol);

        /**
         * @brief Default destructor for the block object.
         * 
         */
        ~Block() = default;

        /**
         * @brief Get the Responsible object
         * 
         * @return std::shared_ptr<Peer> a pointer to the responsible peer of a block.
         */
        std::shared_ptr<Peer> getResponsible() const;

        /**
         * @brief Get the Start Col object
         * 
         * @return int The starting column in the broader result of the block.
         */
        int getStartCol() const;

        /**
         * @brief Get the Start Row object
         * 
         * @return int The starting row in the broader result of the block.
         */
        int getStartRow() const;

        /**
         * @brief Get the End Row object
         * 
         * @return int The end row in the broader result of the block.
         */
        int getEndRow() const;

        /**
         * @brief Get the End Col object
         * 
         * @return int The end column in the broader result of the block.
         */
        int getEndCol() const;

        /**
         * @brief Get the Timestamp object
         * 
         * @return std::chrono::steady_clock::time_point The time at which the block was created, used to check if it expired.
         */
        std::chrono::steady_clock::time_point getTimestamp() const;

        /**
         * @brief Get the Task Id object
         * 
         * @return std::string The task identifier for the block.
         */
        std::string getTaskId() const;

        /**
         * @brief Set the Task Id object
         * 
         * @param id The task identifier to set for the block.
         */
        void setTaskId(std::string const &id);

        /**
         * @brief Compares two blocks based on their starting coordinates in the broader result.
         * 
         * Specifically used to associate a result packet with the associated pending blocks using insertion points.
         * 
         * @param other The block to which the original block is compared.
         * @return true when both the starting coordinates of the blocks are equal.
         * @return false otherwise.
         */
        bool operator==(Block const &other) const;

        /**
         * @brief Refreshes the timestamp of the block to the time of calling.
         */
        void refresh();
};

#endif