#include <fstream>
#include <iostream>
#include "../include/query/archivo.h"
#include <string>


string archivo::separar(string linea) {
    string resultado;
    bool entreComillas = false;
    for (char c : linea) {
        if (c == '"') {
            entreComillas = !entreComillas;
        } else if (c == ',' && !entreComillas) {
            resultado += '#';
        } else {
            resultado += c;
        }
    }
    return resultado;
}

int archivo::contarLineas(string ruta)
{
    ifstream archivo(ruta);

    int contador = 0;
    char c;

    while (archivo.get(c))
    {
        if (c == '\n')
        {
            contador++;
        }
    }
    archivo.close();
    return contador;
}
