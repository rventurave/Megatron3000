#ifndef QUERY_H
#define QUERY_H
#include <filesystem>
#include <iostream>
#include "../include/query/query.h"
#include "../include/query/esquema.h"
#include <fstream>
using namespace std;
class query
{
    private:
    esquema _esquema;
    public:
    void creaTabla();
    int leerCampoNumerico(istream& in);
    void escribirCampoNumerico(ostream& out, int valor);
    bool insert(const char* registro, int tamFijo, const char* ruta);
    bool eliminar(int posicionRegistro, int tamFijo, const char* ruta);
};

#endif