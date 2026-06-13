#pragma once

#include <stdexcept>
#include <string>

using namespace std;

// EXCEPCION BASE

class ExcepcionTienda : public runtime_error
{
public:
    ExcepcionTienda(const string& mensaje): runtime_error(mensaje){}
    virtual ~ExcepcionTienda() {}
};

// VALIDACION

class ExcepcionValidacion : public ExcepcionTienda
{
public:
    ExcepcionValidacion(const string& mensaje): ExcepcionTienda(mensaje){}
};

class NombreInvalidoException : public ExcepcionValidacion
{
public:

    NombreInvalidoException(const string& mensaje) : ExcepcionValidacion(mensaje){}
};

class PrecioInvalidoException : public ExcepcionValidacion
{
public:
    PrecioInvalidoException(const string& mensaje): ExcepcionValidacion(mensaje){}
};

class StockInvalidoException : public ExcepcionValidacion
{
public:

    StockInvalidoException(const string& mensaje): ExcepcionValidacion(mensaje){}
};

class CantidadInvalidaException : public ExcepcionValidacion
{
    public:
    CantidadInvalidaException(const string& mensaje): ExcepcionValidacion(mensaje){}
};
// INVENTARIO
class InventarioException : public ExcepcionTienda
{
    public:
    InventarioException(const string& mensaje): ExcepcionTienda(mensaje){}
};

class ProductoNoEncontradoException : public InventarioException
{
    public:
    ProductoNoEncontradoException(const string& mensaje): InventarioException(mensaje){}
};

class ProductoDuplicadoException : public InventarioException
{
    public:
    ProductoDuplicadoException(const string& mensaje): InventarioException(mensaje){}
};

class StockInsuficienteException : public InventarioException
{
    public:
    StockInsuficienteException(const string& mensaje) : InventarioException(mensaje){}
};

// VENTAS
class VentaException : public ExcepcionTienda
{
    public:
    VentaException(const string& mensaje) : ExcepcionTienda(mensaje){}
};

class VentaVaciaException : public VentaException
{
    public:
    VentaVaciaException(const string& mensaje): VentaException(mensaje){}
};

class VentaFinalizadaException : public VentaException
{
public:
    VentaFinalizadaException(const string& mensaje): VentaException(mensaje){}
};
