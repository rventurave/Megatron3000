#ifndef LRU_H
#define LRU_H
#include <iostream>
#include <climits>
#include <queue>
#include "bufferPool.h"

class LRU : public BufferPool
{
    int hits;
    int misses;

public:
    LRU(int size);
    //void accessPage(const std::string &pageID, char operation, int pinFlag);
    bool accessPage(const std::string &pageID, char operation, int pinFlag);

    void despinear(const std::string &pageID);
    void pinear(const std::string &pageID);
    int frameRemplace();
    void printRendimiendo() const;
    void guardado(const std::string &pageID);
    int indexPageID(const std::string &pageID) const;
    int sizeFrame() const;
    int getPinCount(int frameID) const ;
    std::queue<char>& getOpsQueue(int frameID);
};

#endif
