#include "../include/buffer/lru.h"
#include "../include/buffer/bufferManager.h"
#include <iostream>
#include <vector>
#include <queue>
#include <climits>

// Objetivo: Inicializa el algoritmo LRU con tamaño fijo
// Input: int size
// Output: Estructura LRU lista para uso
// Autor: Ronald Ventura
LRU::LRU(int size) : BufferPool(size), hits(0), misses(0) {}

// Objetivo: Accede una pagina usando politica LRU
// Input: ID de pagina, operacion, pinFlag
// Output: true si fue hit, false si fue miss
// Autor: Ronald Ventura
bool LRU::accessPage(const std::string &pageID, char operation, int pinFlag)
{
    timeCounter++;

    // HIT
    for (auto &f : frames)
    {
        if (f.pageID == pageID)
        {
            hits++;
            f.opsQueue.push(operation);
            f.count = f.opsQueue.size();

            BufferManager::accessFrame(f, operation, pinFlag, timeCounter);
            return false; // HIT
        }
    }

    // MISS
    misses++;

    // Buscar frame libre
    for (auto &f : frames)
    {
        if (f.pageID == "-")
        {
            f.pageID = pageID;
            f.opsQueue.push(operation);
            f.count = f.opsQueue.size();

            BufferManager::accessFrame(f, operation, pinFlag, timeCounter);
            return true; // MISS
        }
    }

    // Politica de reemplazo
    int idx;
    while (true)
    {
        idx = frameRemplace();

        if (idx >= 0)
        {
            BufferManager::resetFrame(frames[idx]);
            frames[idx].pageID = pageID;
            frames[idx].opsQueue.push(operation);
            frames[idx].count = frames[idx].opsQueue.size();

            BufferManager::accessFrame(frames[idx], operation, pinFlag, timeCounter);
            return true; // MISS
        }
        else if (idx == -2)
        {
            std::cout << "No se puede reemplazar ninguna pagina (todas estan PINNEADAS).\n";
            return false;
        }
    }
}

// Reloj modificado que procesa operaciones incluso si esta pineado, pero solo reemplaza si pin==0 y count==0
int LRU::frameRemplace()
{
    int numFrames = frames.size();

    // Paso 1: encontrar el frame mas antiguo como punto inicial
    int start = -1;
    int minTime = INT_MAX;
    bool todosPineados = true;

    for (int i = 0; i < numFrames; ++i)
    {
        if (frames[i].time < minTime)
        {
            minTime = frames[i].time;
            start = i;
        }

        if (frames[i].pin == 0)
            todosPineados = false;
    }

    if (todosPineados)
        return -2;

    int pos = start;

    while (true)
    {
        Frame &f = frames[pos];

        // Procesar operaciones pendientes, incluso si esta pineado
        if (!f.opsQueue.empty())
        {
            char op = f.opsQueue.front();
            f.opsQueue.pop();

            std::cout << "\nAtendiendo operacion pendiente '" << op
                      << "' en pagina " << f.pageID << ".\n";

            if (op == 'W')
            {
                f.dirty = 1;
                std::cout << "-> Pagina marcada como dirty.\n";
            }
            else
            {
                std::cout << "-> Operacion de lectura procesada.\n";
            }

            // Preguntar si se desea finalizar la operacion
            char respuesta;
            std::cout << "¿Desea terminar el proceso de esta operacion? (s/n): ";
            std::cin >> respuesta;

            if (tolower(respuesta) == 's')
            {
                if (f.count > 0)
                    f.count--;
                std::cout << "-> Se ha descontado el count. Nuevo count = " << f.count << "\n";
            }
            else
            {
                // Devuelve la operacion al frente de la cola
                f.opsQueue.push(op);
                std::cout << "-> La operacion se mantiene pendiente.\n";
            }
        }
        else
        {
            if (f.pin == 0)
            {
                if (f.count == 0)
                {
                    std::cout << "Frame " << f.frameID
                              << " esta libre para reemplazo.\n";
                    return pos;
                }
                else
                {
                    std::cout << "Frame " << f.frameID
                              << " tiene count = " << f.count
                              << ", no se puede reemplazar todavia.\n";

                    // Preguntar si se desea disminuir el count
                    char respuesta;
                    std::cout << "¿Desea reducir el count de este frame? (s/n): ";
                    std::cin >> respuesta;

                    if (tolower(respuesta) == 's')
                    {
                        f.count--;
                        std::cout << "-> Nuevo count = " << f.count << "\n";
                    }
                    else
                    {
                        std::cout << "-> Count se mantiene en " << f.count << "\n";
                    }
                }
            }
            else
            {
                // Frame pineado → no se reemplaza pero se puede disminuir el count
                if (f.count > 0)
                {
                    f.count--;
                    std::cout << "-> Pagina pineada, count reducido a "
                              << f.count << "\n";
                }
            }
        }

        pos = (pos + 1) % numFrames;
    }
}


// Objetivo: Desfijar una pagina especifica
// Input: string pageID
// Output: Cambia estado de pinned
// Autor: Ronald Ventura
void LRU::despinear(const std::string &pageID)
{
    for (auto &f : frames)
    {
        if (f.pageID == pageID)
        {
            f.pin = 0;
            return;
        }
    }
    std::cout << "Pagina " << pageID << " no encontrada.\n";
}

// Objetivo: Marca pagina como guardada
// Input: string pageID
// Output: Modifica el frame asociado
// Autor: Ronald Ventura
void LRU::guardado(const std::string &pageID)
{
    for (auto &f : frames)
    {

        if (f.pageID == pageID)
        {
            f.dirty = 0;
            return;
        }
    }
    std::cout << "Pagina " << pageID << " no encontrada.\n";
}

// Objetivo: Imprimir estadisticas LRU
// Input: Ninguno
// Output: Muestra hits y misses
// Autor: Ronald Ventura
void LRU::printRendimiendo() const
{
    std::cout << "\n\nHit: " << hits;
    std::cout << "\nMiss: " << misses << "\n";

    float hit_rate = (float)hits / (hits + misses);
    float miss_rate = (float)misses / (hits + misses);

    std::cout << "Hit_rate: " << hit_rate << "\n";
    std::cout << "Miss_rate: " << miss_rate << "\n";
}

// Objetivo: Buscar indice de pagina
// Input: string pageID
// Output: indice o -1
// Autor: Ronald Ventura
int LRU::indexPageID(const std::string &pageID) const
{
    for (int i = 0; i < frames.size(); ++i)
    {
        if (frames[i].pageID == pageID)
            return i;
    }
    return -1;
}

// Objetivo: Obtener tamaño del pool
// Input: Ninguno
// Output: Número de frames
// Autor: Ronald Ventura
int LRU::sizeFrame() const
{
    return frames.size();
}

void LRU::pinear(const std::string &pageID)
{
    for (auto &f : frames)
    {
        if (f.pageID == pageID)
        {
            f.pin = 1;
            return;
        }
    }
    std::cout << "Pagina " << pageID << " no encontrada.\n";
}

int LRU::getPinCount(int frameID) const {
    if (frameID >= 0 && frameID < frames.size()) {
        return frames[frameID].pin;
    }
    return 0; // Valor por defecto si esta fuera de rango
}

std::queue<char>& LRU::getOpsQueue(int frameID)
{
    return frames[frameID].opsQueue;
}