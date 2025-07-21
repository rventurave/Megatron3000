#include <iostream>
#include "../include/control/control.h"
using namespace std;
int main()
{
    cout << "% MEGATRON3000" << endl;
    cout << "    Welcome to MEGATRON 3000!" << endl;

    int tamBuffer;
    std::cout << "Ingrese el tamano del  buffer: ";
    std::cin >> tamBuffer;
    std::cout << "Ingrese el orden del arbol B+: ";
    int ordenArbol;
    std::cin >> ordenArbol;
    std::cout << "Ingrese la capacidad del bucket: ";
    int capacidadBucket;
    std::cin >> capacidadBucket;

    control _control(tamBuffer, ordenArbol, capacidadBucket);
    _control.menu();
    cout << "% " << endl;
    return 0;
}