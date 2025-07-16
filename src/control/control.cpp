#include "../include/control/control.h"
#include <regex>
// Objetivo: Constructor del controlador del sistema
// Input: tamaño del buffer
// Output: instancia inicializada con estructuras de disco y buffer
// Autor: Ronald Ventura
control::control(int tamBuffer)
    : _lru(tamBuffer), _clock(tamBuffer) {}
// Objetivo: Insertar datos desde CSV al sistema
// Input: Ninguno (internamente usa archivos CSV)
// Output: Datos insertados y almacenados en disco
// Autor: Ronald Ventura
void control::insertCSV()
{
    char nombreTabla[50];
    cout << "Ingrese el nombre de la tabla: ";
    cin >> nombreTabla;

    string archivoCsv;
    cout << "Ingrese el nombre del archivo CSV: ";
    cin >> archivoCsv;
    archivoCsv = "../../archivos/" + archivoCsv + ".csv";

    int opcion;
    cout << "Cantidad de registros a insertar" << endl;
    cout << "0) Insertar todos los registros" << endl;
    cout << "n) Insertar n registros" << endl;
    cin >> opcion;

    if (!_esquema.existeTabla(nombreTabla))
    {
        _esquema.generarEsquema(archivoCsv, nombreTabla);
    }

    char *esquemaTabla = _esquema.extraerEsquema(nombreTabla);

    ifstream archivo(archivoCsv);
    string linea;
    getline(archivo, linea); // saltar encabezado CSV
    string bloque = _headFile.asignarBloque(nombreTabla);
    string sector = _headFile.asignarSector(bloque);
    int bytes = _esquema.countBytes(nombreTabla);
    int maxRegistrosPorBloque = _disk.getSizeBloque() / bytes;

    int contadorTotal = 0;
    int contadorBloque = 0;

    while (getline(archivo, linea))
    {
        string separado = _archivo.separar(linea);
        bool esValida = _esquema.validar(esquemaTabla, separado);
        char *lineaConvertida = _esquema.formatearLinea(separado.c_str(), esquemaTabla);

        if (esValida)
        {
            _bloque.insertarLinea(lineaConvertida);
            contadorBloque++;
            contadorTotal++;

            // Si el bloque esta lleno o se alcanzo la cantidad de inserciones deseadas
            if (contadorBloque == maxRegistrosPorBloque || (opcion != 0 && contadorTotal == opcion))
            {
                const char *contenido = _bloque.obtenerBloque();
                stringstream ss(contenido);
                string registro;

                while (getline(ss, registro, '|'))
                {
                    if (registro.empty())
                        continue;

                    bool insertado = _query.insert((char *)registro.c_str(), bytes, sector.c_str());
                    _disk.setCapacidad(bytes);
                    if (!insertado)
                    {
                        sector = _headFile.asignarSector(bloque);
                        if (sector == "no")
                        {
                            bloque = _headFile.asignarBloque(nombreTabla);
                            sector = _headFile.asignarSector(bloque);
                        }
                        _query.insert((char *)registro.c_str(), bytes, sector.c_str());
                    }
                    cout << "Registro insertado: " << registro << endl;
                }

                _bloque.vaciarBloque(); // vaciar bloque para nueva tanda
                contadorBloque = 0;

                if (opcion != 0 && contadorTotal == opcion)
                    break;
            }
        }
        else
        {
            cout << "Registro invalido: " << lineaConvertida << endl;
        }
    }

    // Insertar lo que quede en el bloque si hay contenido restante
    if (contadorBloque > 0)
    {
        const char *contenido = _bloque.obtenerBloque();
        stringstream ss(contenido);
        string registro;

        while (getline(ss, registro, '|'))
        {
            if (registro.empty())
                continue;

            bool insertado = _query.insert((char *)registro.c_str(), bytes, sector.c_str());
            _disk.setCapacidad(bytes);
            if (!insertado)
            {
                sector = _headFile.asignarSector(bloque);
                if (sector == "no")
                {
                    bloque = _headFile.asignarBloque(nombreTabla);
                    sector = _headFile.asignarSector(bloque);
                }
                _query.insert((char *)registro.c_str(), bytes, sector.c_str());
            }
            cout << "Registro insertado (restante): " << registro << endl;
        }

        _bloque.vaciarBloque();
    }

    archivo.close();
}

// Objetivo: Mostrar una tabla específica por nombre
// Input: nombre de la tabla
// Output: Contenido impreso por consola de todo los datos que estan en el disco
// Autor: Ronald Ventura
void control::mostrarTabla(string nombreTabla)

{
    string bloqueAsignado = _headFile.getPahtBloquesAsignado();

    ifstream BloqueA(bloqueAsignado);

    char *esquemaTabla = _esquema.extraerEsquema(const_cast<char *>(nombreTabla.c_str()));
    string cabecera = _esquema.extraerIdentificadores(esquemaTabla);

    string tabulado = cabecera;
    cout << cabecera << endl;
    cout << "-----------------------------------------------------------------------------------------------------------------------------------------------------" << endl;

    int contador = 0;
    string linea;
    while (std::getline(BloqueA, linea))
    {
        size_t pos = linea.find('#');

        if (pos == string::npos)
            continue;

        string nombre = linea.substr(0, pos);
        string ruta = linea.substr(pos + 1);

        if (nombre == nombreTabla)
        {
            ifstream archivo(ruta);

            string contenido;
            string ruta = contenido;
            while (getline(archivo, contenido))
            {
                ruta = contenido.substr(2);
                ifstream salida(ruta);
                string lineaSalida;
                getline(salida, lineaSalida); // omitir el header
                while (getline(salida, lineaSalida))
                {
                    char linea[4096];
                    int i = 0;
                    while (i < lineaSalida.size() && i < 4095)
                    {
                        linea[i] = lineaSalida[i];
                        i++;
                    }
                    linea[i] = '\0';
                    tabulado = _esquema.formatearLinea(linea, esquemaTabla);
                    cout << tabulado << endl;
                    cout << "-----------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
                    contador++;
                }
                salida.close();
            }
            archivo.close();
        }
    }
    cout << "Tamaño de archivo: " << contador * _esquema.countBytes(const_cast<char *>(nombreTabla.c_str())) << endl;
    BloqueA.close();
}

// Objetivo: Realizar una consulta con condicion WHERE
// Input: nombre de tabla como puntero char
// Output: Resultados impresos si cumplen condicion y guardados en disco con una relacion nueva
// Autor: Ronald Ventura
void control::consultaWhereAll(char *nombreTabla)
{

    char *esquemaTabla = _esquema.extraerEsquema(nombreTabla);

    char campo[50], operador[5], valor[50], nombreRelacion[50];
    cout << "Campo a buscar: ";
    cin >> campo;
    cout << "Operador de comparacion (=, !=, <, >, etc.): ";
    cin >> operador;
    cout << "Valor a comparar: ";
    cin >> valor;
    cout << "Nombre de la relacion: ";
    cin >> nombreRelacion;

    // asignando en donde guardar
    string bloque = _headFile.asignarBloque(nombreRelacion);
    string sector = _headFile.asignarSector(bloque);
    int bytes = _esquema.countBytes(nombreTabla);

    // revisando en todo los bloques que se encuentra los registros
    string bloqueAsignado = _headFile.getPahtBloquesAsignado();
    ifstream BloqueA(bloqueAsignado);

    string linea;
    while (std::getline(BloqueA, linea))
    {
        size_t pos = linea.find('#');

        if (pos == string::npos)
            continue;

        string nombre = linea.substr(0, pos);
        string ruta = linea.substr(pos + 1);

        if (nombre == nombreTabla)
        {
            ifstream archivo(ruta);

            string contenido;
            string ruta = contenido;
            while (getline(archivo, contenido))
            {
                ruta = contenido.substr(2);
                ifstream salida(ruta);
                string lineaSalida;
                getline(salida, lineaSalida); // omitir el header
                while (getline(salida, lineaSalida))
                {
                    char linea[4096];
                    int i = 0;
                    while (i < lineaSalida.size() && i < 4095)
                    {
                        linea[i] = lineaSalida[i];
                        i++;
                    }
                    linea[i] = '\0';
                    bool cumple = false;
                    cumple = _query.where(nombreTabla, linea, esquemaTabla, campo, operador, valor);

                    if (cumple)
                    {
                        bool insert = _query.insert(linea, bytes, sector.c_str());
                        if (insert)
                        {
                            cout << "Sectores donde estoy guardando" << endl;
                            cout << sector << endl;
                            _disk.setCapacidad(bytes);
                        }
                        else
                        {
                            sector = _headFile.asignarSector(bloque);
                            if (sector == "no")
                            {
                                bloque = _headFile.asignarBloque(nombreRelacion);
                                sector = _headFile.asignarSector(bloque);

                                _query.insert(linea, bytes, sector.c_str());
                                _disk.setCapacidad(bytes);
                                cout << "Sectores donde estoy guardando" << endl;
                                cout << sector << endl;
                            }
                            else
                            {
                                _query.insert(linea, bytes, sector.c_str());
                                _disk.setCapacidad(bytes);
                                cout << "Sectores donde estoy guardando" << endl;
                                cout << sector << endl;
                            }
                        }
                    }
                }
                salida.close();
            }
            archivo.close();
        }
    }
    ofstream esquema("../../esquema/esquema.txt", ios::app);
    char *ptr = esquemaTabla;
    while (*ptr && *ptr != '#')
    {
        ptr++;
    }
    esquema << nombreRelacion << ptr << endl;
    esquema.close();
}

// Objetivo: Eliminar un registro específico
// Input: Ninguno directo (usa seleccion del usuario)
// Output: Registro eliminado del disco
// Autor: Ronald Ventura
void control::eliminarRegistro()
{
    cout << "Ingrese el nombre de la tabla: ";
    char nombreTabla[20];
    cin >> nombreTabla;

    int bytes = _esquema.countBytes(nombreTabla);
    cout << "Cantidad de buytes de Registro: " << bytes << endl;

    string bloqueAsignado = _headFile.getPahtBloquesAsignado();
    ifstream BloqueA(bloqueAsignado);

    string linea;
    while (std::getline(BloqueA, linea))
    {
        size_t pos = linea.find('#');

        if (pos == string::npos)
            continue;

        string nombre = linea.substr(0, pos);
        string ruta = linea.substr(pos + 1);

        if (nombre == nombreTabla)
        {
            _bloque.crearBloque(ruta.c_str());
            _bloque.mostrarBloque();
            string Campo1;
            cout << " ingrese el Campo 1 a eliminar: ";
            cin >> Campo1;
            Campo1 = Campo1 + "#";
            if (Campo1 != "-1")
            {
                ifstream archivo(ruta);
                string contenido;
                string ruta = contenido;
                while (getline(archivo, contenido))
                {
                    ruta = contenido.substr(2);
                    ifstream salida(ruta);
                    string lineaSalida;
                    int posEliminado = 0;
                    while (getline(salida, lineaSalida))
                    {
                        if (lineaSalida.rfind(Campo1, 0) == 0)
                        {
                            _query.eliminar(posEliminado, bytes, ruta.c_str());
                            cout << "lugar donde se esta eliminando el registro" << endl;
                            cout << ruta << endl;
                        }
                        std::streampos posicion = archivo.tellg();
                        bool tieneSalto = true;
                        int total = lineaSalida.length() + (tieneSalto ? 1 : 0);
                        posEliminado = posEliminado + total;
                    }
                    salida.close();
                }
                archivo.close();
            }
            else
            {
                break;
            }
        }
    }
    BloqueA.close();
}

// Objetivo: Verificar si un bloque existe en el disco
// Input: ID de la pagina (pageID)
// Output: true si el bloque existe, false si no
// Autor: Ronald Ventura
bool control::verificarBloque(const std::string &pageID)
{
    std::ifstream archivo("../../metaData/mapaBloque.txt");
    std::string linea;
    std::string objetivo = "bloque" + pageID;

    while (std::getline(archivo, linea))
    {
        if (linea.find(objetivo) != std::string::npos)
        {
            return true; // Ya no importa si es '1' o '0'
        }
    }
    return false; // No encontrado
}

// Objetivo: Insertar registros desde un CSV a un bloque específico en el disco
// Input: pageID, nombre de la tabla, archivo CSV, opcion de cantidad de registros
// Output: Registros insertados en el disco, mostrando mensajes de exito o error
// Autor: Ronald Ventura
void control::insertaDisco(const string &pageID, char *nombreTabla, string archivoCsv, int opcion)
{
    string rutaBloqueUnico = "../../metaData/bloque" + pageID + ".txt";

    if (!_esquema.existeTabla(nombreTabla))
    {
        _esquema.generarEsquema(archivoCsv, nombreTabla);
    }

    char *esquemaTabla = _esquema.extraerEsquema(nombreTabla);

    ifstream archivo(archivoCsv);
    string linea;
    getline(archivo, linea); // saltar encabezado CSV

    int bytes = _esquema.countBytes(nombreTabla);
    int maxRegistrosPorBloque = _disk.getSizeBloque() / bytes;
    int limiteRegistros = (opcion == 0) ? maxRegistrosPorBloque : opcion;

    int contadorTotal = 0;
    _bloque.vaciarBloque();

    while (getline(archivo, linea) && contadorTotal < limiteRegistros)
    {
        string separado = _archivo.separar(linea);
        bool esValida = _esquema.validar(esquemaTabla, separado);
        char *lineaConvertida = _esquema.formatearLinea(separado.c_str(), esquemaTabla);

        if (esValida)
        {
            _bloque.insertarLinea(lineaConvertida);
            contadorTotal++;
        }
        else
        {
            cout << "Registro invalido: " << lineaConvertida << endl;
        }
    }

    const char *contenido = _bloque.obtenerBloque();
    stringstream ss(contenido);
    string registro;
    string ultimoSector = "";

    while (getline(ss, registro, '|'))
    {
        if (registro.empty())
            continue;

        bool insertado = false;
        ifstream archivoSectores(rutaBloqueUnico);
        string lineaSector;

        while (getline(archivoSectores, lineaSector))
        {
            size_t pos = lineaSector.find('#');
            if (pos == string::npos)
                continue;

            string rutaSector = lineaSector.substr(pos + 1);

            insertado = _query.insert((char *)registro.c_str(), bytes, rutaSector.c_str());
            if (insertado)
            {
                _disk.setCapacidad(bytes);
                if (rutaSector != ultimoSector)
                {
                    cout << "Insertando en sector: " << rutaSector << endl;
                    ultimoSector = rutaSector;
                }
                break;
            }
        }

        archivoSectores.close();

        if (!insertado)
        {
            cout << "\nBloque lleno. No se pudieron insertar mas registros.\n";
            break;
        }
    }

    _bloque.vaciarBloque();
    archivo.close();
}

// Objetivo: Eliminar un registro específico de un bloque en el disco
// Input: pageID, nombre de la tabla, índice del registro a eliminar
// Output: Registro eliminado del disco, mostrando mensajes de exito o error
// Autor: Ronald Ventura
void control::eliminarDisco(const std::string &pageID, char *nombreTabla, int indiceEliminar)
{

    int bytes = _esquema.countBytes(nombreTabla);
    cout << "Cantidad de bytes de Registro: " << bytes << endl;

    string rutaBloque = "../../metaData/bloque" + pageID + ".txt";
    _bloque.crearBloque(rutaBloque.c_str());

    ifstream archivo(rutaBloque);
    string lineaBloque;
    vector<pair<string, int>> registros; // <rutaSector, offset en bytes>

    int contadorGlobal = 0;

    while (getline(archivo, lineaBloque))
    {
        if (lineaBloque.length() < 3)
            continue;

        string rutaSector = lineaBloque.substr(2); // quitar el "0#" o similar
        ifstream archivoSector(rutaSector);
        string lineaRegistro;
        int offset = 0;
        bool encabezadoMostrado = false;
        bool esPrimeraLinea = true;

        while (getline(archivoSector, lineaRegistro))
        {
            if (esPrimeraLinea)
            {
                esPrimeraLinea = false;
                offset += lineaRegistro.length() + 1;
                continue; // saltar la primera línea
            }

            if (!encabezadoMostrado)
            {
                cout << "\nSector: " << rutaSector << endl;
                encabezadoMostrado = true;
            }

            cout << "  " << contadorGlobal << ": " << lineaRegistro << endl;
            registros.emplace_back(rutaSector, offset);

            offset += lineaRegistro.length() + 1; // contar salto de línea
            contadorGlobal++;
        }

        archivoSector.close();
    }

    archivo.close();

    if (registros.empty())
    {
        cout << "No hay registros para eliminar.\n";
        return;
    }

    if (indiceEliminar < 0 || indiceEliminar >= registros.size())
    {
        cout << "Cancelado o índice fuera de rango.\n";
        return;
    }

    string ruta = registros[indiceEliminar].first;
    int pos = registros[indiceEliminar].second;

    _query.eliminar(pos, bytes, ruta.c_str());
    cout << "Registro eliminado en: " << ruta << " posicion: " << pos << "\n";
}

// Objetivo: Insertar registros desde un CSV a un bloque que esta en memoria
// Input: pageID, bloque puntero, nombre de la tabla, archivo CSV, opcion de cantidad de registros
// Output: Registros insertados en el bloque, mostrando mensajes de exito o error
// Autor: Ronald Ventura
void control::insertaBloque(const string &pageID, bloque *bPtr, char *nombreTabla, string archivoCsv, int opcion)
{
    string rutaBloqueUnico = "../../metaData/bloque" + pageID + ".txt";

    // Si no existe el esquema, generarlo automaticamente
    if (!_esquema.existeTabla(nombreTabla))
    {
        _esquema.generarEsquema(archivoCsv, nombreTabla);
    }

    char *esquemaTabla = _esquema.extraerEsquema(nombreTabla);

    ifstream archivo(archivoCsv);
    if (!archivo.is_open())
    {
        cout << "No se pudo abrir el archivo CSV.\n";
        return;
    }

    string linea;
    getline(archivo, linea); // Omitir encabezado

    int bytes = _esquema.countBytes(nombreTabla);
    int maxRegistrosPorBloque = _disk.getSizeBloque() / bytes;
    int limiteRegistros = (opcion == 0) ? maxRegistrosPorBloque : opcion;

    int contadorTotal = 0;

    if (!bPtr)
        bPtr = &_bloque;
    bPtr->vaciarBloque(); // Limpiar la copia antes de insertar

    while (getline(archivo, linea) && contadorTotal < limiteRegistros)
    {
        string separado = _archivo.separar(linea);
        bool esValida = _esquema.validar(esquemaTabla, separado);
        char *lineaConvertida = _esquema.formatearLinea(separado.c_str(), esquemaTabla);

        if (esValida)
        {
            bPtr->insertarLinea(lineaConvertida);
            contadorTotal++;
        }
        else
        {
            cout << "Registro invalido: " << separado << endl;
        }
    }

    archivo.close();
}

// Objetivo: Eliminar un registro específico de un bloque que esta en memoria
// Input: pageID, bloque puntero, nombre de la tabla
// Output: Registro eliminado del bloque, mostrando mensajes de exito o error
// Autor: Ronald Ventura
int control::eliminarBloque(const std::string &pageID, bloque *bPtr, char *nombreTabla)
{
    int bytes = _esquema.countBytes(nombreTabla);

    if (bytes <= 0)
    {
        cerr << "Error: tamaño de registro invalido (" << bytes << " bytes).\n";
        return -1;
    }

    cout << "Tamaño de registro: " << bytes << " bytes\n";

    string rutaBloque = "../../metaData/bloque" + pageID + ".txt";

    if (!bPtr)
    {
        _bloque.crearBloque(rutaBloque.c_str());
        bPtr = &_bloque;
    }

    const char *contenido = bPtr->obtenerBloque();
    if (!contenido || strlen(contenido) == 0)
    {
        cerr << "Error: el contenido del bloque esta vacío o es nulo.\n";
        return -1;
    }

    std::regex regex_metadato("^\\d{6}#\\d{6}#\\d{6}$");
    stringstream ss(contenido);
    string registro;
    vector<string> registros;

    while (getline(ss, registro, '|'))
    {
        if (!registro.empty() && !regex_match(registro, regex_metadato))
        {
            registros.push_back(registro);
        }
    }

    if (registros.empty())
    {
        cerr << "No hay registros validos para eliminar.\n";
        return -1;
    }

    for (size_t i = 0; i < registros.size(); ++i)
    {
        cout << i << ": " << registros[i] << "\n\n";
    }

    int indice;
    cout << "Ingrese el numero de línea a eliminar (o -1 para cancelar): ";
    cin >> indice;

    if (indice < 0 || indice >= static_cast<int>(registros.size()))
    {
        cout << "Índice invalido o cancelado.\n";
        return -1;
    }

    try
    {
        registros[indice] = string(bytes, '#');
    }
    catch (const std::length_error &e)
    {
        cerr << "Error al reemplazar el registro: " << e.what() << "\n";
        return -1;
    }

    bPtr->vaciarBloque();
    for (const auto &reg : registros)
    {
        bPtr->insertarLinea(reg.c_str());
    }

    cout << "Registro marcado como eliminado.\n";
    return indice;
}

void control::consultaWherePage(
    char *nombreTabla,
    const char *rutaWhere,
    char *campo,
    char *operador,
    char *valor)
{
    char *esquemaTabla = _esquema.extraerEsquema(nombreTabla);

    // revisar en todos los bloques dónde están los registros
    string bloqueAsignado = _headFile.getPahtBloquesAsignado();
    ifstream BloqueA(bloqueAsignado);

    string linea;
    while (getline(BloqueA, linea))
    {
        size_t pos = linea.find('#');
        if (pos == string::npos)
            continue;

        string nombre = linea.substr(0, pos);
        string ruta = linea.substr(pos + 1);

        if (nombre == nombreTabla)
        {
            ifstream archivo(ruta);
            string contenido;
            while (getline(archivo, contenido))
            {
                string rutaArchivo = contenido.substr(2); // asumiendo formato ": ruta"
                ifstream salida(rutaArchivo);
                string lineaSalida;

                getline(salida, lineaSalida); // omitir header
                while (getline(salida, lineaSalida))
                {
                    char lineaBuffer[4096];
                    int i = 0;
                    while (i < lineaSalida.size() && i < 4095)
                    {
                        lineaBuffer[i] = lineaSalida[i];
                        i++;
                    }
                    lineaBuffer[i] = '\0';

                    bool cumple = _query.where(nombreTabla, lineaBuffer, esquemaTabla, campo, operador, valor);
                    if (cumple)
                    {
                        cout << lineaBuffer << endl;
                    }
                }
                salida.close();
            }
            archivo.close();
        }
    }
}

void control::simularBufferLRU()
{
    bloques.clear();
    bloques.resize(_lru.sizeFrame());

    int opcion;
    std::string pageID;
    _lru.printFrames();
    _lru.printRendimiendo();

    do
    {
        std::cout << "\n--- MENu LRU ---\n";
        std::cout << "1. Acceder a pagina\n";
        std::cout << "2. Despinear pagina\n";
        std::cout << "3. Mostrar bloque\n";
        std::cout << "4. Fijar (pin) una pagina\n";
        std::cout << "5. Salir\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        switch (opcion)
        {
        case 1:
        { // ACCEDER
            if (_lru.allPinned())
            {
                std::cout << "Todas las paginas estan PINNEADAS. Solo puedes DESPINEAR.\n";
                break;
            }

            char rw;
            std::cout << "Ingrese el numero de la pagina: ";
            std::cin >> pageID;

            std::cout << "¿Lectura o escritura? (L/W): ";
            std::cin >> rw;
            rw = toupper(rw);

            if (!verificarBloque(pageID))
            {
                std::cout << "\nLa pagina " << pageID << " no existe en el disco.\n";
                break;
            }

            int frameID = _lru.indexPageID(pageID);
            int pinValor = 0;

            if (frameID != -1)
            {
                pinValor = _lru.getPinCount(frameID); // <-- Usa el pin real del frame si ya está cargado
            }

            bool esMiss = _lru.accessPage(pageID, rw, pinValor);
            frameID = _lru.indexPageID(pageID); // Puede cambiar si fue un miss

            if (frameID == -1)
            {
                std::cout << "Error: la pagina no esta en ningun frame.\n";
                break;
            }

            std::string rutaBloque = "../../metaData/bloque" + pageID + ".txt";
            if (esMiss)
            {
                bloques.resize(_lru.sizeFrame());
                bloques[frameID].crearBloque(rutaBloque.c_str());
            }

            if (rw == 'W')
            {
                std::cout << "Contenido actual del bloque:\n";
                _bloque = bloques[frameID];

                int opcionEscritura;
                std::cout << "1) Insertar registro\n2) Eliminar registro\n";
                std::cin >> opcionEscritura;

                if (opcionEscritura == 1)
                {
                    char nombreTabla[50];
                    std::string archivoCsv;
                    int cantidad;

                    std::cout << "Ingrese el nombre de la tabla: ";
                    std::cin >> nombreTabla;
                    std::cout << "Ingrese el nombre del archivo CSV: ";
                    std::cin >> archivoCsv;
                    archivoCsv = "../../archivos/" + archivoCsv + ".csv";

                    std::cout << "Cantidad de registros a insertar:\n";
                    std::cout << "0) Insertar todos los registros\nn) Insertar n registros\n";
                    std::cin >> cantidad;

                    insertaBloque(pageID, &bloques[frameID], nombreTabla, archivoCsv, cantidad);
                    bloques[frameID].mostrarBloque();

                    char guardar;
                    std::cout << "¿Guardar cambios en disco? (s/n): ";
                    std::cin >> guardar;

                    if (tolower(guardar) == 's')
                    {
                        insertaDisco(pageID, nombreTabla, archivoCsv, cantidad);
                        bloques[frameID].vaciarBloque();
                        bloques[frameID].crearBloque(rutaBloque.c_str());
                    }
                    else
                    {
                        bloques[frameID] = _bloque;
                        _bloque.vaciarBloque();
                        std::cout << "Cambios descartados.\n";
                    }
                }
                else if (opcionEscritura == 2)
                {
                    char nombreTabla[50];
                    std::cout << "Ingrese el nombre de la tabla: ";
                    std::cin >> nombreTabla;

                    int indiceEliminar = eliminarBloque(pageID, &bloques[frameID], nombreTabla);
                    bloques[frameID].mostrarBloque();

                    char guardar;
                    std::cout << "¿Guardar cambios en disco? (s/n): ";
                    std::cin >> guardar;

                    if (tolower(guardar) == 's')
                    {
                        eliminarDisco(pageID, nombreTabla, indiceEliminar);
                        bloques[frameID].vaciarBloque();
                        bloques[frameID].crearBloque(rutaBloque.c_str());
                    }
                    else
                    {
                        bloques[frameID] = _bloque;
                        _bloque.vaciarBloque();
                        std::cout << "Cambios descartados.\n";
                    }
                }
            }
            else
            {
                std::cout << "Lectura de bloque " << pageID << ":\n";
                bloques[frameID].mostrarBloque();
                char nombreTabla[50], campo[50], operador[5], valor[50], nombreRelacion[50];
                cout << "Ingrese el nombre de la tabla: ";
                cin >> nombreTabla;
                cout << "Campo a buscar: ";
                cin >> campo;
                cout << "Operador de comparacion (=, !=, <, >, etc.): ";
                cin >> operador;
                cout << "Valor a comparar: ";
                cin >> valor;
                consultaWherePage(
                    nombreTabla,
                    rutaBloque.c_str(),
                    campo,
                    operador,
                    valor);
            }

            _lru.printFrames();
            _lru.printRendimiendo();
            _lru.guardado(pageID);
            break;
        }

        case 2:
        { // DESPINEAR
            std::cout << "ID de la pagina a despinear: ";
            std::cin >> pageID;
            _lru.despinear(pageID);
            _lru.printFrames();
            _lru.printRendimiendo();
            break;
        }

        case 3:
        { // MOSTRAR
            std::cout << "Ingrese el ID de la pagina a mostrar: ";
            std::cin >> pageID;

            int frameID = _lru.indexPageID(pageID);
            if (frameID == -1)
            {
                std::cout << "La pagina no esta en ningun frame.\n";
                break;
            }

            std::cout << "Contenido del bloque " << pageID << ":\n";
            bloques[frameID].mostrarBloque();
            break;
        }

        case 4:
        { // FIJAR
            std::cout << "Ingrese el ID de la pagina que desea fijar (pin): ";
            std::cin >> pageID;

            if (_lru.indexPageID(pageID) == -1)
            {
                std::cout << "La pagina " << pageID << " no esta actualmente en memoria.\n";
            }
            else
            {
                _lru.pinear(pageID);
                std::cout << "Pagina " << pageID << " fijada correctamente.\n";
            }

            _lru.printFrames();
            _lru.printRendimiendo();
            break;
        }

        case 5:
            std::cout << "Saliendo del simulador LRU.\n";
            break;

        default:
            std::cout << "Opcion invalida. Intente nuevamente.\n";
        }

    } while (opcion != 5);
}
void control::simularBufferClock()
{
    bloques.clear();
    bloques.resize(_clock.sizeFrame());

    int opcion;
    std::string pageID;
    _clock.printFrames();
    _clock.printRendimiento();

    do
    {
        std::cout << "\n--- MENu CLOCK ---\n";
        std::cout << "1. Acceder a pagina\n";
        std::cout << "2. Despinear pagina\n";
        std::cout << "3. Mostrar bloque\n";
        std::cout << "4. Fijar (pin) una pagina\n";
        std::cout << "5. Salir\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        switch (opcion)
        {
        case 1:
        { // ACCEDER
            if (_clock.allPinned())
            {
                std::cout << "Todas las paginas estan PINNEADAS. Solo puedes DESPINEAR.\n";
                break;
            }

            char rw;
            std::cout << "Ingrese el numero de la pagina: ";
            std::cin >> pageID;

            std::cout << "¿Lectura o escritura? (L/W): ";
            std::cin >> rw;
            rw = toupper(rw);

            if (!verificarBloque(pageID))
            {
                std::cout << "La pagina " << pageID << " no existe en el disco.\n";
                break;
            }

            int frameID = _clock.indexPageID(pageID);
            int pinValor = 0;

            if (frameID != -1)
            {
                pinValor = _clock.getPinCount(frameID); // <- valor actual del pin si ya está en memoria
            }

            bool esMiss = _clock.accessPage(pageID, rw, pinValor);
            frameID = _clock.indexPageID(pageID); // Puede cambiar si fue miss

            if (frameID == -1)
            {
                std::cout << "Error: la pagina no esta en ningun frame.\n";
                break;
            }

            std::string rutaBloque = "../../metaData/bloque" + pageID + ".txt";
            if (esMiss)
            {
                bloques.resize(_clock.sizeFrame());
                bloques[frameID].crearBloque(rutaBloque.c_str());
            }

            if (rw == 'W')
            {
                std::cout << "Contenido actual del bloque:\n";
                _bloque = bloques[frameID];

                int opcionEscritura;
                std::cout << "1) Insertar registro\n2) Eliminar registro\n";
                std::cin >> opcionEscritura;

                if (opcionEscritura == 1)
                {
                    char nombreTabla[50];
                    std::string archivoCsv;
                    int cantidad;

                    std::cout << "Ingrese el nombre de la tabla: ";
                    std::cin >> nombreTabla;
                    std::cout << "Ingrese el nombre del archivo CSV: ";
                    std::cin >> archivoCsv;
                    archivoCsv = "../../archivos/" + archivoCsv + ".csv";

                    std::cout << "Cantidad de registros a insertar\n";
                    std::cout << "0) Insertar todos los registros\nn) Insertar n registros\n";
                    std::cin >> cantidad;

                    insertaBloque(pageID, &bloques[frameID], nombreTabla, archivoCsv, cantidad);
                    bloques[frameID].mostrarBloque();

                    char guardar;
                    std::cout << "¿Guardar cambios en disco? (s/n): ";
                    std::cin >> guardar;

                    if (tolower(guardar) == 's')
                    {
                        insertaDisco(pageID, nombreTabla, archivoCsv, cantidad);
                        bloques[frameID].vaciarBloque();
                        bloques[frameID].crearBloque(rutaBloque.c_str());
                    }
                    else
                    {
                        bloques[frameID] = _bloque;
                        _bloque.vaciarBloque();
                    }
                }
                else if (opcionEscritura == 2)
                {
                    char nombreTabla[50];
                    std::cout << "Ingrese el nombre de la tabla: ";
                    std::cin >> nombreTabla;

                    int indiceEliminar = eliminarBloque(pageID, &bloques[frameID], nombreTabla);
                    bloques[frameID].mostrarBloque();

                    char guardar;
                    std::cout << "¿Guardar cambios en disco? (s/n): ";
                    std::cin >> guardar;

                    if (tolower(guardar) == 's')
                    {
                        eliminarDisco(pageID, nombreTabla, indiceEliminar);
                        bloques[frameID].vaciarBloque();
                        bloques[frameID].crearBloque(rutaBloque.c_str());
                    }
                    else
                    {
                        bloques[frameID] = _bloque;
                        _bloque.vaciarBloque();
                    }
                }
            }
            else
            { // LECTURA
                std::cout << "Lectura de bloque " << pageID << ":\n";
                bloques[frameID].mostrarBloque();
                char nombreTabla[50], campo[50], operador[5], valor[50], nombreRelacion[50];
                cout << "Ingrese el nombre de la tabla: ";
                cin >> nombreTabla;
                cout << "Campo a buscar: ";
                cin >> campo;
                cout << "Operador de comparacion (=, !=, <, >, etc.): ";
                cin >> operador;
                cout << "Valor a comparar: ";
                cin >> valor;
                consultaWherePage(
                    nombreTabla,
                    rutaBloque.c_str(),
                    campo,
                    operador,
                    valor);
            }

            _clock.printFrames();
            _clock.printRendimiento();
            _clock.guardado(pageID);
            break;
        }

        case 2:
        { // DESPINEAR
            std::cout << "Ingrese el ID de la pagina a despinear: ";
            std::cin >> pageID;
            _clock.despinear(pageID);
            _clock.printFrames();
            _clock.printRendimiento();
            break;
        }

        case 3:
        { // MOSTRAR
            std::cout << "Ingrese el ID de la pagina a mostrar: ";
            std::cin >> pageID;

            int frameID = _clock.indexPageID(pageID);
            if (frameID == -1)
            {
                std::cout << "La pagina no esta en ningun frame.\n";
                break;
            }

            std::cout << "Contenido del bloque " << pageID << ":\n";
            bloques[frameID].mostrarBloque();
            break;
        }

        case 4:
        { // FIJAR
            std::cout << "Ingrese el ID de la pagina que desea fijar (pin): ";
            std::cin >> pageID;

            if (_clock.indexPageID(pageID) == -1)
            {
                std::cout << "La pagina " << pageID << " no esta actualmente en memoria.\n";
            }
            else
            {
                _clock.pinear(pageID);
                std::cout << "Pagina " << pageID << " fijada correctamente.\n";
            }
            _clock.printFrames();
            _clock.printRendimiento();
            break;
        }

        case 5:
            std::cout << "Saliendo del simulador Clock.\n";
            break;

        default:
            std::cout << "Opcion invalida. Intente nuevamente.\n";
        }

    } while (opcion != 5);
}

// Objetivo: Mostrar menu principal al usuario
// Input: Ninguno
// Output: Interaccion por consola
// Autor: Ronald Ventura
void control::menu()
{
    int opcion;

    do
    {
        cout << endl;
        cout << "    1. Crear disco por defecto" << endl;
        cout << "    2. Crear disco personalizado" << endl;
        cout << "    3. Informacion de disco" << endl;
        cout << "    4. Crear tabla" << endl;
        cout << "    5. Insertar registro" << endl;
        cout << "    6. Mostrar tabla" << endl;
        cout << "    7. Capacidad del disco" << endl;
        cout << "    8. Consulta Where" << endl;
        cout << "    9. Mostrar contenido de un bloque" << endl;
        cout << "    10. Eliminar registro" << endl;
        cout << "    11. Buffer (LRU)" << endl;
        cout << "    12. Buffer (Clock)" << endl;
        cout << "    0. Salir" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion)
        {
        case 1:
        {
            _disk.createDisk();
            _headFile.createHeadFile(_disk.getPlato(), _disk.getSuperficie(), _disk.getPistasPorPlato(), _disk.getSectorPorPistas(), _disk.getSectoresPorBloque());
            _headFile.mapearBloques();
            _esquema.creaEsquema();
            _disk.informacionDisk();
            break;
        }
        case 2:
        {
            int plato, pistasPorPlato, sectorPorPistas, sizeSector, sizeBloque;
            cout << "Ingrese el numero de platos: ";
            cin >> plato;
            cout << "Ingrese el numero de pistas por plato: ";
            cin >> pistasPorPlato;
            cout << "Ingrese el numero de sectores por pista: ";
            cin >> sectorPorPistas;
            cout << "Ingrese el tamaño del sector: ";
            cin >> sizeSector;
            cout << "Ingrese la cantidad de sectores por bloque: ";
            cin >> sizeBloque;

            _disk = disk(plato, pistasPorPlato, sectorPorPistas, sizeSector, sizeBloque);
            _disk.createDisk();

            _headFile.createHeadFile(plato, 2, pistasPorPlato, sectorPorPistas, _disk.getSectoresPorBloque());
            _headFile.mapearBloques();

            _esquema.creaEsquema();
            _disk.informacionDisk();
            break;
        }

        case 3:
        {
            _disk.informacionDisk();
            break;
        }
        case 4:
        {
            _query.creaTabla();
            break;
        }
        case 5:
        {
            insertCSV();
            break;
        }
        case 6:
        {
            string nombreTabla;
            cout << "Ingrese el nombre de la tabla: ";
            cin >> nombreTabla;
            mostrarTabla(nombreTabla);
            break;
        }

        case 7:
        {
            _disk.informacionCapacidad();
            break;
        }
        case 8:
        {
            char nombreTabla[50];
            cout << "Nombre de la tabla: ";
            cin >> nombreTabla;
            consultaWhereAll(nombreTabla);
            break;
        }

        case 9:
        {
            cout << "inglese el bloque a ver el contenido" << endl;
            string ruta = "../../metaData/";
            string numBloque;
            cin >> numBloque;
            ruta = ruta + numBloque + ".txt";

            _bloque.crearBloque(ruta.c_str());
            _bloque.mostrarBloque();
            break;
        }
        case 10:
        {
            eliminarRegistro();
            break;
        }
        case 11:
        {
            simularBufferLRU();
            break;
        }
        case 12:
        {
            simularBufferClock();
            break;
        }

        case 0:
            cout << "Saliendo del menu..." << endl;
            break;

        default:
            cout << "Opcion no valida." << endl;
        }

    } while (opcion != 0);
}