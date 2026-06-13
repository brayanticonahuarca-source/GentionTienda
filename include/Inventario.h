#pragma once
#include <vector>
#include <algorithm>
#include <functional>
#include <optional>
#include "Producto.h"

// ============================================================
//  Repositorio<T>  — Plantilla generica de coleccion
//  Cubre el tema: plantillas de clases (programacion generica)
// ============================================================
template<typename T>
class Repositorio {
protected:
    std::vector<T> elementos;

public:
    virtual ~Repositorio() = default;

    void agregar(const T& elemento) {
        elementos.push_back(elemento);
    }

    void agregar(T&& elemento) {
        elementos.push_back(std::move(elemento));
    }

    int contar() const {
        return static_cast<int>(elementos.size());
    }

    bool estaVacio() const {
        return elementos.empty();
    }

    const std::vector<T>& obtenerTodos() const {
        return elementos;
    }

    std::vector<T>& obtenerTodos() {
        return elementos;
    }

    // Busqueda generica por predicado
    std::optional<T*> buscar(std::function<bool(const T&)> predicado) {
        for (T& e : elementos)
            if (predicado(e)) return &e;
        return std::nullopt;
    }

    // Filtrar: devuelve copia de los que cumplen el predicado
    std::vector<T> filtrar(std::function<bool(const T&)> predicado) const {
        std::vector<T> resultado;
        for (const T& e : elementos)
            if (predicado(e)) resultado.push_back(e);
        return resultado;
    }

    // Eliminar por predicado
    bool eliminar(std::function<bool(const T&)> predicado) {
        auto it = std::remove_if(elementos.begin(), elementos.end(), predicado);
        if (it == elementos.end()) return false;
        elementos.erase(it, elementos.end());
        return true;
    }

    // Ordenar
    void ordenar(std::function<bool(const T&, const T&)> comparador) {
        std::sort(elementos.begin(), elementos.end(), comparador);
    }
};

// ============================================================
//  Inventario : hereda de Repositorio<Producto>
//  Agrega logica especifica de productos
// ============================================================
class Inventario : public Repositorio<Producto> {
public:
    // ── Busquedas especificas ────────────────────────────────
    Producto* buscarPorCodigo(int codigo) {
        auto res = buscar([codigo](const Producto& p){ return p.getCodigo() == codigo; });
        if (!res.has_value())
            throw ProductoNoEncontradoException(codigo);
        return res.value();
    }

    Producto* buscarPorNombre(const std::string& nombre) {
        std::string buscado = ValidadorProducto::normalizarNombre(nombre);
        auto res = buscar([&buscado](const Producto& p){ return p.getNombre() == buscado; });
        if (!res.has_value())
            throw ProductoNoEncontradoException(nombre);
        return res.value();
    }

    bool existeNombre(const std::string& nombre) const {
        std::string norm = ValidadorProducto::normalizarNombre(nombre);
        for (const Producto& p : elementos)
            if (p.getNombre() == norm) return true;
        return false;
    }

    bool existeCodigo(int codigo) const {
        for (const Producto& p : elementos)
            if (p.getCodigo() == codigo) return true;
        return false;
    }

    // ── Operaciones de negocio ───────────────────────────────
    void registrarProducto(const std::string& nombre, double precio, int stock) {
        if (existeNombre(nombre))
            throw ProductoDuplicadoException(nombre);
        agregar(Producto(nombre, precio, stock));
    }

    void actualizarPrecio(int codigo, double nuevoPrecio) {
        buscarPorCodigo(codigo)->setPrecio(nuevoPrecio);
    }

    void actualizarStock(int codigo, int nuevoStock) {
        buscarPorCodigo(codigo)->setStock(nuevoStock);
    }

    void descontarStock(int codigo, int cantidad) {
        buscarPorCodigo(codigo)->descontarStock(cantidad);
    }

    void agregarStock(int codigo, int cantidad) {
        buscarPorCodigo(codigo)->agregarStock(cantidad);
    }

    // ── Listados ─────────────────────────────────────────────
    void listarTodos() const {
        if (elementos.empty()) {
            std::cout << "  (No hay productos registrados)\n";
            return;
        }
        Producto::imprimirCabecera();
        for (const Producto& p : elementos)
            p.imprimirFila();
        std::cout << std::string(72, '-') << "\n";
        std::cout << "Total: " << elementos.size() << " producto(s)\n";
    }

    void listarConStockBajo(int umbral = 5) const {
        bool alguno = false;
        for (const Producto& p : elementos) {
            if (p.getStock() <= umbral) {
                if (!alguno) {
                    std::cout << "  Productos con stock bajo (<=  " << umbral << "):\n";
                    Producto::imprimirCabecera();
                    alguno = true;
                }
                p.imprimirFila();
            }
        }
        if (!alguno) std::cout << "  Ningun producto con stock bajo.\n";
    }

    double calcularValorTotalInventario() const {
        double total = 0;
        for (const Producto& p : elementos)
            total += p.calcularValorTotal();
        return total;
    }
};
