#include <iostream>
#include <filesystem>
#include <fstream>
#include <cmath>
#include "../include/storage/disk.h"
using namespace std;
namespace fs = std::filesystem;
disk::disk()
{
    plato = 4;
    pistasPorPlato = 4;
    superficie = 2;
    sectorPorPistas = 4;
    sizeSector = 4096;
    sizeBloque = sizeSector * 2;

    capacidad = superficie * plato * pistasPorPlato * sectorPorPistas * sizeSector;
    sectores = plato * pistasPorPlato * sectorPorPistas * superficie;
    sectoresPorBloque = sizeBloque / sizeSector;
    bloquesPorPista = static_cast<int>(ceil(static_cast<float>(sectorPorPistas) / sectoresPorBloque));
    bloquesTotales = bloquesPorPista * pistasPorPlato * superficie * plato;
    pistas = plato * pistasPorPlato * superficie;
    sectoresPorBloque = sizeBloque / sizeSector;
}

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

void disk::setCapacidad(int _capacidad)
{
    capacidad = capacidad - _capacidad;
}

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