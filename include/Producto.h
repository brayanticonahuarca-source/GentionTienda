#pragma once

#include <string>
#include "Validador.h"

using namespace std;

class Producto
{
private:
    static int siguienteCodigo;

    int codigo;
    string nombre;
    double precio;
    int stock;

public:
    Producto()
    {
        codigo = 0;
        nombre = "";
        precio = 0;
        stock = 0;
    }

    Producto(const string& nombre, double precio, int stock)
    {
        string nombreNormalizado = ValidadorProducto::normalizarNombre(nombre);

        ValidadorProducto::validarNombre(nombreNormalizado);
        ValidadorProducto::validarPrecio(precio);
        ValidadorProducto::validarStock(stock);

        this->codigo = siguienteCodigo++;
        this->nombre = nombreNormalizado;
        this->precio = precio;
        this->stock = stock;
    }

    int obtenerCodigo() const
    {
        return codigo;
    }

    string obtenerNombre() const
    {
        return nombre;
    }

    double obtenerPrecio() const
    {
        return precio;
    }

    int obtenerStock() const
    {
        return stock;
    }

    void establecerNombre(const string& nuevoNombre)
    {
        string nombreNormalizado = ValidadorProducto::normalizarNombre(nuevoNombre);
        ValidadorProducto::validarNombre(nombreNormalizado);
        nombre = nombreNormalizado;
    }

    void establecerPrecio(double nuevoPrecio)
    {
        ValidadorProducto::validarPrecio(nuevoPrecio);
        precio = nuevoPrecio;
    }

    void establecerStock(int nuevoStock)
    {
        ValidadorProducto::validarStock(nuevoStock);
        stock = nuevoStock;
    }

    void aumentarStock(int cantidad)
    {
        ValidadorProducto::validarCantidad(cantidad);

        if (stock + cantidad > ValidadorProducto::STOCK_MAX)
            throw StockInvalidoException("El stock supera el maximo permitido.");

        stock += cantidad;
    }

    void disminuirStock(int cantidad)
    {
        ValidadorProducto::validarCantidad(cantidad);

        if (cantidad > stock)
            throw StockInsuficienteException("Stock insuficiente.");

        stock -= cantidad;
    }
};

int Producto::siguienteCodigo = 1001;
