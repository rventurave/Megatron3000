#include "../include/buffer/bufferPool.h"
#include <iostream>

// Objetivo: Constructor del buffer pool con tamaño fijo
// Input: int size
// Output: Pool inicializado con N frames
// Autor: Ronald Ventura
BufferPool::BufferPool(int size) : timeCounter(0) {
    for (int i = 0; i < size; ++i)
        frames.emplace_back(i + 1);
}


// Objetivo: Verifica si todos los frames están fijados
// Input: Ninguno
// Output: true si todos están pinned
// Autor: Ronald Ventura
bool BufferPool::allPinned() const {
    for (const auto& f : frames)
        if (f.pin == 0) return false;
    return true;
}


// Objetivo: Imprime el estado actual del buffer
// Input: Ninguno
// Output: Información por del frame por consola
// Autor: Ronald Ventura
void BufferPool::printFrames() const {
    std::cout << "\n\nFRAME\tPAGE ID\tL/W\tDIRTY\tTIME\tPIN\tCOUNT\tOPS\n";
    std::cout << "-------------------------------------------------------------\n";

    for (const auto& f : frames) {
        // Determinar la próxima operación pendiente
        char nextOp = '-';
        int dirtyFlag = 0;

        if (!f.opsQueue.empty()) {
            nextOp = f.opsQueue.front();

            // Para mostrar en la tabla si dirty estará pendiente
            if (nextOp == 'W')
                dirtyFlag = 1;
            else
                dirtyFlag = 0;
        }

        // Imprimir cola completa
        std::queue<char> tempQueue = f.opsQueue;
        std::string opsStr;
        while (!tempQueue.empty()) {
            opsStr += tempQueue.front();
            tempQueue.pop();
            if (!tempQueue.empty()) opsStr += ",";
        }

        std::cout << f.frameID << "\t"
                  << f.pageID << "\t"
                  << nextOp << "\t"
                  << dirtyFlag << "\t"
                  << f.time << "\t"
                  << f.pin << "\t"
                  << f.count << "\t"
                  << (opsStr.empty() ? "-" : opsStr) << "\n";
    }

    std::cout << std::endl;
}