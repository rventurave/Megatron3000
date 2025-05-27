#include "../include/storage/headFile.h"

using namespace std;
namespace fs = filesystem;

/*void headFile::createHeadFile(int numPlato, int numSuperficie, int numPistasPorPlato, int numSectorPorPista)
{
    string pahtSector = "../../pahts";

    if (!fs::exists(pahtSector))
    {
        if (fs::create_directory(pahtSector))
        {
            cout << "Se creó la carpeta.\n";
        }
        else
        {
            cout << "No se pudo crear la carpeta.\n";
        }
    }
    else
    {
        cout << "Ya existe esa carpeta.\n";
    }

    string archivoSectoresPath = pahtSector + "/pahtSectores.txt";
    ofstream archivoPahts(archivoSectoresPath);

    for (int d = 0; d < numSectorPorPista; d++)
    {
        for (int c = 0; c < numPistasPorPlato; c++)
        {
            for (int b = 0; b < numSuperficie; b++)
            {
                for (int a = 0; a < numPlato; a++)
                {
                    string archivoSector = "../../disk/plato" + to_string(a + 1) +
                                           "/superfice" + to_string(b + 1) +
                                           "/pistas" + to_string(c + 1) +
                                           "/sector" + to_string(d + 1) + ".txt";
                    ofstream archivo(archivoSector);

                    archivoPahts << "0#" << archivoSector << "\n";
                }
            }
        }
    }

    archivoPahts.close();

    string sectoresDesignados = pahtSector + "/sectoresDesignados.txt";
    ofstream archivoSectoresDesignados(sectoresDesignados);
    archivoSectoresDesignados.close();
}
*/
void headFile::createHeadFile(int numPlato, int numSuperficie, int numPistasPorPlato, int numSectorPorPista, int sectorPorBloque)
{
    string pathBloques = "../../bloques";

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

    for (int pista = 1; pista <= numPistasPorPlato; ++pista)
    {
        for (int plato = 1; plato <= numPlato; ++plato)
        {
            for (int superficie = 1; superficie <= numSuperficie; ++superficie)
            {
                int sectoresAsignados = 0;
                int sectoresEnPista = numSectorPorPista;
                bool esImpar = (sectoresEnPista % 2 != 0);
                int cont = 0;
                while (sectoresAsignados < sectoresEnPista)
                {
                    int sectoresEnEsteBloque = sectorPorBloque;

                    if (sectoresAsignados + sectoresEnEsteBloque > sectoresEnPista)
                    {
                        sectoresEnEsteBloque = sectoresEnPista - sectoresAsignados;
                    }

                    // Si es impar y es el último bloque de la pista
                    if (esImpar && sectoresAsignados + sectoresEnEsteBloque == sectoresEnPista)
                    {
                        sectoresEnEsteBloque = max(sectoresEnEsteBloque - 1, 1);
                    }

                    cont++;
                    string nombreBloque = pathBloques + "/"+ to_string(cont)+ "bloque" + to_string(bloqueID++) + ".txt";
                    ofstream archivoBloque(nombreBloque);

                    for (int i = 0; i < sectoresEnEsteBloque; ++i)
                    {
                        int sector = sectoresAsignados + i + 1;
                        archivoBloque << "0#../../disk/plato" << plato
                                      << "/superficie" << superficie
                                      << "/pista" << pista
                                      << "/sector" << sector <<".txt"<< '\n';
                    }

                    archivoBloque.close();
                    sectoresAsignados += sectoresEnEsteBloque;
                }
            }
        }
    }

    cout << "Mapeo completado. Total bloques creados: " << (bloqueID - 1) << endl;
}
headFile::headFile()
{
    bloque = "../../bloques/mapaBloque.txt";
    pahtBloquesAsignados = "../../bloques/blosquesAsignados.txt";
}
string headFile::getPahtBloque()
{
    return bloque;
}
string headFile::getPahtBloquesAsignado()
{
    return pahtBloquesAsignados;
}

pair<int, int> headFile::extraerGrupoYNumero(string& nombre) {

    std::regex re(R"((\d+)bloque(\d+)\.txt)");
    std::smatch match;
    if (std::regex_match(nombre, match, re)) {
        int grupo = std::stoi(match[1]);
        int numero = std::stoi(match[2]);
        return {grupo, numero};
    }
    return {-1, -1};
}
void headFile::mapearBloques() {
    string path = "../../bloques";
    vector<std::string> resultado;
    map<int, std::vector<pair<int,string>>> grupos;

    for (const auto& entry : fs::directory_iterator(path)) {
        string nombre = entry.path().filename().string();
        auto [grupo, numero] = extraerGrupoYNumero(nombre);
        if (grupo != -1) {
            grupos[grupo].emplace_back(numero, nombre);
        }
    }

    resultado.clear();

    for (auto& [grupo, archivos] : grupos) {
        sort(archivos.begin(), archivos.end()); // ordena por número final
        for (const auto& [numero, nombre] : archivos) {
            resultado.push_back(nombre);
        }
    }
    ofstream salida(bloque, ios::app);

    for (const auto& nombre : resultado) {
        salida <<"0#../../bloques/" <<nombre << '\n';
    }

    salida.close();
    ofstream archivoBloquesAsignados(pahtBloquesAsignados);
    archivoBloquesAsignados.close();
}
string headFile::asignarBloque(string tabla)
{
    fstream archivo(bloque, ios::in | ios::out);
    string linea;
    streampos pos;

    fstream archivo2(pahtBloquesAsignados, ios::app);

    while (!archivo.eof())
    {
        pos = archivo.tellg();
        getline(archivo, linea);

        if (linea.rfind("0#", 0) == 0)
        {
            string ruta = linea.substr(2);
            string nuevaLinea = "1#" + ruta + "\n";

            archivo.seekp(pos);
            archivo << nuevaLinea;
            archivo2 << tabla + "#" + ruta + "\n";

            archivo.close();
            archivo2.close();
            return ruta;
        }
    }
    archivo.close();
    archivo2.close();
    return "no";
}
string headFile::asignarSector(string bloque)
{
    fstream archivo(bloque, ios::in | ios::out);
    string linea;
    streampos pos;

    while (!archivo.eof())
    {
        pos = archivo.tellg();
        getline(archivo, linea);

        if (linea.rfind("0#", 0) == 0)
        {
            string ruta = linea.substr(2);
            string nuevaLinea = "1#" + ruta + "\n";

            archivo.seekp(pos);
            archivo << nuevaLinea;

            archivo.close();
            return ruta;
        }
    }
    archivo.close();
    return "no";
}