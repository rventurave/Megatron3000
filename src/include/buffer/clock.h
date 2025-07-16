#ifndef CLOCK_H
#define CLOCK_H

#include "bufferPool.h"

class Clock : public BufferPool {
    int clockHand;
    int hits;
    int misses;

public:
    Clock(int size);

    bool accessPage(const std::string &pageID, char operation, int pinFlag);
    int frameReplace();
    void despinear(const std::string &pageID);
    void guardado(const std::string &pageID);
    void pinear(const std::string &pageID);
    void printRendimiento() const;
    int indexPageID(const std::string &pageID) const;
    int sizeFrame() const;
    void printFrames() const;
    int getPinCount(int frameID) const;
};

#endif
