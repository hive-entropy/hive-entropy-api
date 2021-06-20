#ifndef  BLOCK_H
#define BLOCK_H

#include <chrono>
#include <string>
#include <memory>

class Peer;
class Block{
    private:
        std::shared_ptr<Peer> responsible;
        std::chrono::steady_clock::time_point timestamp;

        int startRow{};
        int endRow{};
        int startCol{};
        int endCol{};

        std::string taskId;

    public:
        // Constructors
        Block() = default;
        Block(Block const &block) = default;
        Block(Peer const &_responsible, int const &_startRow, int const &_endRow, int const &_startCol, int const &_endCol);
        Block(int const &startRow, int const &startCol);
        ~Block() = default;

        // Getters
        std::shared_ptr<Peer> getResponsible() const;
        int getStartCol() const;
        int getStartRow() const;
        int getEndRow() const;
        int getEndCol() const;
        std::chrono::steady_clock::time_point getTimestamp() const;
        std::string getTaskId() const;

        // Setters
        void setTaskId(std::string const &id);

        // Operators
        bool operator==(Block const &other) const;

        // Methods
        void refresh();
};

#endif