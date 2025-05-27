#ifndef HEADFILE_H
#define HEADFILE_H
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <algorithm>
#include "../storage/disk.h"

using namespace std;
class headFile
{
    private:
    string bloque;
    string pahtBloquesAsignados;


    public:
    void createHeadFile(int numPlato, int numSuperficie, int numPistasPorPlato, int numSectorPorPista, int sectorPorBloque);
    headFile();
    
    string asignarBloque(string tabla);
    string asignarSector(string bloque);
    pair<int, int> extraerGrupoYNumero(string& nombre);
    void mapearBloques();

    string getPahtBloque();
    string getPahtBloquesAsignado();

};

#endif