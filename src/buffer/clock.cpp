#include "../include/buffer/clock.h"
#include "../include/buffer/bufferManager.h"
#include <iostream>

Clock::Clock(int size) : BufferPool(size), clockHand(0), hits(0), misses(0) {}

bool Clock::accessPage(const std::string &pageID, char operation, int pinFlag) {
    timeCounter++;

    // HIT
    for (auto &f : frames) {
        if (f.pageID == pageID) {
            hits++;
            f.reference = 1;
            f.opsQueue.push(operation);
            f.count = f.opsQueue.size();

            BufferManager::accessFrame(f, operation, pinFlag, timeCounter);
            return false; // HIT
        }
    }

    // MISS
    misses++;

    // Frame libre
    for (auto &f : frames) {
        if (f.pageID == "-") {
            f.pageID = pageID;
            f.reference = 1;
            f.opsQueue.push(operation);
            f.count = f.opsQueue.size();

            BufferManager::accessFrame(f, operation, pinFlag, timeCounter);
            return true; // MISS
        }
    }

    // CLOCK reemplazo
    int idx;
    while (true) {
        idx = frameReplace();

        if (idx >= 0) {
            BufferManager::resetFrame(frames[idx]);
            frames[idx].pageID = pageID;
            frames[idx].reference = 1;
            frames[idx].opsQueue.push(operation);
            frames[idx].count = frames[idx].opsQueue.size();

            BufferManager::accessFrame(frames[idx], operation, pinFlag, timeCounter);
            return true;
        }
        else if (idx == -2) {
            std::cout << "No se puede reemplazar ninguna pagina (todas estan PINNED).\n";
            return false;
        }
    }
}

int Clock::frameReplace() {
    int size = frames.size();

    // Verificar si todos los frames estan pineados
    bool todosPineados = true;
    for (const auto& f : frames) {
        if (f.pin == 0) {
            todosPineados = false;
            break;
        }
    }
    if (todosPineados)
        return -2;

    // Girar infinitamente hasta encontrar un frame reemplazable
    while (true) {
        Frame& f = frames[clockHand];

        // Procesar operaciones pendientes si existen
        if (!f.opsQueue.empty()) {
            char op = f.opsQueue.front();

            std::cout << "\nFrame " << f.frameID
                      << " tiene operacion pendiente '" << op
                      << "' en pagina " << f.pageID << ".\n";
            std::cout << "Deseas terminar el proceso de esta operacion? (s/n): ";

            char respuesta;
            std::cin >> respuesta;

            if (tolower(respuesta) == 's') {
                f.opsQueue.pop();

                if (op == 'W') {
                    f.dirty = 1;
                } else {
                    std::cout << "-> Operacion de lectura procesada.\n";
                }

                if (f.count > 0) {
                    f.count--;
                }

                if (f.reference == 1 && f.count == 0) {
                    f.reference = 0;
                }

                // Si procesamos algo, no debemos reemplazar en esta vuelta.
                // Avanzamos al siguiente frame.
                clockHand = (clockHand + 1) % size;
                continue;
            } else {
                std::cout << "-> Operacion permanece pendiente.\n";
                clockHand = (clockHand + 1) % size;
                continue;
            }
        }

        if (f.pin == 0) {
            if (f.reference == 1) {
                f.reference = 0;
                // No se reemplaza aun. Avanzamos al siguiente.
                clockHand = (clockHand + 1) % size;
                continue;
            }
            if (f.count > 0) {
                f.count--;
                clockHand = (clockHand + 1) % size;
                continue;
            }
            if (f.count == 0 && f.reference == 0) {
                std::cout << "-> Frame " << f.frameID << " esta libre para reemplazo.\n";
                int idx = clockHand;
                clockHand = (clockHand + 1) % size;
                return idx;
            }
        } else {
            // Frame pineado: solo reducir count y ref si es posible
            if (f.count > 0) {
                f.count--;
            }
            if (f.reference == 1 && f.count == 0) {
                f.reference = 0;
            }
            // Avanzar sin reemplazar
            clockHand = (clockHand + 1) % size;
            continue;
        }

        // Avanzar al siguiente frame
        clockHand = (clockHand + 1) % size;
    }

    return -1; // nunca se alcanza, por seguridad
}

void Clock::despinear(const std::string &pageID) {
    for (auto &f : frames) {
        if (f.pageID == pageID) {
            f.pin = 0;
            return;
        }
    }
    std::cout << "Pagina " << pageID << " no encontrada.\n";
}

void Clock::guardado(const std::string &pageID) {
    for (auto &f : frames) {
        if (f.pageID == pageID) {
            f.dirty = 0;
            return;
        }
    }
    std::cout << "Pagina " << pageID << " no encontrada.\n";
}

void Clock::printRendimiento() const {
    std::cout << "\n\nHit: " << hits;
    std::cout << "\nMiss: " << misses << "\n";

    float hit_rate = (float)hits / (hits + misses);
    float miss_rate = (float)misses / (hits + misses);

    std::cout << "Hit_rate: " << hit_rate << "\n";
    std::cout << "Miss_rate: " << miss_rate << "\n";
}

int Clock::indexPageID(const std::string &pageID) const {
    for (int i = 0; i < frames.size(); ++i) {
        if (frames[i].pageID == pageID)
            return i;
    }
    return -1;
}

int Clock::sizeFrame() const {
    return frames.size();
}
void Clock::printFrames() const
{
    std::cout << "\nFRAME\tPAGE ID\tL/W\tDIRTY\tPIN\tCOUNT\tREF\tCLOCK\tOPS\n";
    std::cout << std::string(100, '-') << "\n";

    for (int i = 0; i < frames.size(); ++i)
    {
        const Frame &f = frames[i];

        std::string ops_str = "-";
        char display_op = f.operation;
        int display_dirty = f.dirty;

        if (!f.opsQueue.empty())
        {
            display_op = f.opsQueue.front();
            display_dirty = (display_op == 'W') ? 1 : 0;

            ops_str.clear();
            std::queue<char> temp_q = f.opsQueue;
            while (!temp_q.empty())
            {
                ops_str += temp_q.front();
                ops_str += ",";
                temp_q.pop();
            }
            if (!ops_str.empty())
                ops_str.pop_back(); // quita la última coma
        }

        std::cout << f.frameID << "\t"
                  << (f.pageID == "-" ? "-" : f.pageID) << "\t"
                  << (f.pageID == "-" ? "-" : std::string(1, display_op)) << "\t"
                  << display_dirty << "\t"
                  << f.pin << "\t"
                  << f.count << "\t"
                  << f.reference << "\t"
                  << (i == clockHand ? "1" : "0") << "\t"
                  << ops_str << "\n";
    }

    std::cout << std::string(100, '-') << "\n";
}


void Clock::pinear(const std::string &pageID) {
    for (auto &f : frames) {
        if (f.pageID == pageID) {
            f.pin = 1;
            return;
        }
    }
    std::cout << "Pagina " << pageID << " no encontrada.\n";
}
int Clock::getPinCount(int frameID) const {
    if (frameID >= 0 && frameID < frames.size()) {
        return frames[frameID].pin;
    }
    return 0;
}
