#include "Block.h"

Block::Block(Peer* responsible, int startRow, int endRow, int startCol, int endCol) : responsible(responsible), startCol(startCol), startRow(startRow), endCol(endCol), endRow(endRow){
    timestamp = std::chrono::steady_clock::now();
}

Block::Block(int startRow, int startCol) : startRow(startRow), startCol(startCol) {
    timestamp = std::chrono::steady_clock::now();
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

std::chrono::steady_clock::time_point Block::getTimestamp(){
    return timestamp;
}

std::string Block::getTaskId(){
    return taskId;
}

void Block::setTaskId(std::string id){
    this->taskId = id;
}

void Block::refresh(){
    timestamp = std::chrono::steady_clock::now();
}

bool Block::operator==(const Block& other){
    return startCol==other.startCol && startRow==other.startRow;
}