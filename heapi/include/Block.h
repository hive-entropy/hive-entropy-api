#ifndef  BLOCK_H
#define BLOCK_H

#include <ctime>

#include "Peer.h"

class Block{
    private:
        Peer* responsible;
        std::time_t timestamp;

        int startCol;
        int startRow;
        int endCol;
        int endRow;

        std::string taskId;
    public:
        Block(Peer* responsible, int startRow, int endRow, int startCol, int endCol);
        Block(int startRow, int startCol);
        ~Block() = default;

        Peer* getResponsible();
        int getStartCol();
        int getStartRow();
        int getEndRow();
        int getEndCol();
        std::time_t getTimestamp();
        std::string getTaskId();

        void setTaskId(std::string id);

        void refresh();

        bool operator==(const Block& other);
};

#endif