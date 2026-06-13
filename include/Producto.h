#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include "Validador.h"

// ============================================================
//  Clase Producto
//  Integrante 1 – Productos e Inventario
// ============================================================
class Producto {
private:
    static int siguienteCodigo;   // miembro estatico compartido

    int    codigo;
    std::string nombre;
    double precio;
    int    stock;

public:
    // ── Constructores ────────────────────────────────────────
    // Constructor principal: valida todo antes de asignar
    Producto(const std::string& nombre, double precio, int stock)
        : codigo(siguienteCodigo) {
        // Validacion en orden: si alguna falla, el objeto no se crea
        ValidadorProducto::validarNombre(nombre);
        ValidadorProducto::validarPrecio(precio);
        ValidadorProducto::validarStock(stock);

        this->nombre = ValidadorProducto::normalizarNombre(nombre);
        this->precio = precio;
        this->stock  = stock;
        ++siguienteCodigo;
    }

    // Constructor de reconstruccion desde BD (codigo ya conocido)
    Producto(int codigo, const std::string& nombre, double precio, int stock)
        : codigo(codigo) {
        ValidadorProducto::validarNombre(nombre);
        ValidadorProducto::validarPrecio(precio);
        ValidadorProducto::validarStock(stock);
        this->nombre = ValidadorProducto::normalizarNombre(nombre);
        this->precio = precio;
        this->stock  = stock;
        // Ajustar el contador si el codigo cargado supera al actual
        if (codigo >= siguienteCodigo)
            siguienteCodigo = codigo + 1;
    }

    // Destructor (buena practica documentarlo)
    ~Producto() = default;

    // Copia y movimiento por defecto son seguros (no hay recursos dinamicos)
    Producto(const Producto&)            = default;
    Producto& operator=(const Producto&) = default;
    Producto(Producto&&)                 = default;
    Producto& operator=(Producto&&)      = default;

    // ── Getters ──────────────────────────────────────────────
    int                getCodigo()  const { return codigo;  }
    const std::string& getNombre()  const { return nombre;  }
    double             getPrecio()  const { return precio;  }
    int                getStock()   const { return stock;   }
    bool               hayStock()   const { return stock > 0; }

    // Miembro estatico: codigo que se asignara al proximo producto
    static int getSiguienteCodigo() { return siguienteCodigo; }

    // ── Setters con validacion ───────────────────────────────
    void setNombre(const std::string& nuevoNombre) {
        ValidadorProducto::validarNombre(nuevoNombre);
        nombre = ValidadorProducto::normalizarNombre(nuevoNombre);
    }

    void setPrecio(double nuevoPrecio) {
        ValidadorProducto::validarPrecio(nuevoPrecio);
        precio = nuevoPrecio;
    }

    void setStock(int nuevoStock) {
        ValidadorProducto::validarStock(nuevoStock);
        stock = nuevoStock;
    }

    // ── Operaciones de negocio ───────────────────────────────
    // Descontar stock al realizar una venta
    void descontarStock(int cantidad) {
        ValidadorProducto::validarCantidad(cantidad);
        if (cantidad > stock)
            throw StockInsuficienteException(stock, cantidad);
        stock -= cantidad;
    }

    // Agregar stock al recibir mercaderia
    void agregarStock(int cantidad) {
        ValidadorProducto::validarCantidad(cantidad);
        if (stock + cantidad > ValidadorProducto::STOCK_MAX)
            throw StockInvalidoException("El stock resultante supera el maximo permitido");
        stock += cantidad;
    }

    double calcularValorTotal() const {
        return precio * stock;
    }

    // ── Sobrecarga de operadores ─────────────────────────────
    // Igualdad por codigo (identidad unica del producto)
    bool operator==(const Producto& otro) const {
        return codigo == otro.codigo;
    }

    bool operator!=(const Producto& otro) const {
        return !(*this == otro);
    }

    // Ordenamiento por nombre
    bool operator<(const Producto& otro) const {
        return nombre < otro.nombre;
    }

    // Operador de salida amigable (funcion amiga)
    friend std::ostream& operator<<(std::ostream& os, const Producto& p) {
        os << std::left
           << "[" << std::setw(4) << p.codigo << "] "
           << std::setw(30) << p.nombre
           << " S/ " << std::setw(8) << std::fixed << std::setprecision(2) << p.precio
           << " Stock: " << std::setw(6) << p.stock;
        return os;
    }

    // Formato tabla con cabecera estatica
    static void imprimirCabecera() {
        std::cout << std::left
                  << std::setw(6)  << "Cod"
                  << std::setw(32) << "Nombre"
                  << std::setw(12) << "Precio"
                  << std::setw(8)  << "Stock"
                  << std::setw(14) << "Valor total"
                  << "\n"
                  << std::string(72, '-') << "\n";
    }

    void imprimirFila() const {
        std::cout << std::left
                  << std::setw(6)  << codigo
                  << std::setw(32) << nombre
                  << "S/ " << std::setw(9) << std::fixed << std::setprecision(2) << precio
                  << std::setw(8)  << stock
                  << "S/ " << std::fixed << std::setprecision(2) << calcularValorTotal()
                  << "\n";
    }
};

// Definicion del miembro estatico (fuera de la clase, en un .cpp)
// Como usamos header-only aqui, usamos inline (C++17)
inline int Producto::siguienteCodigo = 1001;
