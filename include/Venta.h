#pragma once

#include <vector>
#include <string>
#include <ctime>

#include "Inventario.h"
#include "Producto.h"
#include "Validador.h"
#include "Excepciones.h"

using namespace std;

const int VENDEDOR_MIN = 2;
const int VENDEDOR_MAX = 40;
const int ITEMS_MAX_VENTA = 100;
const double TOTAL_MAX_VENTA = 100000.00;

class DetalleVenta
{
private:
    int codigoProducto;
    string nombreProducto;
    int cantidad;
    double precioUnitario;

public:
    DetalleVenta(
        int codigoProductoVenta,
        const string& nombreProductoVenta,
        int cantidadVenta,
        double precioVenta
    )
    {
        if (codigoProductoVenta <= 0)
        {
            throw CodigoInvalidoException(
                "El codigo del producto debe ser mayor que cero."
            );
        }

        string nombreNormalizado =
            ValidadorProducto::normalizarNombre(nombreProductoVenta);

        ValidadorProducto::validarNombre(nombreNormalizado);
        ValidadorProducto::validarCantidad(cantidadVenta);
        ValidadorProducto::validarPrecio(precioVenta);

        codigoProducto = codigoProductoVenta;
        nombreProducto = nombreNormalizado;
        cantidad = cantidadVenta;
        precioUnitario = precioVenta;
    }

    int obtenerCodigoProducto() const
    {
        return codigoProducto;
    }

    string obtenerNombreProducto() const
    {
        return nombreProducto;
    }

    int obtenerCantidad() const
    {
        return cantidad;
    }

    double obtenerPrecioUnitario() const
    {
        return precioUnitario;
    }

    double obtenerSubtotal() const
    {
        return precioUnitario * cantidad;
    }

    void aumentarCantidad(int cantidadExtra, int stockDisponible)
    {
        ValidadorProducto::validarCantidad(cantidadExtra);

        int nuevaCantidad = cantidad + cantidadExtra;

        if (nuevaCantidad > ValidadorProducto::CANTIDAD_MAX)
        {
            throw CantidadInvalidaException(
                "La cantidad supera el maximo permitido por venta."
            );
        }

        if (nuevaCantidad > stockDisponible)
        {
            throw StockInsuficienteException(
                "Stock insuficiente para aumentar la cantidad."
            );
        }

        cantidad = nuevaCantidad;
    }
};

class Venta
{
private:
    inline static int siguienteId = 1;

    int id;
    vector<DetalleVenta> detalles;
    string fecha;
    string hora;
    string vendedor;
    bool finalizada;

    static string obtenerFechaActual()
    {
        time_t tiempoActual = time(nullptr);
        tm tiempoLocal;

#ifdef _WIN32
        localtime_s(&tiempoLocal, &tiempoActual);
#else
        localtime_r(&tiempoActual, &tiempoLocal);
#endif

        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tiempoLocal);

        return string(buffer);
    }

    static string obtenerHoraActual()
    {
        time_t tiempoActual = time(nullptr);
        tm tiempoLocal;

#ifdef _WIN32
        localtime_s(&tiempoLocal, &tiempoActual);
#else
        localtime_r(&tiempoActual, &tiempoLocal);
#endif

        char buffer[9];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", &tiempoLocal);

        return string(buffer);
    }

    static void validarVendedor(const string& nombreVendedor)
    {
        string vendedorLimpio =
            ValidadorProducto::trim(nombreVendedor);

        if (vendedorLimpio.empty())
        {
            throw VentaException(
                "El vendedor no puede estar vacio."
            );
        }

        if ((int)vendedorLimpio.size() < VENDEDOR_MIN)
        {
            throw VentaException(
                "El nombre del vendedor es demasiado corto."
            );
        }

        if ((int)vendedorLimpio.size() > VENDEDOR_MAX)
        {
            throw VentaException(
                "El nombre del vendedor es demasiado largo."
            );
        }
    }

    void validarNoFinalizada() const
    {
        if (finalizada)
        {
            throw VentaFinalizadaException(
                "No se puede modificar una venta finalizada."
            );
        }
    }

    void validarLimiteItems() const
    {
        if ((int)detalles.size() >= ITEMS_MAX_VENTA)
        {
            throw VentaException(
                "La venta supera el limite maximo de productos."
            );
        }
    }

    void validarTotalMaximo(double total) const
    {
        if (total > TOTAL_MAX_VENTA)
        {
            throw VentaException(
                "La venta supera el total maximo permitido."
            );
        }
    }

public:
    Venta(const string& nombreVendedor)
    {
        string vendedorLimpio =
            ValidadorProducto::trim(nombreVendedor);

        validarVendedor(vendedorLimpio);

        id = siguienteId++;
        fecha = obtenerFechaActual();
        hora = obtenerHoraActual();
        vendedor = vendedorLimpio;
        finalizada = false;
    }

    void agregarDetalle(
        Inventario& inventario,
        int codigoProducto,
        int cantidad
    )
    {
        validarNoFinalizada();

        ValidadorProducto::validarCantidad(cantidad);

        Producto* producto =
            inventario.buscarPorCodigo(codigoProducto);

        if (cantidad > producto->obtenerStock())
        {
            throw StockInsuficienteException(
                "Stock insuficiente para realizar la venta."
            );
        }

        for (DetalleVenta& detalle : detalles)
        {
            if (detalle.obtenerCodigoProducto() == codigoProducto)
            {
                double totalTemporal =
                    calcularTotal()
                    - detalle.obtenerSubtotal()
                    + ((detalle.obtenerCantidad() + cantidad)
                        * producto->obtenerPrecio());

                validarTotalMaximo(totalTemporal);

                detalle.aumentarCantidad(
                    cantidad,
                    producto->obtenerStock()
                );

                return;
            }
        }

        validarLimiteItems();

        DetalleVenta nuevoDetalle(
            producto->obtenerCodigo(),
            producto->obtenerNombre(),
            cantidad,
            producto->obtenerPrecio()
        );

        double totalTemporal =
            calcularTotal() + nuevoDetalle.obtenerSubtotal();

        validarTotalMaximo(totalTemporal);

        detalles.push_back(nuevoDetalle);
    }

    void confirmar(Inventario& inventario)
    {
        validarNoFinalizada();

        if (detalles.empty())
        {
            throw VentaVaciaException(
                "No se puede confirmar una venta vacia."
            );
        }

        for (const DetalleVenta& detalle : detalles)
        {
            Producto* producto =
                inventario.buscarPorCodigo(
                    detalle.obtenerCodigoProducto()
                );

            if (detalle.obtenerCantidad() >
                producto->obtenerStock())
            {
                throw StockInsuficienteException(
                    "Stock insuficiente al confirmar la venta."
                );
            }
        }

        for (const DetalleVenta& detalle : detalles)
        {
            inventario.descontarStock(
                detalle.obtenerCodigoProducto(),
                detalle.obtenerCantidad()
            );
        }

        finalizada = true;
    }

    double calcularTotal() const
    {
        double total = 0.0;

        for (const DetalleVenta& detalle : detalles)
        {
            total += detalle.obtenerSubtotal();
        }

        return total;
    }

    int obtenerId() const
    {
        return id;
    }

    string obtenerFecha() const
    {
        return fecha;
    }

    string obtenerHora() const
    {
        return hora;
    }

    string obtenerVendedor() const
    {
        return vendedor;
    }

    bool estaFinalizada() const
    {
        return finalizada;
    }

    int obtenerCantidadItems() const
    {
        return static_cast<int>(detalles.size());
    }

    vector<DetalleVenta> obtenerDetalles() const
    {
        return detalles;
    }
};
//==================================================
// REGISTRO DE VENTAS
// Guarda el historial de ventas confirmadas
//==================================================

class RegistroVentas
{
private:

    vector<Venta> ventas;

public:

    RegistroVentas()
    {
    }

    ~RegistroVentas()
    {
    }

    void registrarVenta(const Venta& venta)
    {
        if (!venta.estaFinalizada())
        {
            throw VentaException(
                "Solo se pueden registrar ventas finalizadas."
            );
        }

        ventas.push_back(venta);
    }

    vector<Venta> obtenerVentas() const
    {
        return ventas;
    }

    int obtenerCantidadVentas() const
    {
        return static_cast<int>(ventas.size());
    }

    bool estaVacio() const
    {
        return ventas.empty();
    }

    double calcularTotalVentas() const
    {
        double total = 0.0;

        for (const Venta& venta : ventas)
        {
            total += venta.calcularTotal();
        }

        return total;
    }

    vector<Venta> obtenerVentasPorFecha(const string& fecha) const
    {
        vector<Venta> resultado;

        for (const Venta& venta : ventas)
        {
            if (venta.obtenerFecha() == fecha)
            {
                resultado.push_back(venta);
            }
        }

        return resultado;
    }

    double calcularTotalPorFecha(const string& fecha) const
    {
        double total = 0.0;

        for (const Venta& venta : ventas)
        {
            if (venta.obtenerFecha() == fecha)
            {
                total += venta.calcularTotal();
            }
        }

        return total;
    }

    Venta obtenerVentaPorId(int id) const
    {
        for (const Venta& venta : ventas)
        {
            if (venta.obtenerId() == id)
            {
                return venta;
            }
        }

        throw VentaException(
            "No existe una venta con el ID indicado."
        );
    }

    vector<Venta> obtenerUltimasVentas(int cantidad) const
    {
        ValidadorProducto::validarCantidad(cantidad);

        vector<Venta> resultado;

        int totalVentas = static_cast<int>(ventas.size());
        int inicio = totalVentas - cantidad;

        if (inicio < 0)
        {
            inicio = 0;
        }

        for (int i = inicio; i < totalVentas; i++)
        {
            resultado.push_back(ventas[i]);
        }

        return resultado;
    }
};
