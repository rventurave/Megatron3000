#ifndef BLOQUE_H
#define BLOQUE_H

class bloque {
private:
    char bloqueB[50000];
    void agregarAlBloque(const char* contenido);
    void procesarArchivoRegistro(const char* ruta);

public:
    bloque();
    void crearBloque(const char* archivoRutas);
    void mostrarBloque();
    const char* obtenerBloque();
    bool insertarLinea(const char* linea);
    void vaciarBloque();

};

#endif
