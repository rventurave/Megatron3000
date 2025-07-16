#include <fstream>
#include <iostream>
#include "../include/query/archivo.h"
#include <string>

//objetivo: separar una línea de un archivo CSV en partes
// Input: string linea
// Output: string con partes separadas por '#'
// Autor: Ronald Ventura
string archivo::separar(string linea)
{
    string resultado;
    bool entreComillas = false;
    for (char c : linea)
    {
        if (c == '"')
        {
            entreComillas = !entreComillas;
        }
        else if (c == ',' && !entreComillas)
        {
            resultado += '#';
        }
        else
        {
            resultado += c;
        }
    }
    return resultado;
}


//objetivo: contar las líneas de un archivo de texto
// Input: string ruta del archivo cs
// Output: int con el número de líneas
// Autor: Ronald Ventura
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
