#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <cstdlib>

struct NodoBPlus {
    bool esHoja;
    std::vector<int> claves;
    std::vector<NodoBPlus*> hijos;
    NodoBPlus* siguiente;

    NodoBPlus(bool hoja) : esHoja(hoja), siguiente(nullptr) {}
};

class bPlussTree {
private:
    NodoBPlus* raiz;
    int maxClavesHoja;

    void dividirHoja(NodoBPlus* nodo, NodoBPlus** nuevoNodo, int* clavePromovida) {
        *nuevoNodo = new NodoBPlus(true);
        (*nuevoNodo)->claves.assign(nodo->claves.begin() + 1, nodo->claves.end());
        nodo->claves.resize(1);
        (*nuevoNodo)->siguiente = nodo->siguiente;
        nodo->siguiente = *nuevoNodo;
        *clavePromovida = (*nuevoNodo)->claves[0];
    }

    void dividirInterno(NodoBPlus* nodo, NodoBPlus** nuevoNodo, int* clavePromovida) {
        int mid = nodo->claves.size() / 2;
        *nuevoNodo = new NodoBPlus(false);
        *clavePromovida = nodo->claves[mid];
        (*nuevoNodo)->claves.assign(nodo->claves.begin() + mid + 1, nodo->claves.end());
        (*nuevoNodo)->hijos.assign(nodo->hijos.begin() + mid + 1, nodo->hijos.end());
        nodo->claves.resize(mid);
        nodo->hijos.resize(mid + 1);
    }

    void insertarRec(NodoBPlus* nodo, int clave, NodoBPlus** nuevoNodo, int* clavePromovida) {
        int pos = std::upper_bound(nodo->claves.begin(), nodo->claves.end(), clave) - nodo->claves.begin();
        if (nodo->esHoja) {
            if ((int)nodo->claves.size() == maxClavesHoja) {
                std::vector<int> temp = nodo->claves;
                temp.insert(temp.begin() + pos, clave);
                nodo->claves = temp;
                dividirHoja(nodo, nuevoNodo, clavePromovida);
            } else {
                nodo->claves.insert(nodo->claves.begin() + pos, clave);
                *nuevoNodo = nullptr;
            }
        } else {
            NodoBPlus* hijo = nodo->hijos[pos];
            NodoBPlus* nuevoHijo = nullptr;
            int nuevaClave = 0;
            insertarRec(hijo, clave, &nuevoHijo, &nuevaClave);
            if (nuevoHijo) {
                int posInterno = std::upper_bound(nodo->claves.begin(), nodo->claves.end(), nuevaClave) - nodo->claves.begin();
                nodo->claves.insert(nodo->claves.begin() + posInterno, nuevaClave);
                nodo->hijos.insert(nodo->hijos.begin() + posInterno + 1, nuevoHijo);
                if ((int)nodo->claves.size() > maxClavesHoja) {
                    dividirInterno(nodo, nuevoNodo, clavePromovida);
                } else {
                    *nuevoNodo = nullptr;
                }
            } else {
                *nuevoNodo = nullptr;
            }
        }
    }

    bool eliminarRec(NodoBPlus* nodo, int clave, NodoBPlus* padre, int indicePadre) {
        if (nodo->esHoja) {
            auto it = std::find(nodo->claves.begin(), nodo->claves.end(), clave);
            if (it == nodo->claves.end()) return false;
            nodo->claves.erase(it);

            // Si es raíz o tiene suficientes claves, no necesita reorganización
            if (nodo == raiz || nodo->claves.size() >= (maxClavesHoja + 1) / 2) return true;

            NodoBPlus* hermanoIzq = nullptr;
            NodoBPlus* hermanoDer = nullptr;
            if (padre) {
                if (indicePadre > 0)
                    hermanoIzq = padre->hijos[indicePadre - 1];
                if (indicePadre < padre->hijos.size() - 1)
                    hermanoDer = padre->hijos[indicePadre + 1];
            }

            // Redistribución con hermano izquierdo
            if (hermanoIzq && hermanoIzq->claves.size() > (maxClavesHoja + 1) / 2) {
                nodo->claves.insert(nodo->claves.begin(), hermanoIzq->claves.back());
                hermanoIzq->claves.pop_back();
                padre->claves[indicePadre - 1] = nodo->claves[0];
                return true;
            }
            // Redistribución con hermano derecho
            if (hermanoDer && hermanoDer->claves.size() > (maxClavesHoja + 1) / 2) {
                nodo->claves.push_back(hermanoDer->claves.front());
                hermanoDer->claves.erase(hermanoDer->claves.begin());
                padre->claves[indicePadre] = hermanoDer->claves[0];
                return true;
            }
            // Fusión con hermano izquierdo
            if (hermanoIzq) {
                hermanoIzq->claves.insert(hermanoIzq->claves.end(), nodo->claves.begin(), nodo->claves.end());
                hermanoIzq->siguiente = nodo->siguiente;
                padre->claves.erase(padre->claves.begin() + indicePadre - 1);
                padre->hijos.erase(padre->hijos.begin() + indicePadre);
                delete nodo;
                // Si el padre es la raíz y queda sin claves, actualiza la raíz
                if (padre == raiz && padre->claves.empty()) {
                    NodoBPlus* temp = padre;
                    raiz = padre->hijos[0];
                    delete temp;
                }
                return true;
            }
            // Fusión con hermano derecho
            if (hermanoDer) {
                nodo->claves.insert(nodo->claves.end(), hermanoDer->claves.begin(), hermanoDer->claves.end());
                nodo->siguiente = hermanoDer->siguiente;
                padre->claves.erase(padre->claves.begin() + indicePadre);
                padre->hijos.erase(padre->hijos.begin() + indicePadre + 1);
                delete hermanoDer;
                if (padre == raiz && padre->claves.empty()) {
                    NodoBPlus* temp = padre;
                    raiz = padre->hijos[0];
                    delete temp;
                }
                return true;
            }
        } else {
            int pos = std::upper_bound(nodo->claves.begin(), nodo->claves.end(), clave) - nodo->claves.begin();
            bool result = eliminarRec(nodo->hijos[pos], clave, nodo, pos);
            if (!result) return false;

            // Actualiza claves promovidas en el nodo interno
            for (int i = 0; i < (int)nodo->claves.size(); ++i) {
                if (!nodo->hijos[i + 1]->claves.empty() && nodo->claves[i] != nodo->hijos[i + 1]->claves.front()) {
                    nodo->claves[i] = nodo->hijos[i + 1]->claves.front();
                }
            }

            // Si la raíz queda sin claves, actualiza la raíz
            if (nodo == raiz && nodo->claves.empty()) {
                NodoBPlus* temp = nodo;
                raiz = nodo->hijos[0];
                delete temp;
            }
            return true;
        }
        return false;
    }

    void generarDot(NodoBPlus* nodo, std::ostream& out, int& id, std::map<NodoBPlus*, int>& ids) {
        if (ids.count(nodo)) return;
        int nodoId = id++;
        ids[nodo] = nodoId;
        out << "n" << nodoId << " [label=<";
        out << "<TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"0\" BGCOLOR=\"" << (nodo->esHoja ? "lightblue" : "lightgray") << "\"><TR>";
        for (int clave : nodo->claves) out << "<TD>" << clave << "</TD>";
        out << "</TR></TABLE>";
        out << ">, shape=plaintext];\n";
        if (!nodo->esHoja) {
            for (NodoBPlus* hijo : nodo->hijos) {
                generarDot(hijo, out, id, ids);
                out << "n" << nodoId << " -> n" << ids[hijo] << ";\n";
            }
        }
        if (nodo->esHoja && nodo->siguiente) {
            generarDot(nodo->siguiente, out, id, ids);
            out << "n" << nodoId << " -> n" << ids[nodo->siguiente] << " [color=blue, style=dashed, constraint=false];\n";
        }
    }

public:
    bPlussTree(int max) : maxClavesHoja(max) {
        raiz = new NodoBPlus(true);
    }

    void insertar(int clave) {
        NodoBPlus* nuevoNodo = nullptr;
        int clavePromovida = 0;
        insertarRec(raiz, clave, &nuevoNodo, &clavePromovida);
        if (nuevoNodo) {
            NodoBPlus* nuevaRaiz = new NodoBPlus(false);
            nuevaRaiz->claves.push_back(clavePromovida);
            nuevaRaiz->hijos.push_back(raiz);
            nuevaRaiz->hijos.push_back(nuevoNodo);
            raiz = nuevaRaiz;
        }
    }

    void eliminar(int clave) {
        if (!eliminarRec(raiz, clave, nullptr, 0))
            std::cout << "Clave no encontrada.\n";
        else
            std::cout << "Clave eliminada y árbol reorganizado.\n";
    }

    void imprimir() {
        NodoBPlus* actual = raiz;
        while (!actual->esHoja) actual = actual->hijos[0];
        std::cout << "Hojas encadenadas:\n";
        while (actual) {
            std::cout << "[ ";
            for (int k : actual->claves) std::cout << k << " ";
            std::cout << "] -> ";
            actual = actual->siguiente;
        }
        std::cout << "NULL\n";
    }

    void generarImagen() {
        std::ofstream out("arbol.dot");
        out << "digraph BPlusTree {\nnode [shape=plaintext];\n";
        int id = 0;
        std::map<NodoBPlus*, int> ids;
        generarDot(raiz, out, id, ids);
        out << "}\n";
        out.close();
        system("dot -Tpng arbol.dot -o arbol.png");
        system("start arbol.png");
        std::cout << "Imagen generada: arbol.png\n";
    }
};

int main() {
    int max;
    std::cout << "Ingrese el número máximo de claves por hoja: ";
    std::cin >> max;
    bPlussTree arbol(max);
    while (true) {
        int op, val;
        std::cout << "\n1. Insertar\n2. Eliminar\n3. Imprimir\n4. Mostrar árbol\n5. Salir\nOpción: ";
        std::cin >> op;
        if (op == 1) {
            std::cout << "Valor: "; std::cin >> val;
            arbol.insertar(val);
        } else if (op == 2) {
            std::cout << "Valor a eliminar: "; std::cin >> val;
            arbol.eliminar(val);
        } else if (op == 3) {
            arbol.imprimir();
        } else if (op == 4) {
            arbol.generarImagen();
        } else if (op == 5) break;
        else std::cout << "Opción inválida.\n";
    }
    return 0;
}