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

template <typename K, typename V>
class Bucket {
public:
    int capacidad;
    int profundidadLocal;
    std::vector<std::pair<K, V>> elementos;

    Bucket(int cap, int prof);

    bool lleno() const;
    bool agregar(const K& clave, const V& valor);
    bool contiene(const K& clave) const;
    bool eliminar(const K& clave);
    V obtener(const K& clave) const;
    void mostrar() const;
};

template <typename K, typename V>
class HashExtendido {
private:
    int capacidadBucket;
    int profundidadGlobal;
    std::vector<std::shared_ptr<Bucket<K, V>>> directorio;

    size_t obtenerHash(const K& clave) const;
    int obtenerIndice(size_t hash) const;
    void dividirBucket(int indice);
    void fusionarBuckets(int indice);

public:
    HashExtendido(int cap);

    void insertar(const K& clave, const V& valor);
    V buscar(const K& clave) const; // Devuelve el valor asociado a la clave, lanza excepción si no existe
    bool eliminar(const K& clave);
    void mostrar() const;
};

#include "../hashExtendido/hashExtendido.tpp"

#endif
