#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <vector>
#include <map>
#include <iostream>

// Declaración adelantada para evitar dependencias circulares si fuera necesario,
// aunque en este caso NodoBPlus es una estructura y no hay problema.
struct NodoBPlus;

class bPlussTree
{
private:
    // Estructura interna para los nodos del árbol B+
    struct NodoBPlus
    {
        bool esHoja;
        std::vector<int> claves;
        std::vector<NodoBPlus *> hijos;
        NodoBPlus *siguiente; // Puntero al siguiente nodo hoja en el mismo nivel

        NodoBPlus(bool hoja);
    };

    NodoBPlus *raiz;
    int maxClavesHoja;
    int minClavesHoja;
    int minClavesInterno;
    int minHijosInterno;

    // Métodos privados auxiliares
    void dividirHoja(NodoBPlus *nodo, NodoBPlus **nuevoNodo, int *clavePromovida);
    void dividirInterno(NodoBPlus *nodo, NodoBPlus **nuevoNodo, int *clavePromovida);
    void insertarRec(NodoBPlus *nodo, int clave, NodoBPlus **nuevoNodo, int *clavePromovida);
    bool manejarSubflujoHoja(NodoBPlus *nodo, NodoBPlus *padre, int indiceEnPadre);
    bool manejarSubflujoInterno(NodoBPlus *nodo, NodoBPlus *padre, int indiceEnPadre);
    void actualizarSeparadoras(NodoBPlus *padre);
    bool eliminarRec(NodoBPlus *nodoActual, int clave, NodoBPlus *padre, int indiceEnPadre);
    void generarDot(NodoBPlus *nodo, std::ostream &out, int &id, std::map<NodoBPlus *, int> &ids);
    bool buscarRec(NodoBPlus *nodo, int clave) const;

public:
    // Constructor del árbol B+
    bPlussTree(int max);

    // Métodos públicos para operar el árbol
    void insertar(int clave);
    void eliminar(int clave);
    void imprimir();
    void generarImagen();

    // Métodos de búsqueda
    bool buscar(int clave) const;

};

#endif // BPLUSTREE_H