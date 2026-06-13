#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <ctime>
#include "Validador.h"
#include "Inventario.h"

// ============================================================
//  DetalleVenta  — una linea del ticket de venta
// ============================================================
class DetalleVenta {
private:
    int         codigoProducto;
    std::string nombreProducto;
    int         cantidad;
    double      precioUnitario;

public:
    DetalleVenta(int codProd, const std::string& nombre, int cant, double precio)
        : codigoProducto(codProd), nombreProducto(nombre),
          cantidad(cant), precioUnitario(precio) {
        ValidadorProducto::validarCantidad(cant);
        ValidadorProducto::validarPrecio(precio);
    }

    int         getCodigoProducto()  const { return codigoProducto;  }
    const std::string& getNombre()   const { return nombreProducto;  }
    int         getCantidad()        const { return cantidad;         }
    double      getPrecioUnitario()  const { return precioUnitario;   }
    double      getSubtotal()        const { return precioUnitario * cantidad; }

    // Sobrecarga de operador suma: acumular subtotales
    double operator+(double acumulado) const {
        return acumulado + getSubtotal();
    }

    friend std::ostream& operator<<(std::ostream& os, const DetalleVenta& d) {
        os << std::left
           << std::setw(30) << d.nombreProducto
           << std::setw(6)  << d.cantidad
           << "x S/ " << std::setw(8) << std::fixed << std::setprecision(2) << d.precioUnitario
           << "= S/ " << std::fixed << std::setprecision(2) << d.getSubtotal();
        return os;
    }
};

// ============================================================
//  Venta  — cabecera + lista de detalles
//  Integrante 2 – Ventas
// ============================================================
class Venta {
private:
    static int siguienteId;

    int                    id;
    std::vector<DetalleVenta> detalles;
    std::string            fecha;         // "YYYY-MM-DD"
    std::string            hora;          // "HH:MM:SS"
    std::string            vendedor;
    bool                   finalizada;

    static std::string fechaHoraActual(bool soloFecha = false) {
        std::time_t t = std::time(nullptr);
        std::tm*    tm = std::localtime(&t);
        char buf[32];
        if (soloFecha)
            std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
        else
            std::strftime(buf, sizeof(buf), "%H:%M:%S", tm);
        return std::string(buf);
    }

public:
    explicit Venta(const std::string& vendedor)
        : id(siguienteId++), fecha(fechaHoraActual(true)),
          hora(fechaHoraActual(false)), vendedor(vendedor), finalizada(false) {
        if (vendedor.empty())
            throw VentaException("El nombre del vendedor no puede estar vacio");
    }

    // Constructor de recarga desde BD
    Venta(int id, const std::string& vendedor, const std::string& fecha, const std::string& hora)
        : id(id), fecha(fecha), hora(hora), vendedor(vendedor), finalizada(true) {
        if (id >= siguienteId) siguienteId = id + 1;
    }

    ~Venta() = default;

    // ── Construccion de la venta ─────────────────────────────
    void agregarDetalle(Inventario& inv, int codigoProducto, int cantidad) {
        if (finalizada)
            throw VentaException("No se pueden agregar items a una venta ya finalizada");

        Producto* p = inv.buscarPorCodigo(codigoProducto); // lanza si no existe
        ValidadorProducto::validarCantidad(cantidad);

        if (cantidad > p->getStock())
            throw StockInsuficienteException(p->getStock(), cantidad);

        // Verificar si ya existe el mismo producto en esta venta
        for (DetalleVenta& d : detalles) {
            if (d.getCodigoProducto() == codigoProducto) {
                int nuevaCant = d.getCantidad() + cantidad;
                if (nuevaCant > p->getStock())
                    throw StockInsuficienteException(p->getStock(), nuevaCant);
                // Reemplazar detalle con nueva cantidad
                detalles.erase(std::remove_if(detalles.begin(), detalles.end(),
                    [codigoProducto](const DetalleVenta& x){
                        return x.getCodigoProducto() == codigoProducto;
                    }), detalles.end());
                detalles.emplace_back(codigoProducto, p->getNombre(), nuevaCant, p->getPrecio());
                return;
            }
        }
        detalles.emplace_back(codigoProducto, p->getNombre(), cantidad, p->getPrecio());
    }

    // Confirmar: descuenta stock real
    void confirmar(Inventario& inv) {
        if (finalizada)
            throw VentaException("La venta ya fue confirmada");
        if (detalles.empty())
            throw VentaVaciaException();

        // Verificar stock de todos antes de descontar ninguno (atomicidad)
        for (const DetalleVenta& d : detalles) {
            Producto* p = inv.buscarPorCodigo(d.getCodigoProducto());
            if (d.getCantidad() > p->getStock())
                throw StockInsuficienteException(p->getStock(), d.getCantidad());
        }
        // Descontar
        for (const DetalleVenta& d : detalles)
            inv.descontarStock(d.getCodigoProducto(), d.getCantidad());

        finalizada = true;
    }

    // ── Calculos ─────────────────────────────────────────────
    // Sobrecarga de operator+ para sumar detalles de forma funcional
    double calcularTotal() const {
        double total = 0.0;
        for (const DetalleVenta& d : detalles)
            total = d + total;   // usa el operator+ de DetalleVenta
        return total;
    }

    // Sobrecarga: sumar dos ventas = suma de totales
    double operator+(const Venta& otra) const {
        return calcularTotal() + otra.calcularTotal();
    }

    // ── Getters ──────────────────────────────────────────────
    int         getId()        const { return id;         }
    const std::string& getFecha()     const { return fecha;    }
    const std::string& getHora()      const { return hora;     }
    const std::string& getVendedor()  const { return vendedor; }
    bool        estaFinalizada() const { return finalizada; }
    int         cantidadItems() const { return static_cast<int>(detalles.size()); }
    const std::vector<DetalleVenta>& getDetalles() const { return detalles; }

    static int getSiguienteId() { return siguienteId; }

    // ── Impresion ────────────────────────────────────────────
    void imprimirTicket() const {
        std::cout << "\n========================================\n";
        std::cout << "         TIENDA - TICKET DE VENTA\n";
        std::cout << "========================================\n";
        std::cout << "Venta #" << id << "   " << fecha << " " << hora << "\n";
        std::cout << "Vendedor: " << vendedor << "\n";
        std::cout << "----------------------------------------\n";
        for (const DetalleVenta& d : detalles)
            std::cout << d << "\n";
        std::cout << "----------------------------------------\n";
        std::cout << std::right << std::setw(40)
                  << "TOTAL: S/ " << std::fixed << std::setprecision(2) << calcularTotal() << "\n";
        std::cout << "========================================\n\n";
    }

    friend std::ostream& operator<<(std::ostream& os, const Venta& v) {
        os << "Venta #" << std::setw(4) << v.id
           << "  " << v.fecha << " " << v.hora
           << "  Vendedor: " << std::setw(15) << v.vendedor
           << "  Items: " << std::setw(3) << v.detalles.size()
           << "  Total: S/ " << std::fixed << std::setprecision(2) << v.calcularTotal();
        return os;
    }
};

inline int Venta::siguienteId = 1;
