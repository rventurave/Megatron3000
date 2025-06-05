#include <filesystem>
#include <fstream>
#include "../include/query/esquema.h"
using namespace std;
namespace fs = filesystem;

void esquema::creaEsquema()
{
    string nombreEsquema = "../../esquema";
    if (!fs::exists(nombreEsquema))
    {
        if (fs::create_directory(nombreEsquema))
        {
            cout << "Se creeo la capeta: ";
        }
        else
        {
            cout << "No se pudo crear";
        }
    }
    else
    {
        cout << "Ya existe esa carpeta";
    }
    string archivoEsquema = nombreEsquema + "/esquema.txt";
    ofstream archivo(archivoEsquema);
}
bool esquema::existeTabla(char *nombreTabla)
{
    ifstream archivo("../../esquema/esquema.txt");
    if (!archivo.is_open())
    {
        cerr << "No se pudo abrir el archivo";
        return false;
    }

    char linea[1024];
    while (archivo.getline(linea, sizeof(linea)))
    {
        int i = 0;
        while (nombreTabla[i] != '\0' && nombreTabla[i] == linea[i])
        {
            i++;
        }
        if (nombreTabla[i] == '\0' && linea[i] == '#')
        {
            return true;
        }
    }

    return false;
}
int esquema::countBytes(char *nombreTabla)
{
    ifstream archivo("../../esquema/esquema.txt");
    if (!archivo)
    {
        cerr << "No se pudo abrir el archivo.\n";
        return -1;
    }

    char linea[1024];
    while (archivo.getline(linea, sizeof(linea)))
    {
        int i = 0;
        while (nombreTabla[i] != '\0' && nombreTabla[i] == linea[i])
            i++;

        if (nombreTabla[i] == '\0' && linea[i] == '#')
        {
            int suma = 0;
            int j = i + 1;
            int campo = 0;
            char palabra[20];
            int k = 0;

            while (true)
            {
                char c = linea[j];

                if (c == '#' || c == '\0')
                {
                    palabra[k] = '\0';

                    if (campo == 2)
                    {
                        int num = 0;
                        int l = 0;
                        while (palabra[l] >= '0' && palabra[l] <= '9')
                        {
                            num = num * 10 + (palabra[l] - '0');
                            l++;
                        }
                        suma += num+1;
                    }

                    k = 0;
                    campo = (campo + 1) % 3;

                    if (c == '\0')
                        break;
                }
                else
                {
                    if (k < 19)
                        palabra[k++] = c;
                }

                j++;
            }

            return suma;
        }
    }

    return -1;
}
char *esquema::extraerEsquema(char *nombreTabla)
{
    const int MAX_LINEA = 4096;
    const int MAX_NOMBRE = 64;
    static char resultado[MAX_LINEA];
    resultado[0] = '\0';

    ifstream archivo("../../esquema/esquema.txt");

    char linea[MAX_LINEA];
    int lenNombre = 0;
    while (nombreTabla[lenNombre])
        lenNombre++;

    while (archivo.getline(linea, MAX_LINEA))
    {
        bool coincide = true;
        for (int i = 0; i < lenNombre; i++)
        {
            if (linea[i] != nombreTabla[i])
            {
                coincide = false;
                break;
            }
        }
        if (coincide && linea[lenNombre] == '#')
        {
            int i = 0;
            while (linea[i])
            {
                resultado[i] = linea[i];
                i++;
            }
            resultado[i] = '\0';
            return resultado;
        }
    }

    return resultado;
}

const int MAX_COLUMNAS = 50;
const int MAX_LINEA = 1024;

bool esquema::esEntero(char* s) {
    int i = 0;
    if (s[0] == '-' || s[0] == '+') i++;
    bool tieneDigito = false;
    while (s[i]) {
        if (!isdigit(s[i])) return false;
        tieneDigito = true;
        i++;
    }
    return tieneDigito;
}
bool esquema::esFloat(char* s) {
    int i = 0;
    if (s[0] == '-' || s[0] == '+') i++;
    bool tienePunto = false, tieneDigito = false;

    while (s[i]) {
        if (s[i] == '.') {
            if (tienePunto) return false;
            tienePunto = true;
        } else if (isdigit(s[i])) {
            tieneDigito = true;
        } else {
            return false;
        }
        i++;
    }

    return tienePunto || tieneDigito;
}
void esquema::limpiarCampo(char* campo) {
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

void esquema::generarEsquema(string archivoCSV, string nombreTabla) {
    static char resultado[4096];
    resultado[0] = '\0';

    ifstream archivo(archivoCSV);

    char linea[MAX_LINEA];
    char nombres[MAX_COLUMNAS][64];
    int numCampos = 0;

    // Leer encabezado
    archivo.getline(linea, MAX_LINEA);
    int i = 0, col = 0, pos = 0;
    while (linea[i]) {
        if (linea[i] == ',') {
            nombres[col][pos] = '\0';
            limpiarCampo(nombres[col]);
            col++;
            pos = 0;
        } else {
            nombres[col][pos++] = linea[i];
        }
        i++;
    }
    nombres[col][pos] = '\0';
    limpiarCampo(nombres[col]);
    numCampos = col + 1;

    int maxLen[MAX_COLUMNAS] = {0};
    int tipo[MAX_COLUMNAS]; // 0 = int, 1 = float, 2 = string
    for (int j = 0; j < numCampos; j++) tipo[j] = 0;

    while (archivo.getline(linea, MAX_LINEA)) {
        char campos[MAX_COLUMNAS][MAX_LINEA];
        int campoAct = 0, pos = 0;
        bool entreComillas = false;

        for (int j = 0; linea[j]; j++) {
            if (linea[j] == '"') {
                entreComillas = !entreComillas;
            } else if (linea[j] == ',' && !entreComillas) {
                campos[campoAct][pos] = '\0';
                limpiarCampo(campos[campoAct]);
                campoAct++;
                pos = 0;
            } else {
                campos[campoAct][pos++] = linea[j];
            }
        }
        campos[campoAct][pos] = '\0';
        limpiarCampo(campos[campoAct]);
        campoAct++;

        for (int j = 0; j < numCampos; j++) {
            int len = 0;
            while (campos[j][len]) len++;
            if (len > maxLen[j]) {
                maxLen[j] = len;
                if (tipo[j] == 2) maxLen[j] += 10; // margen extra para strings
            }

            if (campos[j][0] == '\0') continue;

            if (tipo[j] == 2) continue;
            if (esEntero(campos[j])) continue;
            else if (esFloat(campos[j])) {
                tipo[j] = max(tipo[j], 1);
            } else {
                tipo[j] = 2;
            }
        }
    }

    int posFinal = 0;
    for (int i = 0; nombreTabla[i]; i++) {
        resultado[posFinal++] = nombreTabla[i];
    }

    for (int j = 0; j < numCampos; j++) {
        resultado[posFinal++] = '#';

        for (int k = 0; nombres[j][k]; k++)
            resultado[posFinal++] = nombres[j][k];

        resultado[posFinal++] = '#';

        const char* tipoTexto = (tipo[j] == 0) ? "int" : (tipo[j] == 1) ? "float" : "string";
        for (int k = 0; tipoTexto[k]; k++)
            resultado[posFinal++] = tipoTexto[k];

        resultado[posFinal++] = '#';

        // Convertir maxLen[j] a texto
        int valor = maxLen[j];
        char buffer[10];
        int len = 0;
        if (valor == 0) {
            buffer[len++] = '0';
        } else {
            int temp = valor, digits = 0;
            while (temp > 0) {
                buffer[digits++] = '0' + (temp % 10);
                temp /= 10;
            }
            for (int d = digits - 1; d >= 0; d--) {
                buffer[len++] = buffer[d];
            }
        }
        for (int k = 0; k < len; k++) {
            resultado[posFinal++] = buffer[k];
        }
    }

    resultado[posFinal] = '\0';
    fstream esquemaFile("../../esquema/esquema.txt", ios::app);
    esquemaFile << resultado << endl;
    esquemaFile.close();
}
bool esquema::validar(char* esquema, string linea) {
    char nombreTabla[64];
    char tipos[50][7];
    int tamaños[50];
    int numCampos = 0;

    int i = 0, j = 0;
    while (esquema[i] && esquema[i] != '#')
        nombreTabla[j++] = esquema[i++];
    nombreTabla[j] = '\0';
    if (esquema[i] == '#') i++;

    while (esquema[i]) {
        while (esquema[i] && esquema[i] != '#') i++;
        if (esquema[i] == '#') i++;

        j = 0;
        while (esquema[i] && esquema[i] != '#')
            tipos[numCampos][j++] = esquema[i++];
        tipos[numCampos][j] = '\0';
        if (esquema[i] == '#') i++;

        int valor = 0;
        while (esquema[i] && esquema[i] != '#') {
            if (isdigit(esquema[i])) valor = valor * 10 + (esquema[i] - '0');
            i++;
        }
        tamaños[numCampos++] = valor;
        if (esquema[i] == '#') i++;
    }

    char campos[50][MAX_LINEA];
    int campoAct = 0, pos = 0;
    for (int k = 0; linea[k]; k++) {
        if (linea[k] == '#') {
            campos[campoAct][pos] = '\0';
            limpiarCampo(campos[campoAct]);
            campoAct++;
            pos = 0;
        } else {
            campos[campoAct][pos++] = linea[k];
        }
    }
    campos[campoAct][pos] = '\0';
    limpiarCampo(campos[campoAct]);
    campoAct++;

    if (campoAct != numCampos) {
        cout << "Falla: número de campos no coincide. Esperado " << numCampos << ", recibido " << campoAct << endl;
        return false;
    }

    for (int k = 0; k < numCampos; k++) {
        int len = 0;
        while (campos[k][len]) len++;

        if (len > tamaños[k]) {
            cout << "Falla: campo #" << k << " excede tamaño máximo (" << len << " > " << tamaños[k] << "): " << campos[k] << endl;
            return false;
        }

        if (campos[k][0] == '\0') continue;

        if (tipos[k][0] == 'i' && !esEntero(campos[k])) {
            cout << "Falla: campo #" << k << " no es entero válido: '" << campos[k] << "'" << endl;
            return false;
        }
        if (tipos[k][0] == 'f' && !esFloat(campos[k])) {
            cout << "Falla: campo #" << k << " no es float válido: '" << campos[k] << "'" << endl;
            return false;
        }
    }

    return true;
}

string esquema::extraerIdentificadores(string esquema) {
    stringstream ss(esquema);
    string token;
    string identificadores;
    getline(ss, token, '#');

    int campo = 0;
    while (getline(ss, token, '#')) {
        if (campo % 3 == 0) {
            identificadores += token + "#";
        }
        campo++;
    }
    if (!identificadores.empty()) {
        identificadores.pop_back();
    }

    return identificadores;
}

char *esquema::formatearLinea(const char *linea, char *esquema)
{
    static char resultado[1024];
    resultado[0] = '\0';

    char tipos[50][10];
    int tamaños[50];
    int numCampos = 0;
    int i = 0;

    while (esquema[i] && esquema[i] != '#')
        i++;
    if (esquema[i] == '#')
        i++;

    while (esquema[i])
    {
        // Saltar el nombre del campo
        while (esquema[i] && esquema[i] != '#')
            i++;
        if (esquema[i] == '#')
            i++;

        // Leer tipo
        int j = 0;
        while (esquema[i] && esquema[i] != '#')
        {
            tipos[numCampos][j++] = esquema[i++];
        }
        tipos[numCampos][j] = '\0';
        if (esquema[i] == '#')
            i++;

        // Leer tamaño
        int valor = 0;
        while (esquema[i] && esquema[i] != '#')
        {
            if (esquema[i] >= '0' && esquema[i] <= '9')
            {
                valor = valor * 10 + (esquema[i] - '0');
            }
            i++;
        }
        tamaños[numCampos++] = valor;
        if (esquema[i] == '#')
            i++;
    }

    // Separar los campos de la línea
    char campos[50][1024];
    int campoAct = 0, pos = 0;
    for (int k = 0; linea[k]; k++)
    {
        if (linea[k] == '#')
        {
            campos[campoAct][pos] = '\0';
            limpiarCampo(campos[campoAct]);
            campoAct++;
            pos = 0;
        }
        else
        {
            campos[campoAct][pos++] = linea[k];
        }
    }
    campos[campoAct][pos] = '\0';
    limpiarCampo(campos[campoAct]);
    campoAct++;

    // Formatear línea
    int r = 0;
    for (int k = 0; k < numCampos; k++)
    {
        if (k > 0)
            resultado[r++] = '#';

        int len = 0;
        while (campos[k][len])
            len++;

        for (int j = 0; j < len && j < tamaños[k]; j++)
        {
            resultado[r++] = campos[k][j];
        }

        for (int j = len; j < tamaños[k]; j++)
        {
            resultado[r++] = ' ';
        }
    }
    resultado[r] = '\0';
    return resultado;
}