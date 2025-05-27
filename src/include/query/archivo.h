#ifndef ARCHIVO_H
#define ARCHIVO_H
#include <iostream>
using namespace std;
class archivo
{
    public:
    int contarLineas(string archivoTXT);
    string separar(string rutaCSV);
};
#endif