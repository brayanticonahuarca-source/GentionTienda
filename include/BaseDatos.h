#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Excepciones.h"

// ============================================================
//  BaseDatos
//  En un entorno real se enlazaria con -lsqlite3.
//  Aqui implementamos persistencia en archivo de texto CSV
//  con la misma interfaz que tendria SQLite, de modo que
//  reemplazar el interior por llamadas a sqlite3_exec() es
//  directo sin cambiar el resto del sistema.
// ============================================================
class BaseDatos {
private:
    std::string rutaArchivo;
    bool        abierta;

    // Escapa comas dentro de un campo CSV
    static std::string escaparCSV(const std::string& s) {
        if (s.find(',') == std::string::npos &&
            s.find('"') == std::string::npos &&
            s.find('\n') == std::string::npos)
            return s;
        std::string r = "\"";
        for (char c : s) {
            if (c == '"') r += "\"\"";
            else          r += c;
        }
        r += "\"";
        return r;
    }

    // Divide una linea CSV respetando campos entre comillas
    static std::vector<std::string> parsearCSV(const std::string& linea) {
        std::vector<std::string> campos;
        std::string campo;
        bool entreComillas = false;
        for (size_t i = 0; i < linea.size(); ++i) {
            char c = linea[i];
            if (c == '"') {
                if (entreComillas && i + 1 < linea.size() && linea[i+1] == '"') {
                    campo += '"'; ++i;
                } else {
                    entreComillas = !entreComillas;
                }
            } else if (c == ',' && !entreComillas) {
                campos.push_back(campo);
                campo.clear();
            } else {
                campo += c;
            }
        }
        campos.push_back(campo);
        return campos;
    }

public:
    explicit BaseDatos(const std::string& ruta = "tienda.db")
        : rutaArchivo(ruta), abierta(false) {}

    ~BaseDatos() { cerrar(); }

    // ── Ciclo de vida ────────────────────────────────────────
    void abrir() {
        // Verificar que podemos escribir en el directorio
        std::ofstream prueba(rutaArchivo, std::ios::app);
        if (!prueba.is_open())
            throw BaseDatosException("No se puede abrir el archivo de base de datos: " + rutaArchivo);
        prueba.close();
        abierta = true;
    }

    void cerrar() {
        abierta = false;
    }

    bool estaAbierta() const { return abierta; }

    // ── Productos ────────────────────────────────────────────
    void guardarProducto(int codigo, const std::string& nombre,
                         double precio, int stock) {
        verificarAbierta();
        std::string archivo = "productos.csv";
        // Leer todos, reemplazar si existe, sino agregar
        std::vector<std::string> lineas = leerLineas(archivo);
        std::string nueva = std::to_string(codigo) + "," +
                            escaparCSV(nombre) + "," +
                            std::to_string(precio) + "," +
                            std::to_string(stock);
        bool encontrado = false;
        for (std::string& l : lineas) {
            auto campos = parsearCSV(l);
            if (!campos.empty() && campos[0] == std::to_string(codigo)) {
                l = nueva;
                encontrado = true;
                break;
            }
        }
        if (!encontrado) lineas.push_back(nueva);
        escribirLineas(archivo, lineas);
    }

    std::vector<std::vector<std::string>> cargarProductos() {
        verificarAbierta();
        return leerTabla("productos.csv");
    }

    void eliminarProducto(int codigo) {
        verificarAbierta();
        eliminarFila("productos.csv", std::to_string(codigo));
    }

    // ── Ventas ───────────────────────────────────────────────
    void guardarVenta(int id, const std::string& vendedor,
                      const std::string& fecha, const std::string& hora,
                      double total) {
        verificarAbierta();
        agregarFila("ventas.csv",
            std::to_string(id) + "," +
            escaparCSV(vendedor) + "," +
            fecha + "," + hora + "," +
            std::to_string(total));
    }

    void guardarDetalleVenta(int idVenta, int codProducto,
                              const std::string& nombre, int cantidad, double precio) {
        verificarAbierta();
        agregarFila("detalles_venta.csv",
            std::to_string(idVenta) + "," +
            std::to_string(codProducto) + "," +
            escaparCSV(nombre) + "," +
            std::to_string(cantidad) + "," +
            std::to_string(precio));
    }

    std::vector<std::vector<std::string>> cargarVentas() {
        verificarAbierta();
        return leerTabla("ventas.csv");
    }

    // ── Movimientos financieros ──────────────────────────────
    void guardarMovimiento(int id, const std::string& tipo,
                            double monto, const std::string& descripcion,
                            const std::string& categoria, const std::string& fecha) {
        verificarAbierta();
        agregarFila("movimientos.csv",
            std::to_string(id) + "," +
            tipo + "," +
            std::to_string(monto) + "," +
            escaparCSV(descripcion) + "," +
            escaparCSV(categoria) + "," +
            fecha);
    }

    std::vector<std::vector<std::string>> cargarMovimientos() {
        verificarAbierta();
        return leerTabla("movimientos.csv");
    }

    // ── Usuarios ─────────────────────────────────────────────
    void guardarUsuario(int id, const std::string& nombre,
                         const std::string& usuario, const std::string& hashPwd,
                         const std::string& rol, bool activo) {
        verificarAbierta();
        std::string archivo = "usuarios.csv";
        std::vector<std::string> lineas = leerLineas(archivo);
        std::string nueva = std::to_string(id) + "," +
                            escaparCSV(nombre) + "," +
                            usuario + "," +
                            hashPwd + "," +
                            rol + "," +
                            (activo ? "1" : "0");
        bool encontrado = false;
        for (std::string& l : lineas) {
            auto campos = parsearCSV(l);
            if (!campos.empty() && campos[0] == std::to_string(id)) {
                l = nueva; encontrado = true; break;
            }
        }
        if (!encontrado) lineas.push_back(nueva);
        escribirLineas(archivo, lineas);
    }

    std::vector<std::vector<std::string>> cargarUsuarios() {
        verificarAbierta();
        return leerTabla("usuarios.csv");
    }

private:
    void verificarAbierta() const {
        if (!abierta)
            throw BaseDatosException("La base de datos no esta abierta");
    }

    std::vector<std::string> leerLineas(const std::string& archivo) const {
        std::vector<std::string> lineas;
        std::ifstream f(archivo);
        if (!f.is_open()) return lineas;  // archivo nuevo -> vacio
        std::string linea;
        while (std::getline(f, linea))
            if (!linea.empty()) lineas.push_back(linea);
        return lineas;
    }

    void escribirLineas(const std::string& archivo,
                        const std::vector<std::string>& lineas) const {
        std::ofstream f(archivo, std::ios::trunc);
        if (!f.is_open())
            throw BaseDatosException("No se puede escribir en: " + archivo);
        for (const std::string& l : lineas)
            f << l << "\n";
    }

    void agregarFila(const std::string& archivo, const std::string& fila) const {
        std::ofstream f(archivo, std::ios::app);
        if (!f.is_open())
            throw BaseDatosException("No se puede escribir en: " + archivo);
        f << fila << "\n";
    }

    void eliminarFila(const std::string& archivo, const std::string& id) const {
        auto lineas = leerLineas(archivo);
        std::vector<std::string> restantes;
        for (const std::string& l : lineas) {
            auto campos = parsearCSV(l);
            if (campos.empty() || campos[0] != id)
                restantes.push_back(l);
        }
        escribirLineas(archivo, restantes);
    }

    std::vector<std::vector<std::string>> leerTabla(const std::string& archivo) const {
        std::vector<std::vector<std::string>> tabla;
        for (const std::string& linea : leerLineas(archivo)) {
            auto campos = parsearCSV(linea);
            if (!campos.empty()) tabla.push_back(campos);
        }
        return tabla;
    }
};
