#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include "Excepciones.h"

// ============================================================
//  ValidadorProducto
//  Clase de utilidad estatica. No se instancia.
//  Centraliza TODAS las reglas de validacion del dominio.
// ============================================================
class ValidadorProducto {
public:
    // Limites del dominio
    static constexpr double PRECIO_MIN  =  0.10;
    static constexpr double PRECIO_MAX  = 10000.00;
    static constexpr int    STOCK_MIN   = 0;
    static constexpr int    STOCK_MAX   = 50000;
    static constexpr int    NOMBRE_MIN  = 2;
    static constexpr int    NOMBRE_MAX  = 50;

    // Eliminar construccion: esta clase solo tiene metodos estaticos
    ValidadorProducto()  = delete;
    ~ValidadorProducto() = delete;

    // ── Validaciones booleanas (no lanzan) ──────────────────
    static bool esNombreValido(const std::string& nombre) {
        if (nombre.empty()) return false;
        int len = static_cast<int>(nombre.size());
        if (len < NOMBRE_MIN || len > NOMBRE_MAX) return false;
        // Debe tener al menos una letra
        bool tieneLetra = false;
        for (char c : nombre) {
            if (!std::isprint(static_cast<unsigned char>(c))) return false;
            if (std::isalpha(static_cast<unsigned char>(c))) tieneLetra = true;
        }
        return tieneLetra;
    }

    static bool esPrecioValido(double precio) {
        return precio >= PRECIO_MIN && precio <= PRECIO_MAX;
    }

    static bool esStockValido(int stock) {
        return stock >= STOCK_MIN && stock <= STOCK_MAX;
    }

    static bool esCantidadValida(int cantidad) {
        return cantidad > 0 && cantidad <= STOCK_MAX;
    }

    static bool esMontoValido(double monto) {
        return monto > 0.0 && monto <= 1'000'000.0;
    }

    // ── Validaciones con excepcion (lanzadoras) ─────────────
    static void validarNombre(const std::string& nombre) {
        if (nombre.empty())
            throw NombreInvalidoException("no puede estar vacio");
        if (static_cast<int>(nombre.size()) < NOMBRE_MIN)
            throw NombreInvalidoException("minimo " + std::to_string(NOMBRE_MIN) + " caracteres");
        if (static_cast<int>(nombre.size()) > NOMBRE_MAX)
            throw NombreInvalidoException("maximo " + std::to_string(NOMBRE_MAX) + " caracteres");
        bool tieneLetra = false;
        for (char c : nombre) {
            if (!std::isprint(static_cast<unsigned char>(c)))
                throw NombreInvalidoException("contiene caracteres no imprimibles");
            if (std::isalpha(static_cast<unsigned char>(c))) tieneLetra = true;
        }
        if (!tieneLetra)
            throw NombreInvalidoException("debe contener al menos una letra");
    }

    static void validarPrecio(double precio) {
        if (precio < PRECIO_MIN)
            throw PrecioInvalidoException("minimo S/ " + formatear(PRECIO_MIN));
        if (precio > PRECIO_MAX)
            throw PrecioInvalidoException("maximo S/ " + formatear(PRECIO_MAX));
    }

    static void validarStock(int stock) {
        if (stock < STOCK_MIN)
            throw StockInvalidoException("no puede ser negativo");
        if (stock > STOCK_MAX)
            throw StockInvalidoException("maximo " + std::to_string(STOCK_MAX) + " unidades");
    }

    static void validarCantidad(int cantidad) {
        if (cantidad <= 0)
            throw CantidadInvalidaException("debe ser mayor a cero");
        if (cantidad > STOCK_MAX)
            throw CantidadInvalidaException("excede el maximo permitido");
    }

    static void validarMonto(double monto, const std::string& campo = "Monto") {
        if (monto <= 0.0)
            throw MontoInvalidoException(campo + " debe ser mayor a cero");
        if (monto > 1'000'000.0)
            throw MontoInvalidoException(campo + " excede el limite permitido");
    }

    // ── Utilidades de formato ────────────────────────────────
    static std::string formatear(double valor, int decimales = 2) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(decimales) << valor;
        return ss.str();
    }

    static std::string trim(const std::string& s) {
        size_t inicio = s.find_first_not_of(" \t\n\r\f\v");
        if (inicio == std::string::npos) return "";
        size_t fin = s.find_last_not_of(" \t\n\r\f\v");
        return s.substr(inicio, fin - inicio + 1);
    }

    // Normaliza el nombre: trim + primera letra mayuscula por palabra
    static std::string normalizarNombre(const std::string& nombre) {
        std::string s = trim(nombre);
        bool nueva = true;
        for (char& c : s) {
            if (std::isspace(static_cast<unsigned char>(c))) {
                nueva = true;
            } else if (nueva) {
                c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
                nueva = false;
            } else {
                c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
        }
        return s;
    }
};

// ============================================================
//  EntradaSegura
//  Funciones para leer desde cin sin riesgo de romper el flujo
// ============================================================
namespace EntradaSegura {

    // Leer entero dentro de [min, max]. Lanza EntradaInvalidaException si falla.
    inline int leerEntero(const std::string& prompt, int min, int max) {
        std::string linea;
        std::cout << prompt;
        if (!std::getline(std::cin, linea))
            throw EntradaInvalidaException("Error al leer la entrada");

        linea = ValidadorProducto::trim(linea);
        if (linea.empty())
            throw EntradaInvalidaException("No se ingreso ningun valor");

        // Verificar que solo contenga digitos (y signo opcional al inicio)
        size_t inicio = (linea[0] == '-' || linea[0] == '+') ? 1 : 0;
        for (size_t i = inicio; i < linea.size(); ++i) {
            if (!std::isdigit(static_cast<unsigned char>(linea[i])))
                throw EntradaInvalidaException("'" + linea + "' no es un numero entero valido");
        }

        try {
            int valor = std::stoi(linea);
            if (valor < min || valor > max)
                throw EntradaInvalidaException(
                    "Debe estar entre " + std::to_string(min) + " y " + std::to_string(max));
            return valor;
        } catch (const std::out_of_range&) {
            throw EntradaInvalidaException("Numero demasiado grande o pequeno");
        }
    }

    // Leer double dentro de [min, max].
    inline double leerDouble(const std::string& prompt, double min, double max) {
        std::string linea;
        std::cout << prompt;
        if (!std::getline(std::cin, linea))
            throw EntradaInvalidaException("Error al leer la entrada");

        linea = ValidadorProducto::trim(linea);
        if (linea.empty())
            throw EntradaInvalidaException("No se ingreso ningun valor");

        try {
            size_t pos;
            double valor = std::stod(linea, &pos);
            if (pos != linea.size())
                throw EntradaInvalidaException("'" + linea + "' no es un numero valido");
            if (valor < min || valor > max)
                throw EntradaInvalidaException(
                    "Debe estar entre " + ValidadorProducto::formatear(min) +
                    " y " + ValidadorProducto::formatear(max));
            return valor;
        } catch (const std::invalid_argument&) {
            throw EntradaInvalidaException("'" + linea + "' no es un numero valido");
        } catch (const std::out_of_range&) {
            throw EntradaInvalidaException("Numero fuera de rango");
        }
    }

    // Leer string no vacio con longitud controlada.
    inline std::string leerTexto(const std::string& prompt, int minLen = 1, int maxLen = 200) {
        std::string linea;
        std::cout << prompt;
        if (!std::getline(std::cin, linea))
            throw EntradaInvalidaException("Error al leer la entrada");

        linea = ValidadorProducto::trim(linea);
        if (static_cast<int>(linea.size()) < minLen)
            throw EntradaInvalidaException("Texto muy corto (minimo " + std::to_string(minLen) + " caracteres)");
        if (static_cast<int>(linea.size()) > maxLen)
            throw EntradaInvalidaException("Texto muy largo (maximo " + std::to_string(maxLen) + " caracteres)");
        return linea;
    }

    // Leer opcion de menu: intento silencioso con reintento automatico.
    inline int leerOpcionMenu(int min, int max) {
        while (true) {
            try {
                return leerEntero("Opcion: ", min, max);
            } catch (const EntradaInvalidaException& e) {
                std::cout << "  [!] " << e.what() << ". Intente de nuevo.\n";
            }
        }
    }
}
