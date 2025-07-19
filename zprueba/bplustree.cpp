#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <cstdlib>

struct NodoBPlus
{
    bool esHoja;
    std::vector<int> claves;
    std::vector<NodoBPlus *> hijos;
    NodoBPlus *siguiente;

    NodoBPlus(bool hoja) : esHoja(hoja), siguiente(nullptr) {}
};

class bPlussTree
{
private:
    NodoBPlus *raiz;
    int maxClavesHoja;
    int minClavesHoja;
    int minClavesInterno;
    int minHijosInterno;

    void dividirHoja(NodoBPlus *nodo, NodoBPlus **nuevoNodo, int *clavePromovida)
    {
        *nuevoNodo = new NodoBPlus(true);
        int mid = (maxClavesHoja + 1) / 2;
        (*nuevoNodo)->claves.assign(nodo->claves.begin() + mid, nodo->claves.end());
        nodo->claves.resize(mid);
        (*nuevoNodo)->siguiente = nodo->siguiente;
        nodo->siguiente = *nuevoNodo;
        *clavePromovida = (*nuevoNodo)->claves[0];
    }

    void dividirInterno(NodoBPlus *nodo, NodoBPlus **nuevoNodo, int *clavePromovida)
    {
        int mid = nodo->claves.size() / 2;
        *nuevoNodo = new NodoBPlus(false);
        *clavePromovida = nodo->claves[mid];
        (*nuevoNodo)->claves.assign(nodo->claves.begin() + mid + 1, nodo->claves.end());
        (*nuevoNodo)->hijos.assign(nodo->hijos.begin() + mid + 1, nodo->hijos.end());
        nodo->claves.resize(mid);
        nodo->hijos.resize(mid + 1);
    }

    void insertarRec(NodoBPlus *nodo, int clave, NodoBPlus **nuevoNodo, int *clavePromovida)
    {
        int pos = std::upper_bound(nodo->claves.begin(), nodo->claves.end(), clave) - nodo->claves.begin();

        if (nodo->esHoja)
        {
            if ((int)nodo->claves.size() == maxClavesHoja)
            {
                std::vector<int> temp = nodo->claves;
                temp.insert(temp.begin() + pos, clave);
                nodo->claves = temp;
                dividirHoja(nodo, nuevoNodo, clavePromovida);
            }
            else
            {
                nodo->claves.insert(nodo->claves.begin() + pos, clave);
                *nuevoNodo = nullptr;
            }
        }
        else
        {
            NodoBPlus *hijo = nodo->hijos[pos];
            NodoBPlus *nuevoHijo = nullptr;
            int nuevaClave = 0;
            insertarRec(hijo, clave, &nuevoHijo, &nuevaClave);
            if (nuevoHijo)
            {
                // POSIBLE DUPLICADO EN NODO INTERNO —> VERIFICAMOS
                int posInterno = std::upper_bound(nodo->claves.begin(), nodo->claves.end(), nuevaClave) - nodo->claves.begin();
                bool esDuplicado = (posInterno > 0 && nodo->claves[posInterno - 1] == nuevaClave);

                if (!esDuplicado)
                {
                    nodo->claves.insert(nodo->claves.begin() + posInterno, nuevaClave);
                    nodo->hijos.insert(nodo->hijos.begin() + posInterno + 1, nuevoHijo);
                }
                else
                {
                    nodo->hijos.insert(nodo->hijos.begin() + posInterno + 1, nuevoHijo);
                }

                if ((int)nodo->claves.size() > maxClavesHoja)
                {
                    dividirInterno(nodo, nuevoNodo, clavePromovida);
                }
                else
                {
                    *nuevoNodo = nullptr;
                }
            }
            else
            {
                *nuevoNodo = nullptr;
            }
        }
    }

    bool manejarSubflujoHoja(NodoBPlus *nodo, NodoBPlus *padre, int indiceEnPadre)
    {
        if (indiceEnPadre > 0)
        { // Intento de redistribución con hermano izquierdo
            NodoBPlus *hermanoIzq = padre->hijos[indiceEnPadre - 1];
            if (hermanoIzq->esHoja && (int)hermanoIzq->claves.size() > minClavesHoja)
            {
                nodo->claves.insert(nodo->claves.begin(), hermanoIzq->claves.back());
                hermanoIzq->claves.pop_back();
                padre->claves[indiceEnPadre - 1] = nodo->claves[0];
                return true;
            }
        }

        if (indiceEnPadre < (int)padre->hijos.size() - 1)
        { // Intento de redistribución con hermano derecho
            NodoBPlus *hermanoDer = padre->hijos[indiceEnPadre + 1];
            if (hermanoDer->esHoja && (int)hermanoDer->claves.size() > minClavesHoja)
            {
                nodo->claves.push_back(hermanoDer->claves.front());
                hermanoDer->claves.erase(hermanoDer->claves.begin());
                padre->claves[indiceEnPadre] = hermanoDer->claves[0];
                return true;
            }
        }

        // Si no se pudo redistribuir, intentar fusión
        if (indiceEnPadre > 0)
        { // Fusión con hermano izquierdo
            NodoBPlus *hermanoIzq = padre->hijos[indiceEnPadre - 1];
            hermanoIzq->claves.insert(hermanoIzq->claves.end(), nodo->claves.begin(), nodo->claves.end());
            hermanoIzq->siguiente = nodo->siguiente;
            padre->claves.erase(padre->claves.begin() + indiceEnPadre - 1);
            padre->hijos.erase(padre->hijos.begin() + indiceEnPadre);
            delete nodo;
        }
        else
        { // Fusión con hermano derecho (si no hay izquierdo, debe haber derecho)
            NodoBPlus *hermanoDer = padre->hijos[indiceEnPadre + 1];
            nodo->claves.insert(nodo->claves.end(), hermanoDer->claves.begin(), hermanoDer->claves.end());
            nodo->siguiente = hermanoDer->siguiente;
            padre->claves.erase(padre->claves.begin() + indiceEnPadre);
            padre->hijos.erase(padre->hijos.begin() + indiceEnPadre + 1);
            delete hermanoDer;
        }
        return true;
    }

    bool manejarSubflujoInterno(NodoBPlus *nodo, NodoBPlus *padre, int indiceEnPadre)
    {
        if (indiceEnPadre > 0)
        { // Intento de redistribución con hermano izquierdo
            NodoBPlus *hermanoIzq = padre->hijos[indiceEnPadre - 1];
            if ((int)hermanoIzq->claves.size() > minClavesInterno)
            {
                nodo->claves.insert(nodo->claves.begin(), padre->claves[indiceEnPadre - 1]);
                padre->claves[indiceEnPadre - 1] = hermanoIzq->claves.back();
                hermanoIzq->claves.pop_back();
                nodo->hijos.insert(nodo->hijos.begin(), hermanoIzq->hijos.back());
                hermanoIzq->hijos.pop_back();
                return true;
            }
        }

        if (indiceEnPadre < (int)padre->hijos.size() - 1)
        { // Intento de redistribución con hermano derecho
            NodoBPlus *hermanoDer = padre->hijos[indiceEnPadre + 1];
            if ((int)hermanoDer->claves.size() > minClavesInterno)
            {
                nodo->claves.push_back(padre->claves[indiceEnPadre]);
                padre->claves[indiceEnPadre] = hermanoDer->claves.front();
                hermanoDer->claves.erase(hermanoDer->claves.begin());
                nodo->hijos.push_back(hermanoDer->hijos.front());
                hermanoDer->hijos.erase(hermanoDer->hijos.begin());
                return true;
            }
        }

        // Si no se pudo redistribuir, intentar fusión
        if (indiceEnPadre > 0)
        { // Fusión con hermano izquierdo
            NodoBPlus *hermanoIzq = padre->hijos[indiceEnPadre - 1];
            hermanoIzq->claves.push_back(padre->claves[indiceEnPadre - 1]);
            hermanoIzq->claves.insert(hermanoIzq->claves.end(), nodo->claves.begin(), nodo->claves.end());
            hermanoIzq->hijos.insert(hermanoIzq->hijos.end(), nodo->hijos.begin(), nodo->hijos.end());
            padre->claves.erase(padre->claves.begin() + indiceEnPadre - 1);
            padre->hijos.erase(padre->hijos.begin() + indiceEnPadre);
            delete nodo;
        }
        else
        { // Fusión con hermano derecho
            NodoBPlus *hermanoDer = padre->hijos[indiceEnPadre + 1];
            nodo->claves.push_back(padre->claves[indiceEnPadre]);
            nodo->claves.insert(nodo->claves.end(), hermanoDer->claves.begin(), hermanoDer->claves.end());
            nodo->hijos.insert(nodo->hijos.end(), hermanoDer->hijos.begin(), hermanoDer->hijos.end());
            padre->claves.erase(padre->claves.begin() + indiceEnPadre);
            padre->hijos.erase(padre->hijos.begin() + indiceEnPadre + 1);
            delete hermanoDer;
        }
        return true;
    }
    void actualizarSeparadoras(NodoBPlus *padre)
    {
        if (!padre || padre->esHoja)
            return;
        for (int i = 0; i < (int)padre->claves.size(); ++i)
        {
            NodoBPlus *hijo = padre->hijos[i + 1];
            // Busca la hoja más a la izquierda del subárbol
            while (hijo && !hijo->esHoja)
                hijo = hijo->hijos[0];
            if (hijo && !hijo->claves.empty())
                padre->claves[i] = hijo->claves.front();
        }
        // Si el nodo es la raíz, fuerza todas las claves root a ser las correctas
        if (padre == raiz && !raiz->esHoja && raiz->hijos.size() > 1)
        {
            for (int i = 0; i < (int)raiz->claves.size(); ++i)
            {
                NodoBPlus *hijo = raiz->hijos[i + 1];
                while (hijo && !hijo->esHoja)
                    hijo = hijo->hijos[0];
                if (hijo && !hijo->claves.empty())
                    raiz->claves[i] = hijo->claves.front();
            }
        }
    }

    bool eliminarRec(NodoBPlus *nodoActual, int clave, NodoBPlus *padre, int indiceEnPadre)
    {
        int pos = std::upper_bound(nodoActual->claves.begin(), nodoActual->claves.end(), clave) - nodoActual->claves.begin();

        if (nodoActual->esHoja)
        {
            auto it = std::find(nodoActual->claves.begin(), nodoActual->claves.end(), clave);
            if (it == nodoActual->claves.end())
                return false;

            bool eraPrimeraClave = (it == nodoActual->claves.begin());
            nodoActual->claves.erase(it);

            if (nodoActual == raiz)
            {
                // Si la raíz es hoja y queda vacía, elimina el árbol
                if (nodoActual->claves.empty())
                {
                    delete raiz;
                    raiz = nullptr;
                }
                return true;
            }

            // Actualiza la clave separadora en el padre si era la primera clave
            if (padre && eraPrimeraClave)
            {
                for (size_t i = 0; i < padre->claves.size(); ++i)
                {
                    if (padre->hijos[i + 1] == nodoActual)
                    {
                        if (!nodoActual->claves.empty())
                            padre->claves[i] = nodoActual->claves[0];
                        else if (i > 0 && padre->hijos[i] && !padre->hijos[i]->claves.empty())
                            padre->claves[i - 1] = padre->hijos[i]->claves[0];
                        break;
                    }
                }
                // Si el padre es la raíz, fuerza la actualización del root
                if (padre == raiz && !raiz->esHoja && raiz->hijos.size() > 1 && !raiz->hijos[1]->claves.empty())
                {
                    raiz->claves[0] = raiz->hijos[1]->claves.front();
                }
            }

            if ((int)nodoActual->claves.size() >= minClavesHoja)
                return true;

            bool fusion = manejarSubflujoHoja(nodoActual, padre, indiceEnPadre);
            // Tras fusión, actualizar separadoras del padre
            if (fusion && padre)
            {
                actualizarSeparadoras(padre);
                // Si el padre es la raíz, fuerza la actualización del root
                if (padre == raiz && !raiz->esHoja && raiz->hijos.size() > 1 && !raiz->hijos[1]->claves.empty())
                {
                    raiz->claves[0] = raiz->hijos[1]->claves.front();
                }
            }
            return fusion;
        }
        else
        {
            bool resultado = eliminarRec(nodoActual->hijos[pos], clave, nodoActual, pos);
            if (!resultado)
                return false;

            actualizarSeparadoras(nodoActual); // Siempre actualiza separadoras después de llamada recursiva

            if (nodoActual == raiz)
            {
                // Si la raíz es interna y queda con un solo hijo, reemplázala por ese hijo
                while (raiz && !raiz->esHoja && raiz->claves.empty() && raiz->hijos.size() == 1)
                {
                    NodoBPlus *temp = raiz;
                    raiz = raiz->hijos[0];
                    delete temp;
                }
                // Actualizar separadora de la raíz si es interna
                if (raiz && !raiz->esHoja)
                {
                    actualizarSeparadoras(raiz);
                    // Fuerza la clave root a ser la del primer hijo derecho
                    if (raiz->hijos.size() > 1 && !raiz->hijos[1]->claves.empty())
                        raiz->claves[0] = raiz->hijos[1]->claves.front();
                }
                return true;
            }

            if ((int)nodoActual->claves.size() >= minClavesInterno &&
                (int)nodoActual->hijos.size() >= minHijosInterno)
            {
                return true;
            }

            bool fusion = manejarSubflujoInterno(nodoActual, padre, indiceEnPadre);
            // Tras fusión, actualizar separadoras del padre
            if (fusion && padre)
            {
                actualizarSeparadoras(padre);
                // Si el padre es la raíz, fuerza la actualización del root
                if (padre == raiz && !raiz->esHoja && raiz->hijos.size() > 1 && !raiz->hijos[1]->claves.empty())
                {
                    raiz->claves[0] = raiz->hijos[1]->claves.front();
                }
            }
            return fusion;
        }
        return false;
    }

    void generarDot(NodoBPlus *nodo, std::ostream &out, int &id, std::map<NodoBPlus *, int> &ids)
    {
        if (ids.count(nodo))
            return;
        int nodoId = id++;
        ids[nodo] = nodoId;
        out << "n" << nodoId << " [label=<";
        out << "<TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"0\" BGCOLOR=\"" << (nodo->esHoja ? "lightblue" : "lightgray") << "\"><TR>";
        for (int clave : nodo->claves)
            out << "<TD>" << clave << "</TD>";
        out << "</TR></TABLE>";
        out << ">, shape=plaintext];\n";
        if (!nodo->esHoja)
        {
            for (NodoBPlus *hijo : nodo->hijos)
            {
                generarDot(hijo, out, id, ids);
                out << "n" << nodoId << " -> n" << ids[hijo] << ";\n";
            }
        }
        if (nodo->esHoja && nodo->siguiente)
        {
            generarDot(nodo->siguiente, out, id, ids);
            out << "n" << nodoId << " -> n" << ids[nodo->siguiente] << " [color=blue, style=dashed, constraint=false];\n";
        }
    }

public:
    bPlussTree(int max) : maxClavesHoja(max)
    {
        raiz = new NodoBPlus(true);
        minClavesHoja = (maxClavesHoja + 1) / 2;        // Para maxClavesHoja = 2, m=3, minClavesHoja = 1.
        minClavesInterno = (maxClavesHoja + 1) / 2 - 1; // Min claves en nodos internos.
        minHijosInterno = (maxClavesHoja + 1) / 2;      // Min hijos en nodos internos.
    }

    void insertar(int clave)
    {
        NodoBPlus *nuevoNodo = nullptr;
        int clavePromovida = 0;
        insertarRec(raiz, clave, &nuevoNodo, &clavePromovida);
        if (nuevoNodo)
        {
            NodoBPlus *nuevaRaiz = new NodoBPlus(false);
            nuevaRaiz->claves.push_back(clavePromovida);
            nuevaRaiz->hijos.push_back(raiz);
            nuevaRaiz->hijos.push_back(nuevoNodo);
            raiz = nuevaRaiz;
        }
        actualizarSeparadoras(raiz);
    }

    void eliminar(int clave)
    {
        if (!raiz)
        {
            std::cout << "El arbol esta vacio.\n";
            return;
        }
        if (!eliminarRec(raiz, clave, nullptr, 0))
            std::cout << "Clave no encontrada.\n";
        else
        {
            actualizarSeparadoras(raiz);
            std::cout << "Clave eliminada y arbol reorganizado.\n";
        }
    }

    void imprimir()
    {
        if (!raiz)
        {
            std::cout << "El arbol esta vacio.\n";
            return;
        }
        NodoBPlus *actual = raiz;
        while (!actual->esHoja)
            actual = actual->hijos[0];
        std::cout << "Hojas encadenadas:\n";
        while (actual)
        {
            std::cout << "[ ";
            for (int k : actual->claves)
                std::cout << k << " ";
            std::cout << "] -> ";
            actual = actual->siguiente;
        }
        std::cout << "NULL\n";
    }

    void generarImagen()
    {
        if (!raiz)
        {
            std::cout << "El arbol esta vacio, no se puede generar imagen.\n";
            return;
        }
        std::ofstream out("arbol.dot");
        out << "digraph BPlusTree {\nnode [shape=plaintext];\n";
        int id = 0;
        std::map<NodoBPlus *, int> ids;
        generarDot(raiz, out, id, ids);
        out << "}\n";
        out.close();
        system("dot -Tpng arbol.dot -o arbol.png");
        system("start arbol.png");
        std::cout << "Imagen generada: arbol.png\n";
    }
};

int main()
{
    int max;
    std::cout << "Ingrese el número maximo de claves por hoja (orden m-1): ";
    std::cin >> max;
    //max = 2; // Ejemplo de orden m-1
    bPlussTree arbol(max);
    while (true)
    {
        int op, val;
        std::cout << "\n1. Insertar\n2. Eliminar\n3. Imprimir\n4. Mostrar arbol\n5. Salir\nOpción: ";
        std::cin >> op;
        if (op == 1)
        {
            /*std::vector<int> claves = {45, 61, 32, 89, 100, 2, 6}; // Ejemplo de claves
            for (int clave : claves)
            {
                arbol.insertar(clave);
            }
            */
            std::cout << "Valor a insertar: ";
            std::cin >> val;
            arbol.insertar(val);
        }
        else if (op == 2)
        {
            std::cout << "Valor a eliminar: ";
            std::cin >> val;
            arbol.eliminar(val);
        }
        else if (op == 3)
        {
            arbol.imprimir();
        }
        else if (op == 4)
        {
            arbol.generarImagen();
        }
        else if (op == 5)
            break;
        else
            std::cout << "Opción invalida.\n";
    }
    return 0;
}