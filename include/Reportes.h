#pragma once

#include <string>
#include <vector>
#include <cctype>

#include "Venta.h"
#include "Finanzas.h"
#include "Excepciones.h"

using namespace std;

//==================================================
// ESTRUCTURA DE RESULTADO PARA REPORTES
//==================================================

struct ResumenReporte
{
    string titulo;
    string periodo;
    int cantidadVentas;
    double totalVendido;
    double totalGastos;
    double totalInversiones;
    double gananciaNeta;
};

//==================================================
// CLASE ABSTRACTA REPORTE
//==================================================

class Reporte
{
public:

    virtual ResumenReporte generar() const = 0;

    virtual ~Reporte()
    {
    }
};

//==================================================
// REPORTE DIARIO
//==================================================

class ReporteDiario : public Reporte
{
private:

    const RegistroVentas& registroVentas;
    const RegistroFinanciero& finanzas;
    string fecha;

    void validarFecha() const
    {
        if (fecha.size() != 10)
        {
            throw FechaInvalidaException(
                "La fecha debe tener formato YYYY-MM-DD."
            );
        }

        if (fecha[4] != '-' || fecha[7] != '-')
        {
            throw FechaInvalidaException(
                "Formato de fecha invalido. Use YYYY-MM-DD."
            );
        }

        for (int i = 0; i < 10; i++)
        {
            if (i == 4 || i == 7)
            {
                continue;
            }

            if (!isdigit(static_cast<unsigned char>(fecha[i])))
            {
                throw FechaInvalidaException(
                    "La fecha solo debe contener numeros y guiones."
                );
            }
        }
    }

    double calcularGastosDelDia() const
    {
        double total = 0.0;

        vector<Gasto> gastos = finanzas.obtenerGastos();

        for (const Gasto& gasto : gastos)
        {
            if (gasto.obtenerFecha() == fecha)
            {
                total += gasto.obtenerMonto();
            }
        }

        return total;
    }

    double calcularInversionesDelDia() const
    {
        double total = 0.0;

        vector<Inversion> inversiones = finanzas.obtenerInversiones();

        for (const Inversion& inversion : inversiones)
        {
            if (inversion.obtenerFecha() == fecha)
            {
                total += inversion.obtenerMonto();
            }
        }

        return total;
    }

public:

    ReporteDiario(
        const RegistroVentas& registro,
        const RegistroFinanciero& registroFinanciero,
        const string& fechaReporte
    )
        : registroVentas(registro),
          finanzas(registroFinanciero),
          fecha(fechaReporte)
    {
        validarFecha();
    }

    ResumenReporte generar() const override
    {
        vector<Venta> ventasDelDia =
            registroVentas.obtenerVentasPorFecha(fecha);

        ResumenReporte resumen;

        resumen.titulo = "Reporte Diario";
        resumen.periodo = fecha;
        resumen.cantidadVentas = static_cast<int>(ventasDelDia.size());
        resumen.totalVendido = registroVentas.calcularTotalPorFecha(fecha);
        resumen.totalGastos = calcularGastosDelDia();
        resumen.totalInversiones = calcularInversionesDelDia();

        resumen.gananciaNeta =
            resumen.totalVendido
            - resumen.totalGastos
            - resumen.totalInversiones;

        return resumen;
    }
};

//==================================================
// REPORTE MENSUAL
//==================================================

class ReporteMensual : public Reporte
{
private:

    const RegistroVentas& registroVentas;
    const RegistroFinanciero& finanzas;
    string mes; // formato YYYY-MM

    void validarMes() const
    {
        if (mes.size() != 7)
        {
            throw FechaInvalidaException(
                "El mes debe tener formato YYYY-MM."
            );
        }

        if (mes[4] != '-')
        {
            throw FechaInvalidaException(
                "Formato de mes invalido. Use YYYY-MM."
            );
        }

        for (int i = 0; i < 7; i++)
        {
            if (i == 4)
            {
                continue;
            }

            if (!isdigit(static_cast<unsigned char>(mes[i])))
            {
                throw FechaInvalidaException(
                    "El mes solo debe contener numeros y guion."
                );
            }
        }
    }

    double calcularGastosDelMes() const
    {
        double total = 0.0;

        vector<Gasto> gastos = finanzas.obtenerGastos();

        for (const Gasto& gasto : gastos)
        {
            string fechaGasto = gasto.obtenerFecha();

            if (fechaGasto.size() >= 7 &&
                fechaGasto.substr(0, 7) == mes)
            {
                total += gasto.obtenerMonto();
            }
        }

        return total;
    }

    double calcularInversionesDelMes() const
    {
        double total = 0.0;

        vector<Inversion> inversiones = finanzas.obtenerInversiones();

        for (const Inversion& inversion : inversiones)
        {
            string fechaInversion = inversion.obtenerFecha();

            if (fechaInversion.size() >= 7 &&
                fechaInversion.substr(0, 7) == mes)
            {
                total += inversion.obtenerMonto();
            }
        }

        return total;
    }

public:

    ReporteMensual(
        const RegistroVentas& registro,
        const RegistroFinanciero& registroFinanciero,
        const string& mesReporte
    )
        : registroVentas(registro),
          finanzas(registroFinanciero),
          mes(mesReporte)
    {
        validarMes();
    }

    ResumenReporte generar() const override
    {
        vector<Venta> todasLasVentas =
            registroVentas.obtenerVentas();

        int cantidad = 0;
        double totalVentas = 0.0;

        for (const Venta& venta : todasLasVentas)
        {
            string fechaVenta = venta.obtenerFecha();

            if (fechaVenta.size() >= 7 &&
                fechaVenta.substr(0, 7) == mes)
            {
                cantidad++;
                totalVentas += venta.calcularTotal();
            }
        }

        ResumenReporte resumen;

        resumen.titulo = "Reporte Mensual";
        resumen.periodo = mes;
        resumen.cantidadVentas = cantidad;
        resumen.totalVendido = totalVentas;
        resumen.totalGastos = calcularGastosDelMes();
        resumen.totalInversiones = calcularInversionesDelMes();

        resumen.gananciaNeta =
            resumen.totalVendido
            - resumen.totalGastos
            - resumen.totalInversiones;

        return resumen;
    }
};
