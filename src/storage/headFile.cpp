#include "../include/storage/headFile.h"
#include <ios>
using namespace std;
namespace fs = filesystem;

void headFile::createHeadFile(int numPlato, int numSuperficie, int numPistasPorPlato, int numSectorPorPista, int sectorPorBloque)
{
    string pathBloques = "../../metaData";

    if (!fs::exists(pathBloques))
    {
        if (fs::create_directory(pathBloques))
        {
            cout << "Se creó la carpeta.\n";
        }
        else
        {
            cout << "No se pudo crear la carpeta.\n";
            return;
        }
    }
    else
    {
        cout << "Ya existe esa carpeta.\n";
    }

    int bloqueID = 1;
    int sectoresEnBloque = 0;
    ofstream archivoBloque;

    for (int sector = 1; sector <= numSectorPorPista; ++sector)
    {
        for (int pista = 1; pista <= numPistasPorPlato; ++pista)
        {
            for (int plato = 1; plato <= numPlato; ++plato)
            {
                for (int superficie = 1; superficie <= numSuperficie; ++superficie)
                {
                    // Si es el primer sector de un nuevo bloque, abrir el archivo
                    if (sectoresEnBloque == 0)
                    {
                        string nombreBloque = pathBloques + "/bloque" + to_string(bloqueID) + ".txt";
                        archivoBloque.open(nombreBloque);
                    }

                    // Escribir sector en el bloque actual
                    archivoBloque << "0#../../disk/plato" << plato
                                  << "/superficie" << superficie
                                  << "/pista" << pista
                                  << "/sector" << sector << ".txt" << '\n';

                    sectoresEnBloque++;

                    // Si se llegó al límite, cerrar archivo y preparar el siguiente bloque
                    if (sectoresEnBloque == sectorPorBloque)
                    {
                        archivoBloque.close();
                        sectoresEnBloque = 0;
                        bloqueID++;
                    }
                }
            }
        }
    }

    // Si el último archivo quedó abierto (bloque incompleto), cerrarlo
    if (archivoBloque.is_open())
    {
        archivoBloque.close();
    }

    cout << "Mapeo completado. Total bloques creados: " << bloqueID - (sectoresEnBloque == 0 ? 1 : 0) << endl;
}

headFile::headFile()
{
    bloque = "../../metaData/mapaBloque.txt";
    pahtBloquesAsignados = "../../metaData/blosquesAsignados.txt";
}
string headFile::getPahtBloque()
{
    return bloque;
}
string headFile::getPahtBloquesAsignado()
{
    return pahtBloquesAsignados;
}

pair<int, int> headFile::extraerGrupoYNumero(string &nombre)
{
    regex re(R"(bloque(\d+)\.txt)");
    smatch match;
    if (regex_match(nombre, match, re))
    {
        int numero = stoi(match[1]);
        return {0, numero};
    }
    return {-1, -1};
}
void headFile::mapearBloques()
{
    string path = "../../metaData";
    vector<pair<int, string>> bloquesOrdenados;

    for (const auto &entry : fs::directory_iterator(path))
    {
        string nombre = entry.path().filename().string();
        auto [_, numero] = extraerGrupoYNumero(nombre);
        if (numero != -1)
        {
            bloquesOrdenados.emplace_back(numero, nombre);
        }
    }

    sort(bloquesOrdenados.begin(), bloquesOrdenados.end());

    ofstream salida(bloque, ios::app);
    for (const auto &[numero, nombre] : bloquesOrdenados)
    {
        salida << "0#../../metaData/" << nombre << '\n';
    }
    salida.close();
    ofstream archivoBloquesAsignados(pahtBloquesAsignados);
    archivoBloquesAsignados.close();
}

string headFile::asignarBloque(string tabla)
{
    fstream archivo(bloque, ios::in);
    if (!archivo.is_open()) return "no";

    vector<string> lineas;
    string linea;
    string ruta = "no";
    bool reemplazado = false;

    // Leer todas las líneas y modificar la primera que empiece con "0#"
    while (getline(archivo, linea))
    {
        if (!reemplazado && linea.rfind("0#", 0) == 0)
        {
            ruta = linea.substr(2);  // Extrae la ruta
            lineas.push_back("1#" + ruta);  // Reemplaza
            reemplazado = true;
        }
        else
        {
            lineas.push_back(linea);  // Guarda sin cambios
        }
    }
    archivo.close();

    // Si no se encontró ningún bloque disponible
    if (!reemplazado) return "no";

    // Escribir todo de nuevo con los cambios
    fstream archivoOut(bloque, ios::out | ios::trunc);
    for (const auto& l : lineas)
    {
        archivoOut << l << "\n";
    }
    archivoOut.close();

    // Registrar en el archivo de asignaciones
    fstream archivo2(pahtBloquesAsignados, ios::app);
    archivo2 << tabla + "#" + ruta + "\n";
    archivo2.close();

    return ruta;
}


string headFile::asignarSector(string bloque1)
{
    fstream archivo(bloque1, ios::in);
    if (!archivo.is_open()) {
        return "error abriendo archivo";
    }

    vector<string> lineas;
    string linea;
    string ruta = "no";
    bool reemplazado = false;

    // Leer todas las líneas y buscar la primera disponible
    while (getline(archivo, linea))
    {
        if (!reemplazado && linea.rfind("0#", 0) == 0)
        {
            ruta = linea.substr(2); // Extrae la ruta
            lineas.push_back("1#" + ruta); // Marca como ocupado
            reemplazado = true;
        }
        else
        {
            lineas.push_back(linea); // Guarda sin cambios
        }
    }
    archivo.close();

    if (!reemplazado) return "no";

    // Reescribir el archivo con los cambios
    fstream archivoOut(bloque1, ios::out | ios::trunc);
    for (const auto& l : lineas)
    {
        archivoOut << l << "\n";
    }
    archivoOut.close();

    return ruta;
}
