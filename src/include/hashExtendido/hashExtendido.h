#ifndef HASH_EXTENDIDO_H
#define HASH_EXTENDIDO_H

#include <vector>
#include <memory>
#include <map>
#include <string>
#include <iostream>
#include <bitset>
#include <iomanip>
#include <functional>

template <typename T>
class Bucket {
public:
    int capacidad;
    int profundidadLocal;
    std::vector<T> elementos;

    Bucket(int cap, int prof);

    bool lleno() const;
    bool agregar(const T& valor);
    bool contiene(const T& valor) const;
    bool eliminar(const T& valor);
    void mostrar() const;
};

template <typename T>
class HashExtendido {
private:
    int capacidadBucket;
    int profundidadGlobal;
    std::vector<std::shared_ptr<Bucket<T>>> directorio;

    size_t obtenerHash(const T& valor) const;
    int obtenerIndice(size_t hash) const;
    void dividirBucket(int indice);
    void fusionarBuckets(int indice);

public:
    HashExtendido(int cap);

    void insertar(const T& valor);
    bool buscar(const T& valor) const;
    bool eliminar(const T& valor);
    void mostrar() const;
};

#include "../hashExtendido/hashExtendido.tpp"

#endif
