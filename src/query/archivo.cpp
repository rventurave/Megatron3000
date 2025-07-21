#include <fstream>
#include <iostream>
#include <sstream>
#include "../include/query/archivo.h"
#include <string>

//objetivo: separar una línea de un archivo CSV en partes
// Input: string linea
// Output: string con partes separadas por '#'
// Autor: Ronald Ventura
string archivo::separar(string linea)
{
    string resultado;
    bool entreComillas = false;
    for (char c : linea)
    {
        if (c == '"')
        {
            entreComillas = !entreComillas;
        }
        else if (c == ',' && !entreComillas)
        {
            resultado += '#';
        }
        else
        {
            resultado += c;
        }
    }
    return resultado;
}


//objetivo: contar las líneas de un archivo de texto
// Input: string ruta del archivo cs
// Output: int con el número de líneas
// Autor: Ronald Ventura
int archivo::contarLineas(string ruta)
{
    ifstream archivo(ruta);

    int contador = 0;
    char c;

    while (archivo.get(c))
    {
        if (c == '\n')
        {
            contador++;
        }
    }
    archivo.close();
    return contador;
}


int archivo::extraerKey(const std::string& rutaBloque) {
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

vector<string> archivo::extraerRutas(const string& rutaArchivo) {
    vector<string> rutas;
    ifstream archivo(rutaArchivo);
    
    if (!archivo) {
        cerr << "No se pudo abrir el archivo: " << rutaArchivo << endl;
        return rutas;
    }

    string linea;
    while (getline(archivo, linea)) {
        size_t pos = linea.find('#');
        if (pos != string::npos && pos + 1 < linea.size()) {
            string ruta = linea.substr(pos + 1);
            rutas.push_back(ruta);
        }
    }

    archivo.close();
    return rutas;
}
vector<string> archivo::obtenerCamposDeTabla(const string& nombreTabla, const string& rutaEsquema) {
    vector<string> campos;
    ifstream archivo(rutaEsquema);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo de esquemas." << endl;
        return campos;
    }

    string linea;
    while (getline(archivo, linea)) {
        size_t pos = linea.find('#');
        if (pos == string::npos) continue;

        string nombre = linea.substr(0, pos);
        if (nombre == nombreTabla) {
            // Dividir el esquema en tokens
            vector<string> tokens;
            stringstream ss(linea);
            string token;
            while (getline(ss, token, '#')) {
                tokens.push_back(token);
            }

            // A partir del segundo elemento, cada 3 es un campo
            for (size_t i = 1; i + 2 < tokens.size(); i += 3) {
                campos.push_back(tokens[i]); // Solo el nombre del campo
            }

            break;
        }
    }

    archivo.close();
    return campos;
}
