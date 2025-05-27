#ifndef CONTROL_H
#define CONTROL_H
#include <iostream>
#include <cmath>
#include <cstdio>
#include "../include/storage/disk.h"
#include "../include/query/query.h"
#include "../include/query/esquema.h"
#include "../include/storage/headFile.h"
#include "../include/query/archivo.h"
using namespace std;

class control
{
    private:
    disk _disk;
    esquema _esquema;
    query _query;
    headFile _headFile;
    archivo _archivo;
    public:
    void menu();
    void insertArchivoCSV();
    void mostrarTabla(string nombreTabla);
    void mostrarWhere(string nombreTabla, string nombreColumna, string op, string valorBuscado);
};
#endif