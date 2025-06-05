#ifndef DISK_H
#define DISK_H

class disk
{
private:
    int plato;
    int pistasPorPlato;
    int superficie;
    int sectorPorPistas;
    int sizeSector;
    int sizeBloque;
    int capacidad;
    int sectores;
    int pistas;
    int sectoresPorBloque;
    float bloquesPorPista;
    float bloquesTotales;


public:
    disk();
    disk(int _plato, int _pistasPorPlato, int _sectorPorPistas, int _sizeSector, int _sizebloque);

    
    int getPlato();
    int getPistas();
    int getSuperficie();
    int getSectorPorPistas();
    int getSizeSector();
    int getCapacidad();
    int getSectores();
    int getPistasPorPlato();
    int getSizeBloque();
    int getSectoresPorBloque();
    int getBloquesPorPista();
    int getBloquesTotales();


    void createDisk();
    void setCapacidad(int _capacidad);
    void informacionDisk();
    void informacionCapacidad();
};
#endif