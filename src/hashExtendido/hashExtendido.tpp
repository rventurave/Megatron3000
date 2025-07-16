#ifndef HASH_EXTENDIDO_TPP
#define HASH_EXTENDIDO_TPP

#include "../include/hashExtendido/hashExtendido.h"

// --- Implementación Bucket ---
template <typename T>
Bucket<T>::Bucket(int cap, int prof) : capacidad(cap), profundidadLocal(prof) {}

template <typename T>
bool Bucket<T>::lleno() const {
    return elementos.size() >= capacidad;
}

template <typename T>
bool Bucket<T>::agregar(const T& valor) {
    if (!lleno()) {
        elementos.push_back(valor);
        return true;
    }
    return false;
}

template <typename T>
bool Bucket<T>::contiene(const T& valor) const {
    for (const auto& e : elementos) {
        if (e == valor) return true;
    }
    return false;
}

template <typename T>
bool Bucket<T>::eliminar(const T& valor) {
    for (size_t i = 0; i < elementos.size(); ++i) {
        if (elementos[i] == valor) {
            elementos.erase(elementos.begin() + i);
            return true;
        }
    }
    return false;
}

template <typename T>
void Bucket<T>::mostrar() const {
    std::cout << "Bucket(prof=" << profundidadLocal << ", elementos={";
    for (size_t i = 0; i < elementos.size(); ++i) {
        std::cout << elementos[i];
        if (i + 1 < elementos.size()) std::cout << ", ";
    }
    std::cout << "})";
}

// --- Implementación HashExtendido ---
template <typename T>
HashExtendido<T>::HashExtendido(int cap) : capacidadBucket(cap), profundidadGlobal(1) {
    directorio.resize(1 << profundidadGlobal);
    for (int i = 0; i < (1 << profundidadGlobal); ++i)
        directorio[i] = std::make_shared<Bucket<T>>(capacidadBucket, profundidadGlobal);
}

template <typename T>
size_t HashExtendido<T>::obtenerHash(const T& valor) const {
    return std::hash<T>{}(valor);
}

template <typename T>
int HashExtendido<T>::obtenerIndice(size_t hash) const {
    return hash & ((1 << profundidadGlobal) - 1);
}

template <typename T>
void HashExtendido<T>::dividirBucket(int indice) {
    auto viejo = directorio[indice];
    int profAnt = viejo->profundidadLocal;
    viejo->profundidadLocal++;

    auto b0 = std::make_shared<Bucket<T>>(capacidadBucket, viejo->profundidadLocal);
    auto b1 = std::make_shared<Bucket<T>>(capacidadBucket, viejo->profundidadLocal);

    for (const auto& val : viejo->elementos) {
        int bit = (obtenerHash(val) >> profAnt) & 1;
        if (bit == 0)
            b0->agregar(val);
        else
            b1->agregar(val);
    }

    viejo->elementos.clear();

    for (int i = 0; i < directorio.size(); ++i) {
        if (directorio[i] == viejo) {
            int bit = (i >> profAnt) & 1;
            directorio[i] = (bit == 0) ? b0 : b1;
        }
    }
}

template <typename T>
void HashExtendido<T>::fusionarBuckets(int indice) {
    auto bucketActual = directorio[indice];

    if (bucketActual->elementos.empty() && bucketActual->profundidadLocal > 1) {
        int profLocal = bucketActual->profundidadLocal;
        int buddyIndex = indice ^ (1 << (profLocal - 1));
        auto buddyBucket = directorio[buddyIndex];

        if (buddyBucket && buddyBucket->profundidadLocal == profLocal) {
            int commonPrefixLength = profLocal - 1;
            int commonPrefix = indice & ((1 << commonPrefixLength) - 1);

            for (int i = 0; i < (1 << (profundidadGlobal - commonPrefixLength)); ++i) {
                int dirIndex = (commonPrefix | (i << commonPrefixLength));
                if (directorio[dirIndex] == bucketActual || directorio[dirIndex] == buddyBucket) {
                    directorio[dirIndex] = buddyBucket;
                }
            }

            buddyBucket->profundidadLocal = commonPrefixLength;
        }
    }
}

template <typename T>
void HashExtendido<T>::insertar(const T& valor) {
    while (true) {
        size_t h = obtenerHash(valor);
        int indice = obtenerIndice(h);
        auto bucket = directorio[indice];

        if (bucket->contiene(valor)) return;

        if (bucket->lleno()) {
            if (bucket->profundidadLocal == profundidadGlobal) {
                int tam = directorio.size();
                profundidadGlobal++;
                directorio.resize(1 << profundidadGlobal);
                for (int i = 0; i < tam; ++i)
                    directorio[i + tam] = directorio[i];
            }
            dividirBucket(indice);
        } else {
            bucket->agregar(valor);
            break;
        }
    }
}

template <typename T>
bool HashExtendido<T>::buscar(const T& valor) const {
    size_t h = obtenerHash(valor);
    int indice = obtenerIndice(h);
    return directorio[indice]->contiene(valor);
}

template <typename T>
bool HashExtendido<T>::eliminar(const T& valor) {
    size_t h = obtenerHash(valor);
    int indice = obtenerIndice(h);
    auto bucket = directorio[indice];

    if (bucket->eliminar(valor)) {
        if (bucket->elementos.empty()) {
            fusionarBuckets(indice);
        }
        return true;
    } else {
        return false;
    }
}

template <typename T>
void HashExtendido<T>::mostrar() const {
    std::cout << "\n--- Estado del Hash ---\n";
    std::cout << "Profundidad Global: " << profundidadGlobal << "\n";
    std::cout << "Tamano del Directorio: " << directorio.size() << "\n";

    std::map<void*, std::shared_ptr<Bucket<T>>> unicos;
    for (int i = 0; i < directorio.size(); ++i) {
        std::cout << "   [" << std::setw(3) << std::bitset<8>(i).to_string().substr(8 - profundidadGlobal) << "] -> ";
        directorio[i]->mostrar();
        std::cout << "\n";
        unicos[directorio[i].get()] = directorio[i];
    }

    std::cout << "\nBuckets Unicos:\n";
    for (const auto& [_, b] : unicos) {
        std::cout << "   ";
        b->mostrar();
        std::cout << "\n";
    }
    std::cout << "------------------------\n";
}

#endif