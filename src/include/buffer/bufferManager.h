#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "frame.h"

class BufferManager {
public:
    static void accessFrame(Frame& frame, char operation, int pinFlag, int currentTime);
    static void resetFrame(Frame& frame);
};

#endif
