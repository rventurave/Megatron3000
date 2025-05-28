#include "../include/query/query.h"
using namespace std;
namespace fs = filesystem;

void query::creaTabla()
{
    string relacion;
    cout << "Ingrese el nombre de la tabla: ";
    string tabla;
    cin >> tabla;
    relacion = tabla;

    int opc = 0;

    cout << "Ingrese el nombre del atributo: ";
    string atributo;
    cin >> atributo;

    cout << "Ingrese el tipo del atributo: ";
    string tipo;
    cin >> tipo;

    cout << "Ingrese el tamaño del atributo: ";
    int size;
    cin >> size;

    relacion += "#" + atributo + "#" + tipo + "#" + to_string(size);

    cout << "¿Desea agregar otro atributo?" << endl;
    cout << "0) No" << endl;
    cout << "1) Si" << endl;
    cin >> opc;

    while (opc == 1)
    {
        cout << "Ingrese el nombre del atributo: ";
        cin >> atributo;

        cout << "Ingrese el tipo del atributo: ";
        cin >> tipo;

        cout << "Ingrese el tamaño del atributo: ";
        cin >> size;

        relacion += "#" + atributo + "#" + tipo + "#" + to_string(size);

        cout << "¿Desea agregar otro atributo?" << endl;
        cout << "0) No" << endl;
        cout << "1) Si" << endl;
        cin >> opc;
    }

    relacion += "\n";

    string esquemaPath = "../../esquema/esquema.txt";
    ofstream esquemaFile(esquemaPath, ios::app);
    if (esquemaFile.is_open())
    {
        esquemaFile << relacion;
        esquemaFile.close();
        cout << "Tabla insertada en el esquema." << endl;
    }
    else
    {
        cerr << "Error al abrir el archivo del esquema." << endl;
    }
}
int query::leerCampoNumerico(std::istream& in) {
    char buffer[6];
    in.read(buffer, 6);
    int valor = 0;
    for (int i = 0; i < 6; ++i)
        valor = valor * 10 + (buffer[i] - '0');
    return valor;
}

void query::escribirCampoNumerico(std::ostream& out, int valor) {
    char buffer[6];
    for (int i = 5; i >= 0; --i) {
        buffer[i] = '0' + (valor % 10);
        valor /= 10;
    }
    out.write(buffer, 6);
}

bool query::insert(const char* registro, int tamFijo, const char* ruta) {
    std::fstream archivo(ruta, std::ios::in | std::ios::out | std::ios::binary);
    if (!archivo) return false;

    archivo.seekg(0);
    int capacidad = leerCampoNumerico(archivo);
    archivo.get();
    int posFinal = leerCampoNumerico(archivo);
    archivo.get();
    int posEliminado = leerCampoNumerico(archivo);
    archivo.get();

    int tamTotal = tamFijo + 1;
    if (capacidad < tamTotal) return false;

    char buffer[tamFijo];
    int i = 0;
    for (; i < tamFijo && registro[i] != '\0'; ++i)
        buffer[i] = registro[i];
    for (; i < tamFijo; ++i)
        buffer[i] = ' ';

    std::streampos posInsertar;

    if (posEliminado != 0) {
        archivo.seekg(20 + posEliminado);
        int siguienteEliminado = leerCampoNumerico(archivo);
        archivo.seekp(20 + posEliminado);
        archivo.write(buffer, tamFijo);
        archivo.put('\n');
        posInsertar = posEliminado;
        posEliminado = siguienteEliminado;
    } else {
        posInsertar = posFinal;
        archivo.seekp(20 + posInsertar);
        archivo.write(buffer, tamFijo);
        archivo.put('\n');
        posFinal += tamTotal;
    }

    capacidad -= tamTotal;
    archivo.seekp(0);
    escribirCampoNumerico(archivo, capacidad);
    archivo.put('#');
    escribirCampoNumerico(archivo, posFinal);
    archivo.put('#');
    escribirCampoNumerico(archivo, posEliminado);
    archivo.put('\n');

    archivo.close();
    return true;
}

bool query::eliminar(int posicionRegistro, int tamFijo, const char* ruta) {
    std::fstream archivo(ruta, std::ios::in | std::ios::out | std::ios::binary);
    if (!archivo) return false;

    archivo.seekg(0);
    int capacidad = leerCampoNumerico(archivo);
    archivo.get();
    int posFinal = leerCampoNumerico(archivo);
    archivo.get();
    int posEliminado = leerCampoNumerico(archivo);
    archivo.get();

    int tamTotal = tamFijo + 1;
    std::streampos posEliminar = 20 + posicionRegistro;

    archivo.seekp(posEliminar);
    escribirCampoNumerico(archivo, posEliminado);
    for (int i = 6; i < tamFijo; ++i)
        archivo.put(' ');
    archivo.put('\n');

    capacidad += tamTotal;
    posEliminado = posicionRegistro;

    archivo.seekp(0);
    escribirCampoNumerico(archivo, capacidad);
    archivo.put('#');
    escribirCampoNumerico(archivo, posFinal);
    archivo.put('#');
    escribirCampoNumerico(archivo, posEliminado);
    archivo.put('\n');

    archivo.close();
    return true;
}