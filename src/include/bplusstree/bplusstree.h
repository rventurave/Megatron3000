#ifndef ARBOL_BPLUS_H
#define ARBOL_BPLUS_H

#include <vector>
#include <map>
#include <iostream>
#include <fstream>

struct NodoBPlus {
    bool esHoja;
    std::vector<int> claves;
    std::vector<NodoBPlus*> hijos;
    NodoBPlus* siguiente;

    NodoBPlus(bool hoja);
};

class bPlussTree {
private:
    NodoBPlus* raiz;
    int maxClavesHoja;

    void dividirHoja(NodoBPlus* nodo, NodoBPlus** nuevoNodo, int* clavePromovida);
    void dividirInterno(NodoBPlus* nodo, NodoBPlus** nuevoNodo, int* clavePromovida);
    void insertarRec(NodoBPlus* nodo, int clave, NodoBPlus** nuevoNodo, int* clavePromovida);
    bool eliminarRec(NodoBPlus* nodo, int clave, NodoBPlus* padre, int indicePadre);
    void generarDot(NodoBPlus* nodo, std::ostream& out, int& id, std::map<NodoBPlus*, int>& ids);

public:
    bPlussTree(int max);

    void insertar(int clave);
    void eliminar(int clave);
    void imprimir();
    void generarImagen();
};

#endif
