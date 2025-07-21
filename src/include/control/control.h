#ifndef CONTROL_H
#define CONTROL_H
#include <iostream>
#include <cmath>
#include <string>
#include <cstdio>
#include "../include/storage/disk.h"
#include "../include/query/query.h"
#include "../include/query/esquema.h"
#include "../include/storage/headFile.h"
#include "../include/query/archivo.h"
#include "../include/storage/bloque.h"
#include "../include/buffer/LRU.h"
#include "../include/buffer/clock.h"
#include "../include/bplusstree/bplusstree.h"
#include "../include/hashExtendido/hashExtendido.h"
using namespace std;

class control
{
private:
    std::vector<bloque> bloques;
    Clock _clock;
    disk _disk;
    esquema _esquema;
    query _query;
    headFile _headFile;
    archivo _archivo;
    bloque _bloque;
    LRU _lru;

    bPlussTree _bPlusTree;
    int _tamBuffer;
    int _ordenArbol;
    HashExtendido<int, int> _hashExtendido;
    int _capacidadBucket;

public:
    control(int tamBuffer, int ordenArbol, int capacidadBucket);
    void menu();
    void insertCSV();
    void mostrarTabla(string nombreTabla);
    void consultaWhereAll(char *nombreTabla);
    void consultaWherePage(
        char *nombreTabla,
        const char *rutaWhere,
        char *campo,
        char *operador,
        char *valor);
    void eliminarRegistro();

    void insertaDisco(const string &pageID, char *tabla, string rutaCSV, int opcion);
    void eliminarDisco(const std::string &pageID, char *nombreTabla, int indiceEliminar);

    void insertaBloque(const string &pageID, bloque *bPtr, char *nombreTabla, string archivoCsv, int opcion);
    int eliminarBloque(const std::string &pageID, bloque *bPtr, char *nombreTabla);

    bool verificarBloque(const std::string &pageID);

    void simularBufferLRU();
    void simularBufferClock();

    void generarBPlusTree();
    vector<string> extraerCampoPorIndice(const string& rutaTxt, int indiceCampo);
    void generarHash(char *nombreTabla);
    bool _insert(const string &pageID, char *nombreTabla, string archivoCsv, int opcion);
    void _insertaBloque(char *nombreTabla, string archivoCsv, int opcion);
    int _eliminarBloque(const std::string &claveEliminar, bloque *bPtr, char *nombreTabla);
    bool accederInsertar(char *nombreTabla, string archivoCsv, int cantidad);

    int extraerNumeroBloque(const std::string &ruta);
    void interfaz();
};
#endif