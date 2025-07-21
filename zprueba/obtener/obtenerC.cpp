#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int extraerPrimerValorSegundaLinea(const std::string& rutaBloque) {
    std::ifstream archivoBloque(rutaBloque);
    if (!archivoBloque.is_open()) {
        std::cerr << "No se pudo abrir el archivo de bloque: " << rutaBloque << std::endl;
        return -1;
    }

    std::string lineaRuta;
    if (!std::getline(archivoBloque, lineaRuta)) {
        std::cerr << "El archivo de bloque esta vacio." << std::endl;
        return -1;
    }
    archivoBloque.close();

    // Quitar prefijo hasta primer '#'
    size_t pos = lineaRuta.find('#');
    if (pos == std::string::npos) {
        std::cerr << "Formato incorrecto en bloque.txt" << std::endl;
        return -1;
    }

    std::string rutaDatos = lineaRuta.substr(pos + 1);

    std::ifstream archivoDatos(rutaDatos);
    if (!archivoDatos.is_open()) {
        std::cerr << "No se pudo abrir el archivo de datos: " << rutaDatos << std::endl;
        return -1;
    }

    std::string linea;
    
    // Ignorar la primera línea
    if (!std::getline(archivoDatos, linea)) {
        std::cerr << "El archivo de datos esta vacio." << std::endl;
        return -1;
    }

    // Leer la segunda línea
    if (!std::getline(archivoDatos, linea)) {
        std::cerr << "El archivo no tiene segunda línea." << std::endl;
        return -1;
    }

    archivoDatos.close();

    // Obtener el primer campo antes del primer '#'
    std::istringstream ss(linea);
    std::string primerCampo;
    if (!std::getline(ss, primerCampo, '#')) {
        std::cerr << "No se encontro el campo esperado en la segunda linea." << std::endl;
        return -1;
    }

    // Limpiar espacios en blanco del primer campo (ej: " 1 ")
    primerCampo.erase(0, primerCampo.find_first_not_of(" \t"));
    primerCampo.erase(primerCampo.find_last_not_of(" \t") + 1);

    try {
        return std::stoi(primerCampo);
    } catch (...) {
        std::cerr << "Error al convertir a entero: '" << primerCampo << "'" << std::endl;
        return -1;
    }
}
int main() {
    int valor = extraerPrimerValorSegundaLinea("../../metaData/bloque1.txt");
    std::cout << "Valor extraido: " << valor << std::endl;
    return 0;
}