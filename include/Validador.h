#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <cctype>
#include "Excepciones.h"

using namespace std;

class ValidadorProducto
{
public:
    static const int NOMBRE_MIN = 2;
    static const int NOMBRE_MAX = 50;

    static const int STOCK_MIN = 0;
    static const int STOCK_MAX = 50000;

    static const int CANTIDAD_MIN = 1;
    static const int CANTIDAD_MAX = 1000;

    static constexpr double PRECIO_MIN = 0.10;
    static constexpr double PRECIO_MAX = 10000.00;

    ValidadorProducto() = delete;
    ~ValidadorProducto() = delete;

    static void validarNombre(const string& nombre)
    {
        string limpio = trim(nombre);

        if (limpio.empty())
            throw NombreInvalidoException("El nombre no puede estar vacio.");

        if ((int)limpio.size() < NOMBRE_MIN)
            throw NombreInvalidoException("El nombre es demasiado corto.");

        if ((int)limpio.size() > NOMBRE_MAX)
            throw NombreInvalidoException("El nombre es demasiado largo.");

        bool tieneLetra = false;

        for (char c : limpio)
        {
            unsigned char uc = static_cast<unsigned char>(c);

            if (!isprint(uc))
                throw NombreInvalidoException("El nombre contiene caracteres invalidos.");

            if (isalpha(uc))
                tieneLetra = true;
        }

        if (!tieneLetra)
            throw NombreInvalidoException("El nombre debe tener al menos una letra.");
    }
    static void validarPrecio(double precio)
    {
        if (precio < PRECIO_MIN)
            throw PrecioInvalidoException("El precio minimo es S/ " + formatear(PRECIO_MIN));

        if (precio > PRECIO_MAX)
            throw PrecioInvalidoException("El precio maximo es S/ " + formatear(PRECIO_MAX));
    }
    static void validarStock(int stock)
    {
        if (stock < STOCK_MIN)
            throw StockInvalidoException("El stock no puede ser negativo.");
        if (stock > STOCK_MAX)
            throw StockInvalidoException("El stock supera el maximo permitido.");
    }
    static void validarCantidad(int cantidad)
    {
        if (cantidad < CANTIDAD_MIN)
            throw CantidadInvalidaException("La cantidad debe ser mayor a cero.");

        if (cantidad > CANTIDAD_MAX)
            throw CantidadInvalidaException("La cantidad supera el maximo permitido.");
    }
    static string trim(const string& texto)
    {
        size_t inicio = texto.find_first_not_of(" \t\n\r\f\v");
        if (inicio == string::npos)
            return "";
        size_t fin = texto.find_last_not_of(" \t\n\r\f\v");
        return texto.substr(inicio, fin - inicio + 1);
    }

    static string normalizarNombre(const string& nombre)
    {
        string texto = trim(nombre);
        bool nuevaPalabra = true;
        for (char& c : texto)
        {
            unsigned char uc = static_cast<unsigned char>(c);
            if (isspace(uc))
            {
                nuevaPalabra = true;
            }
            else if (nuevaPalabra)
            {
                c = static_cast<char>(toupper(uc));
                nuevaPalabra = false;
            }
            else
            {
                c = static_cast<char>(tolower(uc));
            }
        }
        return texto;
    }
    static string formatear(double valor)
    {
        ostringstream ss;
        ss << fixed << setprecision(2) << valor;
        return ss.str();
    }
};
