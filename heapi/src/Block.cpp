#include "Block.h"
#include "Peer.h"

Block::Block(Peer const &_responsible, int const &_startRow, int const &_endRow, int const &_startCol, int const &_endCol) : startCol(_startCol), startRow(_startRow), endCol(_endCol), endRow(_endRow){
    responsible = std::make_shared<Peer>(_responsible);
    timestamp = std::chrono::steady_clock::now();
}

Block::Block(int const &startRow, int const &startCol) : startRow(startRow), startCol(startCol) {
    timestamp = std::chrono::steady_clock::now();
}

std::shared_ptr<Peer> Block::getResponsible() const {
    return responsible;
}

int Block::getStartCol() const {
    return startCol;
}

int Block::getStartRow() const {
    return startRow;
}

int Block::getEndRow() const {
    return endRow;
}

int Block::getEndCol() const {
    return endCol;
}

std::chrono::steady_clock::time_point Block::getTimestamp() const {
    return timestamp;
}

std::string Block::getTaskId() const {
    return taskId;
}

void Block::setTaskId(std::string const &id) {
    this->taskId = id;
}

void Block::refresh(){
    timestamp = std::chrono::steady_clock::now();
}

bool Block::operator==(Block const &other) const {
    return startCol==other.startCol && startRow==other.startRow;
}
