#include "../src/include/hashExtendido/hashExtendido.h"

#include <limits> // Para limpiar cin

void mostrarMenu() {
    std::cout << "\n===== MENU HASH EXTENDIDO =====\n";
    std::cout << "1. Insertar clave-valor\n";
    std::cout << "2. Buscar clave\n";
    std::cout << "3. Eliminar clave\n";
    std::cout << "4. Mostrar estado del hash\n";
    std::cout << "5. Salir\n";
    std::cout << "Seleccione una opcion: ";
}

int main() {
    HashExtendido<std::string, int> hash(2); // Capacidad de cada bucket = 2
    int opcion;

    do {
        mostrarMenu();
        std::cin >> opcion;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpiar entrada

        switch (opcion) {
            case 1: {
                std::string clave;
                int valor;
                std::cout << "Ingrese clave (string): ";
                std::getline(std::cin, clave);
                std::cout << "Ingrese valor (int): ";
                std::cin >> valor;
                std::cin.ignore();
                hash.insertar(clave, valor);
                std::cout << "Insertado correctamente.\n";
                break;
            }
            case 2: {
                std::string clave;
                std::cout << "Ingrese clave a buscar: ";
                std::getline(std::cin, clave);
                int resultado = hash.buscar(clave);
                if (resultado != -1)
                    std::cout << "Valor asociado: " << resultado << "\n";
                else
                    std::cout << "Clave no encontrada.\n";
                break;
            }
            case 3: {
                std::string clave;
                std::cout << "Ingrese clave a eliminar: ";
                std::getline(std::cin, clave);
                if (hash.eliminar(clave))
                    std::cout << "Clave eliminada.\n";
                else
                    std::cout << "Clave no encontrada.\n";
                break;
            }
            case 4: {
                hash.mostrar();
                break;
            }
            case 5: {
                std::cout << "Saliendo...\n";
                break;
            }
            default:
                std::cout << "Opcion invalida. Intente de nuevo.\n";
        }
    } while (opcion != 5);

    return 0;
}
