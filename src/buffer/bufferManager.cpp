#include "../include/buffer/bufferManager.h"
// Autor: Ronald Ventura


// Objetivo: Accede a un frame según operacion y pin
// Input: Frame&, operacion ('L' o 'W'), pinFlag, tiempo
// Output: Modificacion del frame
// Autor: Ronald Ventura
void BufferManager::accessFrame(Frame& frame, char operation, int pinFlag, int currentTime) {
    frame.operation = operation;
    frame.pin = pinFlag;
    frame.time = currentTime;
}

// Objetivo: Reinicia los valores de un frame
// Input: Frame&
// Output: Frame en estado limpio
// Autor: Ronald Ventura
void BufferManager::resetFrame(Frame& frame) {
    frame.pageID = "-";
    frame.dirty = 0;
    frame.pin = 0;
    frame.count = 0;
    frame.time = 0;
}
