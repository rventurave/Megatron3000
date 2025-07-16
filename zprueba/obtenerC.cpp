#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

std::vector<int> obtenerPrimerCampoPorLinea(const std::string& nombreArchivo) {
    std::vector<int> primeros;
    std::ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo.\n";
        return primeros;
    }

    std::string linea;
    std::getline(archivo, linea);
    while (std::getline(archivo, linea)) {
        std::stringstream ss(linea);
        std::string primerCampo;
        if (std::getline(ss, primerCampo, '#')) {
            if (!primerCampo.empty()) {
                primeros.push_back(std::stoi(primerCampo));
            }
        }
    }

    archivo.close();
    return primeros;
}
int main() {
    std::vector<int> resultado = obtenerPrimerCampoPorLinea("dato.txt");

    for (int numero : resultado) {
        std::cout << numero << " ";
    }

    return 0;
}
