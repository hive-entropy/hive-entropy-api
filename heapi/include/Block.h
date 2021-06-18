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

        int startRow;
        int startCol;
        int endCol;
        int endRow;

        std::string taskId;

    public:
        // Constructors
        Block() = default;
        Block(Block const &block) = default;
        Block(Peer const &_responsible, int const &_startRow, int const &_endRow, int const &_startCol, int const &_endCol);
        Block(int const &startRow, int const &startCol);
        ~Block() = default;

        // Getters
        [[nodiscard]] std::shared_ptr<Peer> getResponsible() const;
        [[nodiscard]] int getStartCol() const;
        [[nodiscard]] int getStartRow() const;
        [[nodiscard]] int getEndRow() const;
        [[nodiscard]] int getEndCol() const;
        [[nodiscard]] std::chrono::steady_clock::time_point getTimestamp() const;
        [[nodiscard]] std::string getTaskId() const;

        // Setters
        void setTaskId(std::string const &id);

        // Operations
        bool operator==(Block const &other) const;
        void refresh();
};

#endif