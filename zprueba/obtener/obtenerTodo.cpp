#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

// Objetivo: Extrae el primer campo de la segunda ruta en cada bloque asignado
// Input: ruta al archivo de bloques (ej: "../../metaData/bloquesAsignados.txt")
// Output: vector con el primer campo entero de la segunda línea de cada bloque
// Autor: Ronald Ventura
vector<int> obtenerPrimerCampoDeSegundaRuta(const string& rutaBloquesAsignados) {
    vector<int> resultados;
    ifstream archivoBloques(rutaBloquesAsignados);
    
    if (!archivoBloques) {
        cerr << "No se pudo abrir el archivo de bloques asignados: " << rutaBloquesAsignados << endl;
        return resultados;
    }

    string lineaBloque;
    while (getline(archivoBloques, lineaBloque)) {
        size_t posHash = lineaBloque.find('#');
        if (posHash == string::npos) continue;

        string rutaBloque = lineaBloque.substr(posHash + 1);

        ifstream archivoBloque(rutaBloque);
        if (!archivoBloque) {
            cerr << "No se pudo abrir el archivo de bloque: " << rutaBloque << endl;
            continue;
        }

        // Saltar la primera línea
        string lineaSector;
        getline(archivoBloque, lineaSector);

        // Leer la segunda línea
        if (!getline(archivoBloque, lineaSector)) {
            cerr << "Bloque con menos de dos líneas: " << rutaBloque << endl;
            continue;
        }

        size_t posHashSector = lineaSector.find('#');
        if (posHashSector == string::npos) continue;

        string rutaSector = lineaSector.substr(posHashSector + 1);

        ifstream archivoSector(rutaSector);
        if (!archivoSector) {
            cerr << "No se pudo abrir archivo de sector: " << rutaSector << endl;
            continue;
        }

        string lineaDatos;
        if (getline(archivoSector, lineaDatos)) {
            // Extraer el primer campo numérico antes del primer '#'
            stringstream ss(lineaDatos);
            string primerCampo;
            if (getline(ss, primerCampo, '#')) {
                try {
                    int valor = stoi(primerCampo);
                    resultados.push_back(valor);
                } catch (...) {
                    cerr << "No se pudo convertir el campo a entero: " << primerCampo << endl;
                }
            }
        }

        archivoSector.close();
        archivoBloque.close();
    }

    archivoBloques.close();
    return resultados;
}
int main() {
    vector<int> campos = obtenerPrimerCampoDeSegundaRuta("../../metaData/blosquesAsignados.txt");

    for (int valor : campos) {
        cout << "Campo extraido: " << valor << endl;
    }

    return 0;
}
