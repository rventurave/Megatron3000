#ifndef ESQUEMA_H
#define ESQUEMA_H
#include <iostream>
using namespace std;
class esquema
{
public:
    void creaEsquema();
    bool existeTabla(char *nombreTabla);
    int countBytes(char *nombreTabla);
    char* extraerEsquema(char* nombreTabla);
    void generarEsquema(string rutaCsv, string nombreDataset);
    bool esEntero(char *s);
    bool esFloat(char *s);
    void limpiarCampo(char *campo);
    bool validar(char *esquema,  string linea);
    string extraerIdentificadores(string esquema);
    char * formatearLinea(const char * linea, char * esquema);
};

#endif