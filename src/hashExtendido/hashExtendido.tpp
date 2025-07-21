#ifndef HASH_EXTENDIDO_TPP
#define HASH_EXTENDIDO_TPP

#include "../include/hashExtendido/hashExtendido.h"

// --- Implementación Bucket ---
template <typename K, typename V>
Bucket<K, V>::Bucket(int cap, int prof) : capacidad(cap), profundidadLocal(prof) {}

template <typename K, typename V>
bool Bucket<K, V>::lleno() const {
    return elementos.size() >= capacidad;
}

template <typename K, typename V>
bool Bucket<K, V>::agregar(const K& clave, const V& valor) {
    if (!lleno()) {
        elementos.push_back(std::make_pair(clave, valor));
        return true;
    }
    return false;
}

template <typename K, typename V>
bool Bucket<K, V>::contiene(const K& clave) const {
    for (const auto& e : elementos) {
        if (e.first == clave) return true;
    }
    return false;
}

template <typename K, typename V>
bool Bucket<K, V>::eliminar(const K& clave) {
    for (size_t i = 0; i < elementos.size(); ++i) {
        if (elementos[i].first == clave) {
            elementos.erase(elementos.begin() + i);
            return true;
        }
    }
    return false;
}

template <typename K, typename V>
V Bucket<K, V>::obtener(const K& clave) const {
    for (const auto& e : elementos) {
        if (e.first == clave) return e.second;
    }
    throw std::out_of_range("Clave no encontrada en el bucket");
}

template <typename K, typename V>
void Bucket<K, V>::mostrar() const {
    std::cout << "Bucket(prof=" << profundidadLocal << ", elementos={";
    for (size_t i = 0; i < elementos.size(); ++i) {
        std::cout << "[" << elementos[i].first << ": " << elementos[i].second << "]";
        if (i + 1 < elementos.size()) std::cout << ", ";
    }
    std::cout << "})";
}

// --- Implementación HashExtendido ---
template <typename K, typename V>
HashExtendido<K, V>::HashExtendido(int cap) : capacidadBucket(cap), profundidadGlobal(1) {
    directorio.resize(1 << profundidadGlobal);
    for (int i = 0; i < (1 << profundidadGlobal); ++i)
        directorio[i] = std::make_shared<Bucket<K, V>>(capacidadBucket, profundidadGlobal);
}

template <typename K, typename V>
size_t HashExtendido<K, V>::obtenerHash(const K& clave) const {
    return std::hash<K>{}(clave);
}

template <typename K, typename V>
int HashExtendido<K, V>::obtenerIndice(size_t hash) const {
    return hash & ((1 << profundidadGlobal) - 1);
}

template <typename K, typename V>
void HashExtendido<K, V>::dividirBucket(int indice) {
    auto viejo = directorio[indice];
    int profAnt = viejo->profundidadLocal;
    viejo->profundidadLocal++;

    auto b0 = std::make_shared<Bucket<K, V>>(capacidadBucket, viejo->profundidadLocal);
    auto b1 = std::make_shared<Bucket<K, V>>(capacidadBucket, viejo->profundidadLocal);

    for (const auto& par : viejo->elementos) {
        int bit = (obtenerHash(par.first) >> profAnt) & 1;
        if (bit == 0)
            b0->agregar(par.first, par.second);
        else
            b1->agregar(par.first, par.second);
    }

    viejo->elementos.clear();

    for (int i = 0; i < directorio.size(); ++i) {
        if (directorio[i] == viejo) {
            int bit = (i >> profAnt) & 1;
            directorio[i] = (bit == 0) ? b0 : b1;
        }
    }
}

template <typename K, typename V>
void HashExtendido<K, V>::fusionarBuckets(int indice) {
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

template <typename K, typename V>
void HashExtendido<K, V>::insertar(const K& clave, const V& valor) {
    while (true) {
        size_t h = obtenerHash(clave);
        int indice = obtenerIndice(h);
        auto bucket = directorio[indice];

        if (bucket->contiene(clave)) return;

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
            bucket->agregar(clave, valor);
            break;
        }
    }
}

template <typename K, typename V>
V HashExtendido<K, V>::buscar(const K& clave) const {
    size_t hash = obtenerHash(clave);
    int indice = obtenerIndice(hash);
    auto bucket = directorio[indice];
    for (const auto& par : bucket->elementos) {
        if (par.first == clave) {
            return par.second;
        }
    }
    return -1;
}

template <typename K, typename V>
bool HashExtendido<K, V>::eliminar(const K& clave) {
    size_t h = obtenerHash(clave);
    int indice = obtenerIndice(h);
    auto bucket = directorio[indice];

    if (bucket->eliminar(clave)) {
        if (bucket->elementos.empty()) {
            fusionarBuckets(indice);
        }
        return true;
    } else {
        return false;
    }
}

template <typename K, typename V>
void HashExtendido<K, V>::mostrar() const {
    std::cout << "\n--- Estado del Hash ---\n";
    std::cout << "Profundidad Global: " << profundidadGlobal << "\n";
    std::cout << "Tamano del Directorio: " << directorio.size() << "\n";

    std::map<void*, std::shared_ptr<Bucket<K, V>>> unicos;
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