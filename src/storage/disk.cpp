#include <iostream>
#include <filesystem>
#include <fstream>
#include <cmath>
#include "../include/storage/disk.h"
using namespace std;
namespace fs = std::filesystem;

// Objetivo: Constructor por defecto del disco
// Input: Ninguno
// Output: Inicializa los parámetros del disco con valores predeterminados
// Autor: Ronald Ventura
disk::disk()
{
    plato = 2;
    pistasPorPlato = 2;
    superficie = 2;
    sectorPorPistas = 2;
    sizeSector = 1000;
    sizeBloque = sizeSector * 2;

    capacidad = superficie * plato * pistasPorPlato * sectorPorPistas * sizeSector;
    sectores = plato * pistasPorPlato * sectorPorPistas * superficie;
    sectoresPorBloque = sizeBloque / sizeSector;
    bloquesPorPista = static_cast<int>(ceil(static_cast<float>(sectorPorPistas) / sectoresPorBloque));
    bloquesTotales = bloquesPorPista * pistasPorPlato * superficie * plato;
    pistas = plato * pistasPorPlato * superficie;
    sectoresPorBloque = sizeBloque / sizeSector;
}

// Objetivo: Constructor del disco con parámetros personalizados
// Input: int _plato, int _pistasPorPlato, int _sectorPorPistas, int _sizeSector, int _sizebloque
// Output: Inicializa los parámetros del disco con los valores proporcionados
// Autor: Ronald Ventura
disk::disk(int _plato, int _pistasPorPlato, int _sectorPorPistas, int _sizeSector, int _sizebloque)
{
    plato = _plato;
    pistasPorPlato = _pistasPorPlato;
    superficie = 2;
    sectorPorPistas = _sectorPorPistas;
    sizeSector = _sizeSector;
    sizeBloque = sizeSector * _sizebloque;

    capacidad = superficie * plato * pistasPorPlato * sectorPorPistas * sizeSector;
    sectores = plato * pistasPorPlato * sectorPorPistas * superficie;
    pistas = plato * pistasPorPlato * superficie;
    sectoresPorBloque = sizeBloque / sizeSector;
    bloquesTotales = static_cast<int>(ceil(static_cast<float>(sectorPorPistas) / sectoresPorBloque));
    sectoresPorBloque = sizeBloque / sizeSector;
}

// Objetivo: Crea la estructura del disco en el sistema de archivos
// Input: Ninguno
// Output: Crea directorios y archivos para representar el disco y sus componentes
// Autor: Ronald Ventura
void disk::createDisk()
{
    string nombreDisk = "../../disk";
    if (!fs::exists(nombreDisk))
    {
        if (fs::create_directory(nombreDisk))
        {
            cout << "Se creeo la capeta: ";
        }
        else
        {
            cout << "No se pudo crear";
        }
    }
    else
    {
        cout << "Ya existe esa carpeta";
    }
    for (int a = 0; a < plato; a++)
    {
        string path = "../../disk/plato";
        string nombrePlato = path + to_string(a + 1);
        if (!fs::exists(nombrePlato))
        {
            if (fs::create_directory(nombrePlato))
            {
                cout << "Se creeo la capeta: ";
            }
            else
            {
                cout << "No se pudo crear";
            }
        }
        else
        {
            cout << "Ya existe esa carpeta";
        }
        for (int b = 0; b < superficie; b++)
        {
            string path = nombrePlato + "/superficie";
            string nombreSuperficie = path + to_string(b + 1);
            if (!fs::exists(nombreSuperficie))
            {
                if (fs::create_directory(nombreSuperficie))
                {
                    cout << "Se creeo la capeta: ";
                }
                else
                {
                    cout << "No se pudo crear";
                }
            }
            else
            {
                cout << "Ya existe esa carpeta";
            }
            for (int c = 0; c < pistasPorPlato; c++)
            {
                string path = nombreSuperficie + "/pista";
                string nombrePistasPorPlato = path + to_string(c + 1);
                if (!fs::exists(nombrePistasPorPlato))
                {
                    if (fs::create_directory(nombrePistasPorPlato))
                    {
                        cout << "Se creeo la capeta: ";
                    }
                    else
                    {
                        cout << "No se pudo crear";
                    }
                }
                else
                {
                    cout << "Ya existe esa carpeta";
                }
                for (int d = 0; d < sectorPorPistas; d++)
                {
                    string path = nombrePistasPorPlato;
                    string archivoSectores = path + "/sector" + to_string(d + 1) + ".txt";
                    fstream archivo(archivoSectores, ios::out | ios::app);
                    int sizeSector = getSizeSector();
                    archivo << setw(6) << setfill('0') << sizeSector << "#000000#000000\n";
                    archivo.close();
                }
            }
        }
    }
}

// Métodos para obtener información del disco
int disk::getPlato()
{
    return plato;
}
int disk::getPistas()
{
    return pistas;
}
int disk::getSuperficie()
{
    return superficie;
}
int disk::getSectorPorPistas()
{
    return sectorPorPistas;
}
int disk::getSizeSector()
{
    return sizeSector;
}
int disk::getCapacidad()
{
    return capacidad;
}
int disk::getSectores()
{
    return sectores;
}
int disk::getPistasPorPlato()
{
    return pistasPorPlato;
}
int disk::getSizeBloque()
{
    return sizeBloque;
}
int disk::getSectoresPorBloque()
{
    return sectoresPorBloque;
}
int disk::getBloquesPorPista()
{
    return bloquesPorPista;
}
int disk::getBloquesTotales()
{
    return bloquesTotales;
}

// Objetivo: Actualiza la capacidad del disco al insertar o elimnar registros
// Input: int _capacidad - tamaño del registro
// Output: Actualiza la capacidad del disco
// Autor: Ronald Ventura
void disk::setCapacidad(int _capacidad)
{
    if (capacidad > 0)
    {

        capacidad = capacidad - _capacidad;
    }
    else
    {
        cout << "ya no hay espacio" << endl;
    }
}


// Objetivo: Muestra la información del disco por consola
// Input: Ninguno
// Output: Imprime las características del disco, como capacidad, tamaño de bloque, sectores, etc.
// Autor: Ronald Ventura
void disk::informacionDisk()
{
    cout << "---------------------------------------------------------------------" << endl;
    cout << "CARACTERISTICAS DEL DISCO" << endl;
    cout << "---------------------------------------------------------------------" << endl;
    cout << "Capacidad del disco: " << getCapacidad() << endl;
    cout << "Capacidad de bloque: " << getSizeBloque() << endl;
    cout << "Capacidad de sector: " << getSizeSector() << endl;
    cout << "Pistas totales: " << getPistas() << endl;
    cout << "Platos totales: " << getPlato() << endl;
    cout << "Sectores totales: " << getSectores() << endl;
    cout << "---------------------------------------------------------------------" << endl;
}

// Objetivo: Muestra la información de capacidad del disco por consola
// Input: Ninguno
// Output: Imprime la capacidad total, capacidad libre y ocupada del disco
// Autor: Ronald Ventura
void disk::informacionCapacidad()
{
    cout << "---------------------------------------------------------------------" << endl;
    cout << "INFORMACION DE CAPACIDAD DEL DISCO" << endl;
    cout << "---------------------------------------------------------------------" << endl;

    int capacidad = getSuperficie() * getPlato() * getPistasPorPlato() * getSectorPorPistas() * getSizeSector();
    cout << "Capacidad del Disco: " << capacidad << endl;
    cout << "Capcacidad libre de disco: " << getCapacidad() << endl;
    cout << "Capacidad ocupada del disco: " << capacidad - getCapacidad() << endl;
    cout << "---------------------------------------------------------------------" << endl;
}