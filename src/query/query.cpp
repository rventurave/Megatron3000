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

bool query::insert(char* registro, int tamFijo, const char* ruta) {
    std::fstream archivo(ruta, std::ios::in | std::ios::out | std::ios::binary);
    if (!archivo) return false;

    archivo.seekg(0);
    int capacidad = leerCampoNumerico(archivo);
    archivo.get();
    int posFinal = leerCampoNumerico(archivo);
    archivo.get();
    int posEliminado = leerCampoNumerico(archivo);
    archivo.get();

    int tamTotal = tamFijo + 1; // +1 por el salto de línea

    // Verificación de longitud real
    int longitudReal = strlen(registro);
    if (longitudReal > tamFijo) {
        std::cerr << "Error: El registro es más grande que el tamaño fijo permitido." << std::endl;
        std::cerr << "Registro recibido (" << longitudReal << " bytes): " << registro << std::endl;
        std::cerr << "Tamaño fijo permitido: " << tamFijo << std::endl;
        return false;
    }

    if (capacidad < tamTotal) return false;

    // Rellenar con espacios si es más corto
    char buffer[tamFijo];
    int i = 0;
    for (; i < longitudReal; ++i)
        buffer[i] = registro[i];
    for (; i < tamFijo; ++i)
        buffer[i] = ' ';

    std::streampos posInsertar;

    if (posEliminado != 0) {
        archivo.seekg(21 + posEliminado);
        int siguienteEliminado = leerCampoNumerico(archivo);
        archivo.seekp(21 + posEliminado);
        archivo.write(buffer, tamFijo);
        archivo.put('\n');
        posInsertar = posEliminado;
        posEliminado = siguienteEliminado;
    } else {
        posInsertar = posFinal;
        archivo.seekp(21 + posInsertar);
        archivo.write(buffer, tamFijo);
        archivo.put('\n');
        posFinal += tamTotal;
    }

    // Actualizar encabezado
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

void query::limpiarCampo(char* campo) {
    int ini = 0, fin = 0;
    while (campo[ini] && isspace(campo[ini])) ini++;
    while (campo[fin]) fin++;
    fin--;
    while (fin >= ini && isspace(campo[fin])) fin--;

    int len = 0;
    for (int i = ini; i <= fin; i++) {
        campo[len++] = campo[i];
    }
    campo[len] = '\0';
}
bool query::where(char *nombreTabla, char* linea, char *esquemaTabla, char *campo, char *operador, char *valor)
{
    int numCampos = 0;
    char nombres[50][64];
    char tipos[50][10];
    int tamaños[50];

    // Parsear esquema
    int i = 0;
    while (esquemaTabla[i] && esquemaTabla[i] != '#') i++;
    if (esquemaTabla[i] == '#') i++;

    while (esquemaTabla[i]) {
        // Nombre del campo
        int j = 0;
        while (esquemaTabla[i] && esquemaTabla[i] != '#')
            nombres[numCampos][j++] = esquemaTabla[i++];
        nombres[numCampos][j] = '\0';
        if (esquemaTabla[i] == '#') i++;

        // Tipo
        j = 0;
        while (esquemaTabla[i] && esquemaTabla[i] != '#')
            tipos[numCampos][j++] = esquemaTabla[i++];
        tipos[numCampos][j] = '\0';
        if (esquemaTabla[i] == '#') i++;

        // Tamaño
        int valorTmp = 0;
        while (esquemaTabla[i] && esquemaTabla[i] != '#') {
            if (isdigit(esquemaTabla[i]))
                valorTmp = valorTmp * 10 + (esquemaTabla[i] - '0');
            i++;
        }
        tamaños[numCampos] = valorTmp;
        if (esquemaTabla[i] == '#') i++;

        numCampos++;
    }

    // Índice del campo a evaluar
    int campoIndex = -1;
    for (int k = 0; k < numCampos; k++) {
        if (strcmp(nombres[k], campo) == 0) {
            campoIndex = k;
            break;
        }
    }
    if (campoIndex == -1)
        return false; // Campo no encontrado

    // Separar campos de la línea
    char campos[50][1024];
    int campoAct = 0, pos = 0;
    for (int j = 0; linea[j]; j++) {
        if (linea[j] == '#') {
            campos[campoAct][pos] = '\0';
            limpiarCampo(campos[campoAct]);  // suponiendo que esta función ya está definida
            campoAct++;
            pos = 0;
        } else {
            campos[campoAct][pos++] = linea[j];
        }
    }
    campos[campoAct][pos] = '\0';
    limpiarCampo(campos[campoAct]);
    campoAct++;

    // Evaluar condición
    char *dato = campos[campoIndex];
    char tipo = tipos[campoIndex][0];

    if (tipo == 'i') {
        int datoInt = atoi(dato);
        int valorInt = atoi(valor);
        if (strcmp(operador, "==") == 0) return datoInt == valorInt;
        else if (strcmp(operador, "!=") == 0) return datoInt != valorInt;
        else if (strcmp(operador, "<") == 0) return datoInt < valorInt;
        else if (strcmp(operador, "<=") == 0) return datoInt <= valorInt;
        else if (strcmp(operador, ">") == 0) return datoInt > valorInt;
        else if (strcmp(operador, ">=") == 0) return datoInt >= valorInt;
    } else if (tipo == 'f') {
        float datoFloat = atof(dato);
        float valorFloat = atof(valor);
        if (strcmp(operador, "==") == 0) return datoFloat == valorFloat;
        else if (strcmp(operador, "!=") == 0) return datoFloat != valorFloat;
        else if (strcmp(operador, "<") == 0) return datoFloat < valorFloat;
        else if (strcmp(operador, "<=") == 0) return datoFloat <= valorFloat;
        else if (strcmp(operador, ">") == 0) return datoFloat > valorFloat;
        else if (strcmp(operador, ">=") == 0) return datoFloat >= valorFloat;
    } else {
        if (strcmp(operador, "==") == 0) return strcmp(dato, valor) == 0;
        else if (strcmp(operador, "!=") == 0) return strcmp(dato, valor) != 0;
    }
    return false;
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
    std::streampos posEliminar = posicionRegistro;

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