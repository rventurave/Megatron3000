#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

int obtenerEnteroSegundaLinea(const std::string& rutaSector) {
    std::ifstream archivo(rutaSector);
    std::string linea;
    int lineaActual = 0;

    while (std::getline(archivo, linea)) {
        if (++lineaActual == 2) {
            size_t pos = linea.find('#');
            if (pos != std::string::npos) {
                std::string valorStr = linea.substr(0, pos);
                try {
                    return std::stoi(valorStr);
                } catch (...) {
                    return -1; // Valor inválido
                }
            }
        }
    }
    return -1; // No hay segunda línea o error
}

std::string extraerRutaDespuesDeHash(const std::string& linea) {
    size_t pos = linea.find('#');
    if (pos != std::string::npos)
        return linea.substr(pos + 1);
    return "";
}

std::vector<int> obtenerValoresDesdeArchivoPrincipal(const std::string& rutaArchivoPrincipal) {
    std::ifstream archivoPrincipal(rutaArchivoPrincipal);
    std::string linea;
    std::vector<int> valores;

    while (std::getline(archivoPrincipal, linea)) {
        std::string rutaMetadata = extraerRutaDespuesDeHash(linea);
        std::ifstream archivoMetadata(rutaMetadata);

        std::string lineaMeta;
        while (std::getline(archivoMetadata, lineaMeta)) {
            std::string rutaSector = extraerRutaDespuesDeHash(lineaMeta);
            int valor = obtenerEnteroSegundaLinea(rutaSector);
            if (valor != -1) {
                valores.push_back(valor);
            }
        }
    }

    return valores;
}
int main() {
    std::vector<int> resultado = obtenerValoresDesdeArchivoPrincipal("../metaData/bloquesAsignados.txt");

    for (int valor : resultado) {
        std::cout << valor << std::endl;
    }

    return 0;
}
