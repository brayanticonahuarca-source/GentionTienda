#pragma once
#include <stdexcept>
#include <string>

// ============================================================
//  Jerarquia de excepciones del sistema
//  TiendaException (base)
//    ├── ValidacionException
//    │     ├── NombreInvalidoException
//    │     ├── PrecioInvalidoException
//    │     └── StockInvalidoException
//    ├── AutenticacionException
//    │     ├── UsuarioNoEncontradoException
//    │     └── ContrasenaIncorrectaException
//    ├── InventarioException
//    │     ├── StockInsuficienteException
//    │     └── ProductoNoEncontradoException
//    ├── VentaException
//    ├── FinanzasException
//    └── BaseDatosException
// ============================================================

class TiendaException : public std::exception {
protected:
    std::string mensaje;
    std::string modulo;
public:
    TiendaException(const std::string& msg, const std::string& mod = "Sistema")
        : mensaje(msg), modulo(mod) {}
    const char* what() const noexcept override { return mensaje.c_str(); }
    const std::string& getModulo() const { return modulo; }
    virtual std::string getTipo() const { return "TiendaException"; }
};

// ── Validacion ──────────────────────────────────────────────
class ValidacionException : public TiendaException {
public:
    ValidacionException(const std::string& msg)
        : TiendaException(msg, "Validacion") {}
    std::string getTipo() const override { return "ValidacionException"; }
};

class NombreInvalidoException : public ValidacionException {
public:
    NombreInvalidoException(const std::string& detalle)
        : ValidacionException("Nombre invalido: " + detalle) {}
    std::string getTipo() const override { return "NombreInvalidoException"; }
};

class PrecioInvalidoException : public ValidacionException {
public:
    PrecioInvalidoException(const std::string& detalle)
        : ValidacionException("Precio invalido: " + detalle) {}
    std::string getTipo() const override { return "PrecioInvalidoException"; }
};

class StockInvalidoException : public ValidacionException {
public:
    StockInvalidoException(const std::string& detalle)
        : ValidacionException("Stock invalido: " + detalle) {}
    std::string getTipo() const override { return "StockInvalidoException"; }
};

class CantidadInvalidaException : public ValidacionException {
public:
    CantidadInvalidaException(const std::string& detalle)
        : ValidacionException("Cantidad invalida: " + detalle) {}
    std::string getTipo() const override { return "CantidadInvalidaException"; }
};

class MontoInvalidoException : public ValidacionException {
public:
    MontoInvalidoException(const std::string& detalle)
        : ValidacionException("Monto invalido: " + detalle) {}
    std::string getTipo() const override { return "MontoInvalidoException"; }
};

class FechaInvalidaException : public ValidacionException {
public:
    FechaInvalidaException(const std::string& detalle)
        : ValidacionException("Fecha invalida: " + detalle) {}
    std::string getTipo() const override { return "FechaInvalidaException"; }
};

class EntradaInvalidaException : public ValidacionException {
public:
    EntradaInvalidaException(const std::string& detalle)
        : ValidacionException("Entrada invalida: " + detalle) {}
    std::string getTipo() const override { return "EntradaInvalidaException"; }
};

// ── Autenticacion ───────────────────────────────────────────
class AutenticacionException : public TiendaException {
public:
    AutenticacionException(const std::string& msg)
        : TiendaException(msg, "Autenticacion") {}
    std::string getTipo() const override { return "AutenticacionException"; }
};

class UsuarioNoEncontradoException : public AutenticacionException {
public:
    UsuarioNoEncontradoException(const std::string& usuario)
        : AutenticacionException("Usuario no encontrado: " + usuario) {}
    std::string getTipo() const override { return "UsuarioNoEncontradoException"; }
};

class ContrasenaIncorrectaException : public AutenticacionException {
public:
    ContrasenaIncorrectaException()
        : AutenticacionException("Contrasena incorrecta") {}
    std::string getTipo() const override { return "ContrasenaIncorrectaException"; }
};

class SesionNoIniciadaException : public AutenticacionException {
public:
    SesionNoIniciadaException()
        : AutenticacionException("Debe iniciar sesion primero") {}
    std::string getTipo() const override { return "SesionNoIniciadaException"; }
};

class PermisoInsuficienteException : public AutenticacionException {
public:
    PermisoInsuficienteException(const std::string& accion)
        : AutenticacionException("Sin permiso para: " + accion) {}
    std::string getTipo() const override { return "PermisoInsuficienteException"; }
};

// ── Inventario ──────────────────────────────────────────────
class InventarioException : public TiendaException {
public:
    InventarioException(const std::string& msg)
        : TiendaException(msg, "Inventario") {}
    std::string getTipo() const override { return "InventarioException"; }
};

class StockInsuficienteException : public InventarioException {
    int disponible, solicitado;
public:
    StockInsuficienteException(int disp, int sol)
        : InventarioException("Stock insuficiente. Disponible: " +
                              std::to_string(disp) + ", Solicitado: " + std::to_string(sol)),
          disponible(disp), solicitado(sol) {}
    int getDisponible() const { return disponible; }
    int getSolicitado() const { return solicitado; }
    std::string getTipo() const override { return "StockInsuficienteException"; }
};

class ProductoNoEncontradoException : public InventarioException {
public:
    ProductoNoEncontradoException(int codigo)
        : InventarioException("Producto no encontrado. Codigo: " + std::to_string(codigo)) {}
    ProductoNoEncontradoException(const std::string& nombre)
        : InventarioException("Producto no encontrado: " + nombre) {}
    std::string getTipo() const override { return "ProductoNoEncontradoException"; }
};

class ProductoDuplicadoException : public InventarioException {
public:
    ProductoDuplicadoException(const std::string& nombre)
        : InventarioException("Producto ya existe: " + nombre) {}
    std::string getTipo() const override { return "ProductoDuplicadoException"; }
};

// ── Venta ───────────────────────────────────────────────────
class VentaException : public TiendaException {
public:
    VentaException(const std::string& msg)
        : TiendaException(msg, "Ventas") {}
    std::string getTipo() const override { return "VentaException"; }
};

class VentaVaciaException : public VentaException {
public:
    VentaVaciaException()
        : VentaException("No se puede registrar una venta sin productos") {}
    std::string getTipo() const override { return "VentaVaciaException"; }
};

// ── Finanzas ────────────────────────────────────────────────
class FinanzasException : public TiendaException {
public:
    FinanzasException(const std::string& msg)
        : TiendaException(msg, "Finanzas") {}
    std::string getTipo() const override { return "FinanzasException"; }
};

class TipoMovimientoInvalidoException : public FinanzasException {
public:
    TipoMovimientoInvalidoException(const std::string& tipo)
        : FinanzasException("Tipo de movimiento invalido: " + tipo) {}
    std::string getTipo() const override { return "TipoMovimientoInvalidoException"; }
};

// ── Base de Datos ───────────────────────────────────────────
class BaseDatosException : public TiendaException {
public:
    BaseDatosException(const std::string& msg)
        : TiendaException(msg, "BaseDatos") {}
    std::string getTipo() const override { return "BaseDatosException"; }
};
