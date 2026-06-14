#pragma once

#include <string>
#include <vector>
#include <ctime>

#include "Excepciones.h"

using namespace std;

class MovimientoFinanciero
{
protected:
    inline static int siguienteId = 1;

    int id;
    string descripcion;
    double monto;
    string fecha;

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

    void validarDatos(const string& descripcionMovimiento, double montoMovimiento)
    {
        if (descripcionMovimiento.empty())
        {
            throw ExcepcionValidacion("La descripcion no puede estar vacia.");
        }

        if (montoMovimiento <= 0)
        {
            throw MontoInvalidoException("El monto debe ser mayor a cero.");
        }

        if (montoMovimiento > 1000000.00)
        {
            throw MontoInvalidoException("El monto excede el limite permitido.");
        }
    }

public:
    MovimientoFinanciero(
        const string& descripcionMovimiento,
        double montoMovimiento
    )
    {
        validarDatos(descripcionMovimiento, montoMovimiento);

        id = siguienteId++;
        descripcion = descripcionMovimiento;
        monto = montoMovimiento;
        fecha = obtenerFechaActual();
    }

    virtual string obtenerTipo() const = 0;

    int obtenerId() const
    {
        return id;
    }

    string obtenerDescripcion() const
    {
        return descripcion;
    }

    double obtenerMonto() const
    {
        return monto;
    }

    string obtenerFecha() const
    {
        return fecha;
    }

    virtual ~MovimientoFinanciero()
    {
    }
};

class Gasto : public MovimientoFinanciero
{
public:
    Gasto(const string& descripcionGasto, double montoGasto)
        : MovimientoFinanciero(descripcionGasto, montoGasto)
    {
    }

    string obtenerTipo() const override
    {
        return "GASTO";
    }
};

class Inversion : public MovimientoFinanciero
{
public:
    Inversion(const string& descripcionInversion, double montoInversion)
        : MovimientoFinanciero(descripcionInversion, montoInversion)
    {
    }

    string obtenerTipo() const override
    {
        return "INVERSION";
    }
};

class RegistroFinanciero
{
private:
    vector<Gasto> gastos;
    vector<Inversion> inversiones;

public:
    void registrarGasto(const string& descripcion, double monto)
    {
        Gasto nuevoGasto(descripcion, monto);
        gastos.push_back(nuevoGasto);
    }

    void registrarInversion(const string& descripcion, double monto)
    {
        Inversion nuevaInversion(descripcion, monto);
        inversiones.push_back(nuevaInversion);
    }

    vector<Gasto> obtenerGastos() const
    {
        return gastos;
    }

    vector<Inversion> obtenerInversiones() const
    {
        return inversiones;
    }

    double calcularTotalGastos() const
    {
        double total = 0.0;

        for (const Gasto& gasto : gastos)
        {
            total += gasto.obtenerMonto();
        }

        return total;
    }

    double calcularTotalInversiones() const
    {
        double total = 0.0;

        for (const Inversion& inversion : inversiones)
        {
            total += inversion.obtenerMonto();
        }

        return total;
    }
};
