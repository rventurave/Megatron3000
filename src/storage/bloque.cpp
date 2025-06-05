#include "../include/storage/bloque.h"
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

bloque::bloque() {
    memset(bloqueB, 0, sizeof(bloqueB));
}

void bloque::agregarAlBloque(const char* contenido) {
    int lenActual = strlen(bloqueB);
    int lenNuevo = strlen(contenido);
    if (lenActual + lenNuevo + 2 < 50000) {
        strcat(bloqueB, contenido);
        strcat(bloqueB, "|");
    } else {
        cout << "Bloque lleno, no se puede agregar más contenido.\n";
    }
}

void bloque::procesarArchivoRegistro(const char* ruta) {
    ifstream archivo(ruta);
    if (!archivo) {
        cout << "No se pudo abrir archivo de registro: " << ruta << endl;
        return;
    }

    char linea[1024];
    while (archivo.getline(linea, sizeof(linea))) {
        agregarAlBloque(linea);
    }

    archivo.close();
}

void bloque::crearBloque(const char* archivoRutas) {
    memset(bloqueB, 0, sizeof(bloqueB));

    ifstream archivo(archivoRutas);
    if (!archivo) {
        cout << "No se pudo abrir el archivo de rutas.\n";
        return;
    }

    char linea[1024];
    while (archivo.getline(linea, sizeof(linea))) {
        char* separador = strchr(linea, '#');
        if (separador) {
            const char* ruta = separador + 1;
            procesarArchivoRegistro(ruta);
        }
    }

    archivo.close();
}

void bloque::mostrarBloque() {
    cout << "Contenido del bloque:\n";

    for (int i = 0; bloqueB[i] != '\0'; ++i) {
        if (bloqueB[i] == '|')
            cout << '\n';
        else
            cout << bloqueB[i];
    }

    cout << endl;
}

const char* bloque::obtenerBloque() {
    return bloqueB;
}
bool bloque::insertarLinea(const char* linea) {
    int lenActual = strlen(bloqueB);
    int lenLinea = strlen(linea);
    if (lenActual + lenLinea + 2 < sizeof(bloqueB)) {  // +2 por '|'
        strcat(bloqueB, linea);
        strcat(bloqueB, "|");
        return true;
    } else {
        return false;  // No pudo insertar por falta de espacio
    }
}

void bloque::vaciarBloque() {
    memset(bloqueB, 0, sizeof(bloqueB));
}