#include "../include/storage/bloque.h"
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

// Objetivo: Constructor de bloque
// Input: Ninguno
// Output: Inicializa el bloque con contenido vacío
// Autor: Ronald Ventura
bloque::bloque() {
    memset(bloqueB, 0, sizeof(bloqueB));
}

// Objetivo: Agrega un separador de registros del bloque
// Input: contenido
// Output: Agrega la cadena al bloque, separada por '|'
// Autor: Ronald Ventura
void bloque::agregarAlBloque(const char* contenido) {
    int lenActual = strlen(bloqueB);
    int lenNuevo = strlen(contenido);
    if (lenActual + lenNuevo + 2 < 50000) {
        strcat(bloqueB, contenido);
        strcat(bloqueB, "|");
    } else {
        cout << "Bloque lleno, no se puede agregar mas contenido.\n";
    }
}

// Objetivo: copia el contenido de un archivo de registro al bloque
// Input: ruta - ruta del sector del disco
// Output: Agrega las líneas del archivo al bloque
// Autor: Ronald Ventura
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

// Objetivo: Crea un bloque a partir de la ruta del bloque
// Input: archivoRutas - ruta del bloque en disco
// Output: Crea el bloque con el contenido de los archivos de registro
// Autor: Ronald Ventura
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

// Objetivo: Muestra el contenido del bloque por consola
// Input: Ninguno
// Output: Imprime el contenido del bloque en formato legible
// Autor: Ronald Ventura
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

// Objetivo: Obtiene el contenido del bloque
// Input: Ninguno
// Output: Devuelve un puntero al contenido del bloque
// Autor: Ronald Ventura
const char* bloque::obtenerBloque() {
    return bloqueB;
}

//objetivo: Inserta una línea en el bloque
// Input: linea - línea a insertar
// Output: true si se insertó correctamente, false si no hay espacio
// Autor: Ronald Ventura
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

// Objetivo: Vacía el bloque, eliminando todo su contenido
// Input: Ninguno
// Output: El bloque queda vacío
// Autor: Ronald Ventura
void bloque::vaciarBloque() {
    memset(bloqueB, 0, sizeof(bloqueB));
}
void bloque::eliminarPrimeraLinea() {
    char* separador = strchr(bloqueB, '|');
    if (separador) {
        separador++; // Avanza después de '|'
        // Calcula cuántos caracteres quedan después de la primera línea
        size_t longitudRestante = strlen(separador);
        // Mueve la parte restante al inicio del buffer
        memmove(bloqueB, separador, longitudRestante + 1); // +1 para incluir '\0'
    } else {
        // No hay separador, por lo tanto no hay líneas que eliminar
        vaciarBloque(); // Por seguridad, vacía el bloque si es inválido
    }
}