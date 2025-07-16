#ifndef BUFFER_POOL_H
#define BUFFER_POOL_H

#include "frame.h"
#include <vector>

class BufferPool {
protected:
    std::vector<Frame> frames;
    int timeCounter;

public:
    BufferPool(int size);
    bool allPinned() const;
    void printFrames() const;
};

#endif
