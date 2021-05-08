#include "DataSplitter.h"
#include "stdio.h"

DataSplitter::DataSplitter(/* args */)
{
}

DataSplitter::DataSplitter(HiveEntropyNode* reqNode)
{
    requestNode = reqNode;
    
}

DataSplitter::~DataSplitter()
{
}

void DataSplitter::fillNodeList(){

    printf("FillNondeList\n");
    //requestNode->queryNodeAvailability();

    printf("end function fillnodelist\n");

}