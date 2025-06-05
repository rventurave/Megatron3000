#ifndef QUERY_H
#define QUERY_H
#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;
class query
{
    public:
    void creaTabla();
    int leerCampoNumerico(istream& in);
    void escribirCampoNumerico(ostream& out, int valor);
    bool insert(char* registro, int tamFijo, const char* ruta);
    bool eliminar(int posicionRegistro, int tamFijo, const char* ruta);
    void limpiarCampo(char* campo);

    bool where(char *nombreTabla, char* linea, char *esquemaTabla, char *campo, char *operador, char *valor);

};

#endif