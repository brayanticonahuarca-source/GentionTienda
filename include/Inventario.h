#pragma once

#include <vector>
#include <string>
#include <algorithm>

#include "Producto.h"
#include "Validador.h"
#include "Excepciones.h"

using namespace std;

class Inventario
{
private:
    vector<Producto> productos;

    void validarCodigo(int codigo) const
    {
        if (codigo <= 0)
        {
            throw CodigoInvalidoException(
                "El codigo debe ser mayor que cero."
            );
        }
    }

    bool existeNombreNormalizado(
        const string& nombreNormalizado,
        int codigoIgnorado = -1
    ) const
    {
        for (const Producto& producto : productos)
        {
            if (producto.obtenerNombre() == nombreNormalizado &&
                producto.obtenerCodigo() != codigoIgnorado)
            {
                return true;
            }
        }

        return false;
    }

public:
    Inventario()
    {
    }

    ~Inventario()
    {
    }

    //========================================
    // AGREGAR PRODUCTO
    //========================================

    Producto agregarProducto(
        const string& nombreProducto,
        double precioProducto,
        int stockProducto
    )
    {
        string nombreNormalizado =
            ValidadorProducto::normalizarNombre(nombreProducto);

        ValidadorProducto::validarNombre(nombreNormalizado);
        ValidadorProducto::validarPrecio(precioProducto);
        ValidadorProducto::validarStock(stockProducto);

        if (existeNombreNormalizado(nombreNormalizado))
        {
            throw ProductoDuplicadoException(
                "Ya existe un producto con el nombre: " +
                nombreNormalizado
            );
        }

        Producto nuevoProducto(
            nombreNormalizado,
            precioProducto,
            stockProducto
        );

        productos.push_back(nuevoProducto);

        return productos.back();
    }

    //========================================
    // BUSCAR PRODUCTO
    //========================================

    Producto* buscarPorCodigo(int codigo)
    {
        validarCodigo(codigo);

        for (Producto& producto : productos)
        {
            if (producto.obtenerCodigo() == codigo)
            {
                return &producto;
            }
        }

        throw ProductoNoEncontradoException(
            "No existe un producto con el codigo indicado."
        );
    }

    const Producto* buscarPorCodigo(int codigo) const
    {
        validarCodigo(codigo);

        for (const Producto& producto : productos)
        {
            if (producto.obtenerCodigo() == codigo)
            {
                return &producto;
            }
        }

        throw ProductoNoEncontradoException(
            "No existe un producto con el codigo indicado."
        );
    }

    Producto* buscarPorNombre(const string& nombreProducto)
    {
        string nombreNormalizado =
            ValidadorProducto::normalizarNombre(nombreProducto);

        ValidadorProducto::validarNombre(nombreNormalizado);

        for (Producto& producto : productos)
        {
            if (producto.obtenerNombre() == nombreNormalizado)
            {
                return &producto;
            }
        }

        throw ProductoNoEncontradoException(
            "No existe un producto con el nombre indicado."
        );
    }

    //========================================
    // VERIFICACIONES
    //========================================

    bool existeCodigo(int codigo) const
    {
        if (codigo <= 0)
        {
            return false;
        }

        for (const Producto& producto : productos)
        {
            if (producto.obtenerCodigo() == codigo)
            {
                return true;
            }
        }

        return false;
    }

    bool existeNombre(const string& nombreProducto) const
    {
        string nombreNormalizado =
            ValidadorProducto::normalizarNombre(nombreProducto);

        if (!ValidadorProducto::esNombreValido(nombreNormalizado))
        {
            return false;
        }

        return existeNombreNormalizado(nombreNormalizado);
    }

    bool estaVacio() const
    {
        return productos.empty();
    }

    int cantidadProductos() const
    {
        return static_cast<int>(productos.size());
    }

    //========================================
    // ACTUALIZAR PRODUCTO
    //========================================

    void actualizarNombre(int codigo, const string& nuevoNombre)
    {
        validarCodigo(codigo);

        Producto* producto = buscarPorCodigo(codigo);

        string nombreNormalizado =
            ValidadorProducto::normalizarNombre(nuevoNombre);

        ValidadorProducto::validarNombre(nombreNormalizado);

        if (existeNombreNormalizado(
                nombreNormalizado,
                producto->obtenerCodigo()
            ))
        {
            throw ProductoDuplicadoException(
                "Ya existe otro producto con ese nombre."
            );
        }

        producto->establecerNombre(nombreNormalizado);
    }

    void actualizarPrecio(int codigo, double nuevoPrecio)
    {
        validarCodigo(codigo);

        Producto* producto = buscarPorCodigo(codigo);

        producto->establecerPrecio(nuevoPrecio);
    }

    void actualizarStock(int codigo, int nuevoStock)
    {
        validarCodigo(codigo);

        Producto* producto = buscarPorCodigo(codigo);

        producto->establecerStock(nuevoStock);
    }

    //========================================
    // STOCK
    //========================================

    void aumentarStock(int codigo, int cantidad)
    {
        validarCodigo(codigo);

        Producto* producto = buscarPorCodigo(codigo);

        producto->aumentarStock(cantidad);
    }

    void descontarStock(int codigo, int cantidad)
    {
        validarCodigo(codigo);

        Producto* producto = buscarPorCodigo(codigo);

        producto->disminuirStock(cantidad);
    }

    //========================================
    // ELIMINAR PRODUCTO
    //========================================

    void eliminarProducto(int codigo)
    {
        validarCodigo(codigo);

        auto iterador = remove_if(
            productos.begin(),
            productos.end(),
            [codigo](const Producto& producto)
            {
                return producto.obtenerCodigo() == codigo;
            }
        );

        if (iterador == productos.end())
        {
            throw ProductoNoEncontradoException(
                "No se puede eliminar. Producto no encontrado."
            );
        }

        productos.erase(iterador, productos.end());
    }

    //========================================
    // LISTAR PRODUCTOS
    //========================================

    vector<Producto> obtenerProductos() const
    {
        return productos;
    }

    vector<Producto> obtenerProductosConStockBajo() const
    {
        vector<Producto> resultado;

        for (const Producto& producto : productos)
        {
            if (producto.obtenerStock() <= 10)
            {
                resultado.push_back(producto);
            }
        }

        return resultado;
    }
};
