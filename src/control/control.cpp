#include "../include/control/control.h"
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

            // Si el bloque está lleno o se alcanzó la cantidad de inserciones deseadas
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
            cout << "Registro inválido: " << lineaConvertida << endl;
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
void control::consultaWhere(char *nombreTabla)
{

    char *esquemaTabla = _esquema.extraerEsquema(nombreTabla);

    char campo[50], operador[5], valor[50], nombreRelacion[50];
    cout << "Campo a buscar: ";
    cin >> campo;
    cout << "Operador de comparación (=, !=, <, >, etc.): ";
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
                            _disk.setCapacidad(bytes);
                            cout << "Registro insertado correctamente: " << linea << endl;
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
                            }
                            else
                            {
                                _query.insert(linea, bytes, sector.c_str());
                                _disk.setCapacidad(bytes);
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
void control::eliminarR()
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
                            cout << "Posicion a eliminar essadffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff:" << posEliminado << endl;
                            _query.eliminar(posEliminado, bytes, ruta.c_str());
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
            else{
                break;
            }
        }
    }
    BloqueA.close();
}
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
        cout << "    0. Salir" << endl;
        cout << "Seleccione una opción: ";
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
            cout << "Ingrese el número de platos: ";
            cin >> plato;
            cout << "Ingrese el número de pistas por plato: ";
            cin >> pistasPorPlato;
            cout << "Ingrese el número de sectores por pista: ";
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
            consultaWhere(nombreTabla);
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
            eliminarR();
            break;
        }
        case 0:
            cout << "Saliendo del menú..." << endl;
            break;

        default:
            cout << "Opción no válida." << endl;
        }

    } while (opcion != 0);
}