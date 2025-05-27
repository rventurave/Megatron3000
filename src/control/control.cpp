#include <iostream>
#include "../include/control/control.h"

using namespace std;

void control::insertArchivoCSV()
{
    char nombreTabla[50];
    cout << "Ingrese el nombre de la tabla: ";
    cin >> nombreTabla;

    string archivoCsv;
    cout << "Ingrese el nombre del archivo CSV: ";
    cin >> archivoCsv;
    archivoCsv = "../../archivos/csv/" + archivoCsv + ".csv";

    if (_esquema.existeTabla(nombreTabla) == false)
    {
        _esquema.generarEsquema(archivoCsv, nombreTabla);
    }
    char *esquemaTabla = _esquema.lineaEsquema(nombreTabla);

    ifstream archivo(archivoCsv);
    string linea;
    getline(archivo, linea);

    string bloque = _headFile.asignarBloque(nombreTabla);
    string sector = _headFile.asignarSector(bloque);
    int bytes = _esquema.countBytes(nombreTabla);

    cout<<"Tamaño de archivo es: "<<bytes*(_archivo.contarLineas(archivoCsv)-1)<<endl;
    
    while (getline(archivo, linea))
    {
        string lineaConvertida = _archivo.separar(linea);
        bool esValida = _esquema.validar(esquemaTabla, lineaConvertida);
        if (esValida)
        {
            bool insert = _query.insert(lineaConvertida.c_str(), bytes, sector.c_str());
            if (insert)
            {
                cout << "Registro insertado correctamente: " << lineaConvertida << endl;
            }
            else
            {
                sector = _headFile.asignarSector(bloque);
                if (sector == "no")
                {
                    bloque = _headFile.asignarBloque(nombreTabla);
                    sector = _headFile.asignarSector(bloque);

                    _query.insert(lineaConvertida.c_str(), bytes, sector.c_str());
                    _disk.setCapacidad(bytes);
                }
                else
                {
                    _query.insert(lineaConvertida.c_str(), bytes, sector.c_str());
                    _disk.setCapacidad(bytes);
                }
            }

        }
        else
        {
            cout << "Registro invalido: " << lineaConvertida << endl;
        }
    }
}

// select * from nombreTabla
void control::mostrarTabla(string nombreTabla)

{
    string bloqueAsignado = _headFile.getPahtBloquesAsignado();

    ifstream BloqueA(bloqueAsignado);

    char *esquemaTabla = _esquema.lineaEsquema(const_cast<char *>(nombreTabla.c_str()));
    string cabecera = _esquema.extraerIdentificadores(esquemaTabla);
    string tabulado = _esquema.tabulacion(cabecera);
    cout << tabulado << endl;
    cout << "----------------------------------------------------------------------------------------------" << endl;

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
                while (getline(salida, lineaSalida))
                {
                    tabulado = _esquema.tabulacion(lineaSalida);
                    cout << tabulado << endl;
                    cout << "----------------------------------------------------------------------------------------------" << endl;
                    contador++;
                }
                salida.close();
            }
            archivo.close();
        }
    }
    cout << "----------------------------------------------------------------------------------------------" << endl;
    cout <<"Tamaño de archivo: " << contador*_esquema.countBytes(const_cast<char *>(nombreTabla.c_str())) << endl;
    BloqueA.close();
}
// select * from nombreTabla where nombreColumna = valorBuscado
void control::mostrarWhere(string nombreTabla, string nombreColumna, string op, string valorBuscado)
{
    string bloqueAsignado = _headFile.getPahtBloquesAsignado();

    ifstream BloqueA(bloqueAsignado);

    char *esquemaTabla = _esquema.lineaEsquema(const_cast<char *>(nombreTabla.c_str()));
    string cabecera = _esquema.extraerIdentificadores(esquemaTabla);
    string tabulado = _esquema.tabulacion(cabecera);
    cout << tabulado << endl;
    cout << "----------------------------------------------------------------------------------------------" << endl;

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
            while (getline(archivo, contenido))
            {
                _query.consultaWhere(const_cast<char *>(nombreTabla.c_str()), const_cast<char *>(nombreColumna.c_str()), const_cast<char *>(op.c_str()), const_cast<char *>(valorBuscado.c_str()), const_cast<char *>(contenido.c_str()));
                contador++;
            }
            archivo.close();
        }
    }
    cout << "----------------------------------------------------------------------------------------------" << endl;
    cout <<"Tamaño de archivo: " << contador*_esquema.countBytes(const_cast<char *>(nombreTabla.c_str())) << endl;
    BloqueA.close();
}
void control::menu()
{
    int opcion;

    cout << "    1. Crear disco por defecto" << endl;
    cout << "    2. Crear disco personalizado" << endl;
    cout << "    3. Informacion de disco" << endl;
    cout << "    4. Crear tabla" << endl;
    cout << "    5. Insertar registro" << endl;
    cout << "    6. Mostrar tabla" << endl;
    cout << "    7. Consulta Where" << endl;
    cout << "    0. Salir" << endl;
    cout << "Seleccione una opción: ";
    cin >> opcion;

    while (opcion != 0)
    {
        if (opcion == 1)
        {
            _disk.createDisk();

            _headFile.createHeadFile(_disk.getPlato(), _disk.getSuperficie(), _disk.getPistasPorPlato(), _disk.getSectorPorPistas(), _disk.getSectoresPorBloque());
            _headFile.mapearBloques();
            _esquema.creaEsquema();
            _disk.informacionDisk();
        }
        else if (opcion == 2)
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
            cout << "Ingrese el la cantidad de sectores por bloque: ";
            cin >> sizeBloque;

            _disk = disk(plato, pistasPorPlato, sectorPorPistas, sizeSector, sizeBloque);
            _disk.createDisk();

            _headFile.createHeadFile(plato, 2, pistasPorPlato, sectorPorPistas, _disk.getSectoresPorBloque());
            _headFile.mapearBloques();

            _esquema.creaEsquema();
            _disk.informacionDisk();
        }
        else if (opcion == 3)
        {
            _disk.informacionDisk();
        }
        else if (opcion == 4)
        {
            _query.creaTabla();
        }
        else if (opcion == 5)
        {
            insertArchivoCSV();
        }
        else if (opcion == 6)
        {
            string nombreTabla;
            cout << "Ingrese el nombre de la tabla: ";
            cin >> nombreTabla;
            mostrarTabla(nombreTabla);
        }
        else if(opcion == 7)
        {
            char nombreTabla[50];
            cout << "Ingrese el nombre de la tabla: ";
            cin >> nombreTabla;

            char nombreColumna[50];
            cout << "Ingrese el nombre de la columna: ";
            cin >> nombreColumna;

            char op[10];
            cout << "Ingrese el operador: ";
            cin >> op;

            char valorBuscado[50];
            cout << "Ingrese el valor buscado: ";
            cin >> valorBuscado;
            
            mostrarWhere(nombreTabla, nombreColumna, op, valorBuscado);
        }
        else
        {
            cout << "Opción no válida." << endl;
        }

        cout << endl;
        cout << "    1. Crear disco por defecto" << endl;
        cout << "    2. Crear disco personalizado" << endl;
        cout << "    3. Informacion de disco" << endl;
        cout << "    4. Crear tabla" << endl;
        cout << "    5. Insertar registro" << endl;
        cout << "    6. Mostrar tabla" << endl;
        cout << "    7. Consulta Where" << endl;
        cout << "    0. Salir" << endl;
        cout << "Seleccione una opción: ";
        cin >> opcion;
    }

    cout << "Saliendo del menú..." << endl;
}
