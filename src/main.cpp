#include <iostream>
#include "../include/control/control.h"
using namespace std;
int main()
{
    cout << "% MEGATRON3000" << endl;
    cout << "    Welcome to MEGATRON 3000!" << endl;

    int tamBuffer;
    std::cout << "Ingrese el tamano del buffer (numero de frames): ";
    std::cin >> tamBuffer;

    control _control(tamBuffer);
    _control.menu();
    cout << "% " << endl;
    return 0;
}