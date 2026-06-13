#pragma once
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include "Excepciones.h"
#include "Validador.h"

// ============================================================
//  Usuario  — clase base abstracta
//  Herencia + encapsulamiento + miembros protected
// ============================================================
class Usuario {
protected:
    int         id;
    std::string nombre;
    std::string usuario;
    std::string contrasena;     // en prod. se guardaria el hash
    bool        activo;

    static int siguienteId;

    // Hash simple (XOR + acumulacion). En produccion: bcrypt/SHA-256
    static std::string hashSimple(const std::string& s) {
        unsigned long h = 5381;
        for (char c : s)
            h = ((h << 5) + h) ^ static_cast<unsigned char>(c);
        return std::to_string(h);
    }

    static void validarUsuario(const std::string& u) {
        if (u.size() < 4 || u.size() > 20)
            throw ValidacionException("El usuario debe tener entre 4 y 20 caracteres");
        for (char c : u)
            if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_')
                throw ValidacionException("El usuario solo puede contener letras, numeros y '_'");
    }

    static void validarContrasena(const std::string& p) {
        if (p.size() < 6)
            throw ValidacionException("La contrasena debe tener al menos 6 caracteres");
        if (p.size() > 50)
            throw ValidacionException("La contrasena no puede superar 50 caracteres");
        bool tieneNum = false, tieneLetra = false;
        for (char c : p) {
            if (std::isdigit(static_cast<unsigned char>(c))) tieneNum   = true;
            if (std::isalpha(static_cast<unsigned char>(c))) tieneLetra = true;
        }
        if (!tieneNum || !tieneLetra)
            throw ValidacionException("La contrasena debe contener letras y numeros");
    }

public:
    Usuario(const std::string& nombre, const std::string& usuario,
            const std::string& contrasena)
        : id(siguienteId++), activo(true) {
        ValidadorProducto::validarNombre(nombre);
        validarUsuario(usuario);
        validarContrasena(contrasena);

        this->nombre     = ValidadorProducto::trim(nombre);
        this->usuario    = usuario;
        this->contrasena = hashSimple(contrasena);   // guardar hash
    }

    // Constructor para cargar desde BD (contrasena ya hasheada)
    Usuario(int id, const std::string& nombre, const std::string& usuario,
            const std::string& hashContrasena, bool activo)
        : id(id), nombre(nombre), usuario(usuario),
          contrasena(hashContrasena), activo(activo) {
        if (id >= siguienteId) siguienteId = id + 1;
    }

    virtual ~Usuario() = default;

    // ── Autenticacion ────────────────────────────────────────
    bool verificarContrasena(const std::string& intento) const {
        return contrasena == hashSimple(intento);
    }

    // ── Interfaz abstracta ───────────────────────────────────
    virtual std::string getRol()          const = 0;
    virtual bool        puedeEliminar()   const = 0;
    virtual bool        puedeVerReportes()const = 0;

    // ── Getters ──────────────────────────────────────────────
    int                getId()      const { return id;      }
    const std::string& getNombre()  const { return nombre;  }
    const std::string& getUsuario() const { return usuario; }
    bool               estaActivo() const { return activo;  }

    void desactivar() { activo = false; }
    void activar()    { activo = true;  }

    void cambiarContrasena(const std::string& actual, const std::string& nueva) {
        if (!verificarContrasena(actual))
            throw ContrasenaIncorrectaException();
        validarContrasena(nueva);
        contrasena = hashSimple(nueva);
        std::cout << "  Contrasena actualizada correctamente.\n";
    }

    friend std::ostream& operator<<(std::ostream& os, const Usuario& u) {
        os << "[" << u.id << "] " << std::left << std::setw(20) << u.nombre
           << " @" << std::setw(15) << u.usuario
           << " Rol: " << u.getRol()
           << (u.activo ? "" : " [INACTIVO]");
        return os;
    }
};

inline int Usuario::siguienteId = 1;

// ============================================================
//  Administrador : public Usuario
// ============================================================
class Administrador : public Usuario {
public:
    Administrador(const std::string& nombre, const std::string& usuario,
                  const std::string& contrasena)
        : Usuario(nombre, usuario, contrasena) {}

    Administrador(int id, const std::string& nombre, const std::string& usuario,
                  const std::string& hash, bool activo)
        : Usuario(id, nombre, usuario, hash, activo) {}

    std::string getRol()           const override { return "Administrador"; }
    bool        puedeEliminar()    const override { return true;  }
    bool        puedeVerReportes() const override { return true;  }
};

// ============================================================
//  Empleado : public Usuario
// ============================================================
class Empleado : public Usuario {
private:
    std::string turno;   // "Manana", "Tarde", "Noche"

public:
    Empleado(const std::string& nombre, const std::string& usuario,
             const std::string& contrasena, const std::string& turno = "Manana")
        : Usuario(nombre, usuario, contrasena) {
        if (turno != "Manana" && turno != "Tarde" && turno != "Noche")
            throw ValidacionException("Turno invalido. Use: Manana, Tarde o Noche");
        this->turno = turno;
    }

    Empleado(int id, const std::string& nombre, const std::string& usuario,
             const std::string& hash, bool activo, const std::string& turno)
        : Usuario(id, nombre, usuario, hash, activo), turno(turno) {}

    std::string getRol()           const override { return "Empleado"; }
    bool        puedeEliminar()    const override { return false; }
    bool        puedeVerReportes() const override { return false; }
    const std::string& getTurno()  const { return turno; }
};

// ============================================================
//  SistemaLogin  — maneja sesion activa
// ============================================================
#include <memory>
#include <vector>

class SistemaLogin {
private:
    std::vector<std::unique_ptr<Usuario>> usuarios;
    Usuario* usuarioActual = nullptr;

    static constexpr int MAX_INTENTOS = 3;

public:
    SistemaLogin() {
        // Crear administrador por defecto
        usuarios.push_back(
            std::make_unique<Administrador>("Administrador", "admin", "admin123"));
    }

    void agregarUsuario(std::unique_ptr<Usuario> u) {
        // Verificar que el nombre de usuario no exista
        for (const auto& existente : usuarios)
            if (existente->getUsuario() == u->getUsuario())
                throw ValidacionException("El nombre de usuario ya existe: " + u->getUsuario());
        usuarios.push_back(std::move(u));
    }

    // Intenta login; lanza excepcion con detalle correcto
    Usuario* iniciarSesion(const std::string& nombreUsuario,
                            const std::string& contrasena) {
        // Buscar usuario
        for (const auto& u : usuarios) {
            if (u->getUsuario() == nombreUsuario) {
                if (!u->estaActivo())
                    throw AutenticacionException("La cuenta esta desactivada");
                if (!u->verificarContrasena(contrasena))
                    throw ContrasenaIncorrectaException();
                usuarioActual = u.get();
                return usuarioActual;
            }
        }
        throw UsuarioNoEncontradoException(nombreUsuario);
    }

    // Login interactivo con limite de intentos
    Usuario* loginInteractivo() {
        int intentos = 0;
        while (intentos < MAX_INTENTOS) {
            try {
                std::string usr = EntradaSegura::leerTexto("  Usuario   : ", 1, 20);
                std::string pwd = EntradaSegura::leerTexto("  Contrasena: ", 1, 50);
                return iniciarSesion(usr, pwd);
            } catch (const UsuarioNoEncontradoException& e) {
                ++intentos;
                std::cout << "  [!] " << e.what()
                          << " (" << (MAX_INTENTOS - intentos) << " intento(s) restante(s))\n";
            } catch (const ContrasenaIncorrectaException& e) {
                ++intentos;
                std::cout << "  [!] " << e.what()
                          << " (" << (MAX_INTENTOS - intentos) << " intento(s) restante(s))\n";
            } catch (const AutenticacionException& e) {
                throw; // Cuenta desactivada: no reintentar
            } catch (const EntradaInvalidaException& e) {
                std::cout << "  [!] " << e.what() << "\n";
            }
        }
        throw AutenticacionException("Demasiados intentos fallidos. Acceso bloqueado.");
    }

    void cerrarSesion() {
        usuarioActual = nullptr;
        std::cout << "  Sesion cerrada.\n";
    }

    Usuario* getUsuarioActual() const {
        if (!usuarioActual)
            throw SesionNoIniciadaException();
        return usuarioActual;
    }

    bool haySesion() const { return usuarioActual != nullptr; }

    void listarUsuarios() const {
        std::cout << "\n  USUARIOS DEL SISTEMA\n  " << std::string(50, '-') << "\n";
        for (const auto& u : usuarios)
            std::cout << "  " << *u << "\n";
    }
};
