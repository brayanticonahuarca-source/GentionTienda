#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <vector>
#include "Validador.h"

// ============================================================
//  MovimientoFinanciero  — clase base abstracta
//  Herencia + clases abstractas + funciones virtuales
//  Integrante 3 – Finanzas y Reportes
// ============================================================
class MovimientoFinanciero {
protected:
    static int siguienteId;

    int         id;
    double      monto;
    std::string descripcion;
    std::string fecha;
    std::string categoria;

    static std::string fechaActual() {
        std::time_t t = std::time(nullptr);
        std::tm* tm = std::localtime(&t);
        char buf[16];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
        return std::string(buf);
    }

public:
    MovimientoFinanciero(double monto, const std::string& desc,
                         const std::string& cat, const std::string& fecha = "")
        : id(siguienteId++) {
        ValidadorProducto::validarMonto(monto, "Monto del movimiento");
        if (desc.empty() || desc.size() < 3)
            throw ValidacionException("La descripcion debe tener al menos 3 caracteres");
        if (desc.size() > 100)
            throw ValidacionException("La descripcion no puede superar 100 caracteres");
        this->monto       = monto;
        this->descripcion = desc;
        this->categoria   = cat.empty() ? "General" : cat;
        this->fecha       = fecha.empty() ? fechaActual() : fecha;
    }

    virtual ~MovimientoFinanciero() = default;

    // ── Getters ──────────────────────────────────────────────
    int                getId()          const { return id;          }
    double             getMonto()       const { return monto;        }
    const std::string& getDescripcion() const { return descripcion;  }
    const std::string& getFecha()       const { return fecha;        }
    const std::string& getCategoria()   const { return categoria;    }

    // ── Interfaz abstracta ───────────────────────────────────
    virtual std::string getTipo()    const = 0;
    virtual char        getSigno()   const = 0;   // '+' o '-'
    virtual void        imprimir()   const = 0;

    // Metodo virtual con implementacion base (puede sobreescribirse)
    virtual double getImpactoEnCaja() const {
        return (getSigno() == '+') ? monto : -monto;
    }

    // Funcion amiga para operador de salida
    friend std::ostream& operator<<(std::ostream& os, const MovimientoFinanciero& m) {
        os << m.getSigno()
           << " [" << std::setw(4) << m.id << "] "
           << std::setw(12) << m.getTipo()
           << " " << m.fecha
           << " S/ " << std::setw(10) << std::fixed << std::setprecision(2) << m.monto
           << " | " << m.descripcion;
        return os;
    }
};

inline int MovimientoFinanciero::siguienteId = 1;

// ============================================================
//  Inversion : public MovimientoFinanciero
//  Representa dinero que entra (compra de mercaderia, capital)
// ============================================================
class Inversion : public MovimientoFinanciero {
private:
    std::string proveedor;
    int         cantidadUnidades;

public:
    Inversion(double monto, const std::string& desc,
              const std::string& proveedor = "", int unidades = 0,
              const std::string& cat = "Inversion", const std::string& fecha = "")
        : MovimientoFinanciero(monto, desc, cat, fecha) {
        if (!proveedor.empty() && proveedor.size() > 60)
            throw ValidacionException("Nombre de proveedor muy largo");
        if (unidades < 0)
            throw ValidacionException("Las unidades no pueden ser negativas");
        this->proveedor         = proveedor;
        this->cantidadUnidades  = unidades;
    }

    std::string getTipo()  const override { return "Inversion"; }
    char        getSigno() const override { return '+'; }

    const std::string& getProveedor()  const { return proveedor; }
    int                getUnidades()   const { return cantidadUnidades; }

    void imprimir() const override {
        std::cout << "  [INVERSION #" << id << "] " << fecha << "\n"
                  << "  Descripcion : " << descripcion << "\n"
                  << "  Categoria   : " << categoria << "\n"
                  << "  Monto       : S/ " << std::fixed << std::setprecision(2) << monto << "\n";
        if (!proveedor.empty())
            std::cout << "  Proveedor   : " << proveedor << "\n";
        if (cantidadUnidades > 0)
            std::cout << "  Unidades    : " << cantidadUnidades << "\n";
    }
};

// ============================================================
//  Gasto : public MovimientoFinanciero
//  Representa dinero que sale (servicios, sueldos, etc.)
// ============================================================
class Gasto : public MovimientoFinanciero {
private:
    bool esFijo;   // gasto fijo vs variable

public:
    Gasto(double monto, const std::string& desc,
          bool fijo = false,
          const std::string& cat = "Gasto", const std::string& fecha = "")
        : MovimientoFinanciero(monto, desc, cat, fecha), esFijo(fijo) {}

    std::string getTipo()  const override { return "Gasto"; }
    char        getSigno() const override { return '-'; }
    bool        esFijoQ()  const { return esFijo; }

    // Los gastos restan de la caja (override para enfatizar semantica)
    double getImpactoEnCaja() const override { return -monto; }

    void imprimir() const override {
        std::cout << "  [GASTO #" << id << "] " << fecha << "\n"
                  << "  Descripcion : " << descripcion << "\n"
                  << "  Categoria   : " << categoria << "\n"
                  << "  Tipo        : " << (esFijo ? "Fijo" : "Variable") << "\n"
                  << "  Monto       : S/ " << std::fixed << std::setprecision(2) << monto << "\n";
    }
};

// ============================================================
//  RegistroFinanciero  — contiene todos los movimientos
//  Usa binding dinamico via punteros a MovimientoFinanciero
// ============================================================
#include <memory>

class RegistroFinanciero {
private:
    // Usamos unique_ptr para ownership seguro (sin memory leaks)
    std::vector<std::unique_ptr<MovimientoFinanciero>> movimientos;

public:
    ~RegistroFinanciero() = default;

    void agregarInversion(double monto, const std::string& desc,
                          const std::string& proveedor = "", int unidades = 0,
                          const std::string& cat = "Inversion") {
        movimientos.push_back(
            std::make_unique<Inversion>(monto, desc, proveedor, unidades, cat));
    }

    void agregarGasto(double monto, const std::string& desc,
                      bool fijo = false, const std::string& cat = "Gasto") {
        movimientos.push_back(
            std::make_unique<Gasto>(monto, desc, fijo, cat));
    }

    // Binding dinamico: cada movimiento sabe como imprimirse
    void listarTodos() const {
        if (movimientos.empty()) {
            std::cout << "  (No hay movimientos registrados)\n";
            return;
        }
        std::cout << std::string(70, '-') << "\n";
        for (const auto& m : movimientos) {
            m->imprimir();
            std::cout << std::string(70, '-') << "\n";
        }
    }

    double calcularTotalInversiones() const {
        double total = 0;
        for (const auto& m : movimientos)
            if (m->getSigno() == '+') total += m->getMonto();
        return total;
    }

    double calcularTotalGastos() const {
        double total = 0;
        for (const auto& m : movimientos)
            if (m->getSigno() == '-') total += m->getMonto();
        return total;
    }

    double calcularBalance() const {
        double bal = 0;
        for (const auto& m : movimientos)
            bal += m->getImpactoEnCaja();   // binding dinamico
        return bal;
    }

    // Filtrar por fecha (para reportes diarios/mensuales)
    std::vector<MovimientoFinanciero*> filtrarPorFecha(const std::string& prefijo) const {
        std::vector<MovimientoFinanciero*> resultado;
        for (const auto& m : movimientos)
            if (m->getFecha().find(prefijo) == 0)
                resultado.push_back(m.get());
        return resultado;
    }

    const std::vector<std::unique_ptr<MovimientoFinanciero>>& obtenerTodos() const {
        return movimientos;
    }
};
