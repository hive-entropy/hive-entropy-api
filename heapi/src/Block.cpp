#include "Block.h"

Block::Block(Peer* responsible, int startRow, int endRow, int startCol, int endCol) : responsible(responsible), startCol(startCol), startRow(startRow), endCol(endCol), endRow(endRow){
    timestamp = std::time(nullptr);
}

Block::Block(int startRow, int startCol) : startRow(startRow), startCol(startCol) {
    timestamp = std::time(nullptr);
}

Peer* Block::getResponsible(){
    return responsible;
}

int Block::getStartCol(){
    return startCol;
}

int Block::getStartRow(){
    return startRow;
}

int Block::getEndRow(){
    return endRow;
}

int Block::getEndCol(){
    return endCol;
}

std::time_t Block::getTimestamp(){
    return timestamp;
}

std::string Block::getTaskId(){
    return taskId;
}

void Block::setTaskId(std::string id){
    this->taskId = id;
}

void Block::refresh(){
    timestamp = std::time(nullptr);
}

bool Block::operator==(const Block& other){
    return startCol==other.startCol && startRow==other.startRow;
}