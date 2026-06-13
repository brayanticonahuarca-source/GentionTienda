#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <ctime>
#include "Venta.h"
#include "Finanzas.h"
#include "Inventario.h"

// ============================================================
//  Reporte  — clase base abstracta
//  Herencia + clases abstractas + funciones virtuales
// ============================================================
class Reporte {
protected:
    std::string titulo;
    std::string periodo;   // fecha o mes

    static std::string fechaActual() {
        std::time_t t = std::time(nullptr);
        std::tm* tm = std::localtime(&t);
        char buf[16];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
        return std::string(buf);
    }

    static std::string mesActual() {
        std::time_t t = std::time(nullptr);
        std::tm* tm = std::localtime(&t);
        char buf[8];
        std::strftime(buf, sizeof(buf), "%Y-%m", tm);
        return std::string(buf);
    }

    void imprimirEncabezado() const {
        int ancho = 60;
        std::string borde(ancho, '=');
        std::cout << "\n" << borde << "\n";
        std::cout << std::setw((ancho + static_cast<int>(titulo.size())) / 2)
                  << titulo << "\n";
        std::cout << std::setw((ancho + static_cast<int>(periodo.size())) / 2)
                  << periodo << "\n";
        std::cout << borde << "\n";
    }

public:
    Reporte(const std::string& titulo, const std::string& periodo)
        : titulo(titulo), periodo(periodo) {}

    virtual ~Reporte() = default;

    // Interfaz abstracta obligatoria
    virtual void generar(const std::vector<Venta>& ventas,
                         const RegistroFinanciero& finanzas,
                         const Inventario& inventario) = 0;

    const std::string& getTitulo()  const { return titulo; }
    const std::string& getPeriodo() const { return periodo; }
};

// ============================================================
//  ReporteDiario : public Reporte
// ============================================================
class ReporteDiario : public Reporte {
private:
    std::string fecha;

public:
    explicit ReporteDiario(const std::string& f = "")
        : Reporte("REPORTE DIARIO", f.empty() ? fechaActual() : f),
          fecha(f.empty() ? fechaActual() : f) {
        // Validar formato YYYY-MM-DD basico
        if (fecha.size() != 10 || fecha[4] != '-' || fecha[7] != '-')
            throw FechaInvalidaException("Formato esperado: YYYY-MM-DD");
    }

    void generar(const std::vector<Venta>& ventas,
                 const RegistroFinanciero& finanzas,
                 const Inventario& inventario) override {
        imprimirEncabezado();

        // ── Ventas del dia ───────────────────────────────────
        std::cout << "\n  VENTAS DEL DIA\n  " << std::string(56, '-') << "\n";
        double totalVentas = 0.0;
        int    numVentas   = 0;
        int    totalItems  = 0;

        for (const Venta& v : ventas) {
            if (v.getFecha() == fecha && v.estaFinalizada()) {
                std::cout << "  " << v << "\n";
                totalVentas += v.calcularTotal();
                totalItems  += v.cantidadItems();
                ++numVentas;
            }
        }

        if (numVentas == 0) std::cout << "  (Sin ventas en esta fecha)\n";

        // ── Movimientos financieros del dia ──────────────────
        std::cout << "\n  MOVIMIENTOS FINANCIEROS\n  " << std::string(56, '-') << "\n";
        auto movsDia = finanzas.filtrarPorFecha(fecha);
        double ingresos = 0, egresos = 0;
        for (MovimientoFinanciero* m : movsDia) {
            std::cout << "  " << *m << "\n";
            if (m->getSigno() == '+') ingresos += m->getMonto();
            else                      egresos  += m->getMonto();
        }
        if (movsDia.empty()) std::cout << "  (Sin movimientos)\n";

        // ── Resumen ──────────────────────────────────────────
        std::cout << "\n  RESUMEN DEL DIA\n  " << std::string(56, '-') << "\n";
        double gananciaNeta = totalVentas + ingresos - egresos;

        auto fila = [](const std::string& label, double val) {
            std::cout << "  " << std::left << std::setw(28) << label
                      << "S/ " << std::right << std::setw(10)
                      << std::fixed << std::setprecision(2) << val << "\n";
        };

        fila("Ventas (" + std::to_string(numVentas) + " transacciones):", totalVentas);
        fila("Inversiones/Ingresos:", ingresos);
        fila("Gastos/Egresos:", egresos);
        std::cout << "  " << std::string(42, '-') << "\n";
        fila("GANANCIA NETA DEL DIA:", gananciaNeta);
        std::cout << "  " << std::string(56, '=') << "\n\n";
    }
};

// ============================================================
//  ReporteMensual : public Reporte
// ============================================================
class ReporteMensual : public Reporte {
private:
    std::string mes; // "YYYY-MM"

public:
    explicit ReporteMensual(const std::string& m = "")
        : Reporte("REPORTE MENSUAL", m.empty() ? mesActual() : m),
          mes(m.empty() ? mesActual() : m) {
        if (mes.size() != 7 || mes[4] != '-')
            throw FechaInvalidaException("Formato esperado: YYYY-MM");
    }

    void generar(const std::vector<Venta>& ventas,
                 const RegistroFinanciero& finanzas,
                 const Inventario& inventario) override {
        imprimirEncabezado();

        // ── Ventas del mes ───────────────────────────────────
        std::cout << "\n  RESUMEN DE VENTAS\n  " << std::string(56, '-') << "\n";
        double totalVentas   = 0.0;
        int    numVentas     = 0;
        double ventaMaxima   = 0.0;
        std::string diaMaximo;

        // Conteo por dia para encontrar el dia con mas ventas
        std::map<std::string, double> ventasPorDia;

        for (const Venta& v : ventas) {
            if (v.getFecha().find(mes) == 0 && v.estaFinalizada()) {
                double tot = v.calcularTotal();
                totalVentas += tot;
                ++numVentas;
                ventasPorDia[v.getFecha()] += tot;
            }
        }

        for (const auto& par : ventasPorDia) {
            if (par.second > ventaMaxima) {
                ventaMaxima = par.second;
                diaMaximo   = par.first;
            }
        }

        // ── Movimientos del mes ──────────────────────────────
        auto movsMes = finanzas.filtrarPorFecha(mes);
        double inversiones = 0, gastos = 0;
        for (MovimientoFinanciero* m : movsMes) {
            if (m->getSigno() == '+') inversiones += m->getMonto();
            else                      gastos      += m->getMonto();
        }

        // ── Inventario actual ────────────────────────────────
        double valorInventario = inventario.calcularValorTotalInventario();

        // ── Imprimir resumen ─────────────────────────────────
        auto fila = [](const std::string& label, double val) {
            std::cout << "  " << std::left << std::setw(32) << label
                      << "S/ " << std::right << std::setw(12)
                      << std::fixed << std::setprecision(2) << val << "\n";
        };

        std::cout << "  Transacciones del mes: " << numVentas << "\n";
        if (!diaMaximo.empty())
            std::cout << "  Mejor dia: " << diaMaximo
                      << " (S/ " << std::fixed << std::setprecision(2) << ventaMaxima << ")\n";

        std::cout << "\n  ESTADO FINANCIERO\n  " << std::string(56, '-') << "\n";
        fila("Ingresos por ventas:", totalVentas);
        fila("Inversiones recibidas:", inversiones);
        fila("Gastos del mes:", gastos);
        std::cout << "  " << std::string(48, '-') << "\n";
        double gananciaNeta = totalVentas + inversiones - gastos;
        fila("GANANCIA NETA DEL MES:", gananciaNeta);

        std::cout << "\n  VALOR ACTUAL DEL INVENTARIO\n  " << std::string(56, '-') << "\n";
        fila("Valor en stock:", valorInventario);

        std::cout << "\n  PRODUCTOS CON BAJO STOCK\n  " << std::string(56, '-') << "\n";
        inventario.listarConStockBajo(5);

        std::cout << "  " << std::string(56, '=') << "\n\n";
    }
};
