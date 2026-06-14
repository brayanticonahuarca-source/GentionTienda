#pragma once

#include <stdexcept>
#include <string>

using namespace std;

//==================================================
// EXCEPCION BASE DEL SISTEMA
//==================================================

class ExcepcionTienda : public runtime_error
{
public:
    ExcepcionTienda(const string& mensaje)
        : runtime_error(mensaje)
    {
    }

    virtual ~ExcepcionTienda() {}
};

//==================================================
// VALIDACIONES
//==================================================

class ExcepcionValidacion : public ExcepcionTienda
{
public:
    ExcepcionValidacion(const string& mensaje)
        : ExcepcionTienda(mensaje)
    {
    }
};

class NombreInvalidoException : public ExcepcionValidacion
{
public:
    NombreInvalidoException(const string& mensaje)
        : ExcepcionValidacion(mensaje)
    {
    }
};

class PrecioInvalidoException : public ExcepcionValidacion
{
public:
    PrecioInvalidoException(const string& mensaje)
        : ExcepcionValidacion(mensaje)
    {
    }
};

class StockInvalidoException : public ExcepcionValidacion
{
public:
    StockInvalidoException(const string& mensaje)
        : ExcepcionValidacion(mensaje)
    {
    }
};

class CantidadInvalidaException : public ExcepcionValidacion
{
public:
    CantidadInvalidaException(const string& mensaje)
        : ExcepcionValidacion(mensaje)
    {
    }
};

class CodigoInvalidoException : public ExcepcionValidacion
{
public:
    CodigoInvalidoException(const string& mensaje)
        : ExcepcionValidacion(mensaje)
    {
    }
};

class MontoInvalidoException : public ExcepcionValidacion
{
public:
    MontoInvalidoException(const string& mensaje)
        : ExcepcionValidacion(mensaje)
    {
    }
};

class FechaInvalidaException : public ExcepcionValidacion
{
public:
    FechaInvalidaException(const string& mensaje)
        : ExcepcionValidacion(mensaje)
    {
    }
};

class EntradaInvalidaException : public ExcepcionValidacion
{
public:
    EntradaInvalidaException(const string& mensaje)
        : ExcepcionValidacion(mensaje)
    {
    }
};

//==================================================
// INVENTARIO / PRODUCTOS
//==================================================

class InventarioException : public ExcepcionTienda
{
public:
    InventarioException(const string& mensaje)
        : ExcepcionTienda(mensaje)
    {
    }
};

class ProductoNoEncontradoException : public InventarioException
{
public:
    ProductoNoEncontradoException()
        : InventarioException("Producto no encontrado.")
    {
    }

    ProductoNoEncontradoException(const string& mensaje)
        : InventarioException(mensaje)
    {
    }

    ProductoNoEncontradoException(int codigo)
        : InventarioException("Producto no encontrado. Codigo: " + to_string(codigo))
    {
    }
};

class ProductoDuplicadoException : public InventarioException
{
public:
    ProductoDuplicadoException()
        : InventarioException("Producto duplicado.")
    {
    }

    ProductoDuplicadoException(const string& mensaje)
        : InventarioException(mensaje)
    {
    }
};

class InventarioVacioException : public InventarioException
{
public:
    InventarioVacioException()
        : InventarioException("El inventario esta vacio.")
    {
    }

    InventarioVacioException(const string& mensaje)
        : InventarioException(mensaje)
    {
    }
};

class StockInsuficienteException : public InventarioException
{
public:
    StockInsuficienteException()
        : InventarioException("Stock insuficiente.")
    {
    }

    StockInsuficienteException(const string& mensaje)
        : InventarioException(mensaje)
    {
    }

    StockInsuficienteException(int disponible, int solicitado)
        : InventarioException(
            "Stock insuficiente. Disponible: " +
            to_string(disponible) +
            ", solicitado: " +
            to_string(solicitado)
        )
    {
    }
};

//==================================================
// VENTAS
//==================================================

class VentaException : public ExcepcionTienda
{
public:
    VentaException(const string& mensaje)
        : ExcepcionTienda(mensaje)
    {
    }
};

class VentaVaciaException : public VentaException
{
public:
    VentaVaciaException()
        : VentaException("No se puede confirmar una venta vacia.")
    {
    }

    VentaVaciaException(const string& mensaje)
        : VentaException(mensaje)
    {
    }
};

class VentaFinalizadaException : public VentaException
{
public:
    VentaFinalizadaException()
        : VentaException("La venta ya fue finalizada.")
    {
    }

    VentaFinalizadaException(const string& mensaje)
        : VentaException(mensaje)
    {
    }
};

class VendedorInvalidoException : public VentaException
{
public:
    VendedorInvalidoException()
        : VentaException("Vendedor invalido.")
    {
    }

    VendedorInvalidoException(const string& mensaje)
        : VentaException(mensaje)
    {
    }
};

class TotalVentaExcedidoException : public VentaException
{
public:
    TotalVentaExcedidoException()
        : VentaException("El total de la venta excede el limite permitido.")
    {
    }

    TotalVentaExcedidoException(const string& mensaje)
        : VentaException(mensaje)
    {
    }

    TotalVentaExcedidoException(double total)
        : VentaException("El total de la venta excede el limite permitido. Total: S/ " + to_string(total))
    {
    }
};

class LimiteItemsVentaException : public VentaException
{
public:
    LimiteItemsVentaException()
        : VentaException("La venta supera el limite maximo de productos permitidos.")
    {
    }

    LimiteItemsVentaException(const string& mensaje)
        : VentaException(mensaje)
    {
    }
};
