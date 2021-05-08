#ifndef DATA_SPLITTER_H
#define DATA_SPLITTER_H

#include <list>
#include <iostream>
#include "HiveEntropyNode.h"
#include "Matrix.h" 
using namespace std;

class DataSplitter
{
private:
    /* data */
    HiveEntropyNode* nodeList = NULL;
    HiveEntropyNode* requestNode = NULL;
    

public:
    DataSplitter(/* args */);
    DataSplitter(HiveEntropyNode* requestNode);
    ~DataSplitter();

    void fillNodeList();
};



#endif