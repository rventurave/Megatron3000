#ifndef ARCHIVO_H
#define ARCHIVO_H
#include <iostream>
#include <vector>
using namespace std;
class archivo
{
public:
    int contarLineas(string archivoTXT);
    string separar(string rutaCSV);

    int extraerKey(const std::string &rutaBloque);
    vector<string> extraerRutas(const string &rutaArchivo);
    vector<string> obtenerCamposDeTabla(const string &nombreTabla, const string &rutaEsquema);
};
#endif