#pragma once

#include <string>
#include <fstream>

#include "Excepciones.h"

using namespace std;

//==================================================
// BASE DE DATOS SIMPLE
// Por ahora guarda texto en archivos.
// Luego se puede cambiar por SQLite.
//==================================================

class BaseDatos
{
private:

    string rutaArchivo;
    bool conectada;

public:

    BaseDatos(const string& ruta)
    {
        if (ruta.empty())
        {
            throw ExcepcionValidacion(
                "La ruta de la base de datos no puede estar vacia."
            );
        }

        rutaArchivo = ruta;
        conectada = false;
    }

    void conectar()
    {
        ofstream archivo(rutaArchivo, ios::app);

        if (!archivo.is_open())
        {
            throw ExcepcionTienda(
                "No se pudo abrir la base de datos."
            );
        }

        conectada = true;
        archivo.close();
    }

    void desconectar()
    {
        conectada = false;
    }

    bool estaConectada() const
    {
        return conectada;
    }

    string obtenerRuta() const
    {
        return rutaArchivo;
    }

    void guardarLinea(const string& linea)
    {
        if (!conectada)
        {
            throw ExcepcionTienda(
                "La base de datos no esta conectada."
            );
        }

        if (linea.empty())
        {
            throw ExcepcionValidacion(
                "No se puede guardar una linea vacia."
            );
        }

        ofstream archivo(rutaArchivo, ios::app);

        if (!archivo.is_open())
        {
            throw ExcepcionTienda(
                "No se pudo escribir en la base de datos."
            );
        }

        archivo << linea << endl;

        archivo.close();
    }
};
