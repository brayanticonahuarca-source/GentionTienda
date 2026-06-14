#pragma once

#include <string>
#include <vector>

#include "Validador.h"
#include "Excepciones.h"

using namespace std;

//==================================================
// CLASE BASE USUARIO
//==================================================

class Usuario
{
protected:

    inline static int siguienteId = 1;

    int id;
    string nombreUsuario;
    string clave;
    string nombreCompleto;
    bool activo;

    void validarNombreUsuario(const string& usuario) const
    {
        string limpio = ValidadorProducto::trim(usuario);

        if (limpio.empty())
        {
            throw ExcepcionValidacion(
                "El nombre de usuario no puede estar vacio."
            );
        }

        if (limpio.size() < 4)
        {
            throw ExcepcionValidacion(
                "El nombre de usuario debe tener al menos 4 caracteres."
            );
        }

        if (limpio.size() > 20)
        {
            throw ExcepcionValidacion(
                "El nombre de usuario no puede superar 20 caracteres."
            );
        }
    }

    void validarClave(const string& claveUsuario) const
    {
        if (claveUsuario.empty())
        {
            throw ExcepcionValidacion(
                "La clave no puede estar vacia."
            );
        }

        if (claveUsuario.size() < 4)
        {
            throw ExcepcionValidacion(
                "La clave debe tener al menos 4 caracteres."
            );
        }

        if (claveUsuario.size() > 30)
        {
            throw ExcepcionValidacion(
                "La clave no puede superar 30 caracteres."
            );
        }
    }

public:

    Usuario(
        const string& usuario,
        const string& claveUsuario,
        const string& nombre
    )
    {
        string usuarioLimpio = ValidadorProducto::trim(usuario);
        string nombreLimpio = ValidadorProducto::normalizarNombre(nombre);

        validarNombreUsuario(usuarioLimpio);
        validarClave(claveUsuario);
        ValidadorProducto::validarNombre(nombreLimpio);

        id = siguienteId++;
        nombreUsuario = usuarioLimpio;
        clave = claveUsuario;
        nombreCompleto = nombreLimpio;
        activo = true;
    }

    virtual string obtenerRol() const = 0;

    virtual bool puedeGestionarUsuarios() const = 0;

    int obtenerId() const
    {
        return id;
    }

    string obtenerNombreUsuario() const
    {
        return nombreUsuario;
    }

    string obtenerNombreCompleto() const
    {
        return nombreCompleto;
    }

    bool estaActivo() const
    {
        return activo;
    }

    void activar()
    {
        activo = true;
    }

    void desactivar()
    {
        activo = false;
    }

    bool validarAcceso(const string& usuario, const string& claveUsuario) const
    {
        return activo &&
               nombreUsuario == usuario &&
               clave == claveUsuario;
    }

    virtual ~Usuario()
    {
    }
};

//==================================================
// ADMINISTRADOR
//==================================================

class Administrador : public Usuario
{
public:

    Administrador(
        const string& usuario,
        const string& claveUsuario,
        const string& nombre
    )
        : Usuario(usuario, claveUsuario, nombre)
    {
    }

    string obtenerRol() const override
    {
        return "ADMINISTRADOR";
    }

    bool puedeGestionarUsuarios() const override
    {
        return true;
    }
};

//==================================================
// EMPLEADO
//==================================================

class Empleado : public Usuario
{
public:

    Empleado(
        const string& usuario,
        const string& claveUsuario,
        const string& nombre
    )
        : Usuario(usuario, claveUsuario, nombre)
    {
    }

    string obtenerRol() const override
    {
        return "EMPLEADO";
    }

    bool puedeGestionarUsuarios() const override
    {
        return false;
    }
};

//==================================================
// GESTOR DE USUARIOS
//==================================================

class GestorUsuarios
{
private:

    vector<Administrador> administradores;
    vector<Empleado> empleados;

    bool existeUsuario(const string& usuario) const
    {
        for (const Administrador& admin : administradores)
        {
            if (admin.obtenerNombreUsuario() == usuario)
            {
                return true;
            }
        }

        for (const Empleado& empleado : empleados)
        {
            if (empleado.obtenerNombreUsuario() == usuario)
            {
                return true;
            }
        }

        return false;
    }

public:

    void registrarAdministrador(
        const string& usuario,
        const string& clave,
        const string& nombre
    )
    {
        string usuarioLimpio = ValidadorProducto::trim(usuario);

        if (existeUsuario(usuarioLimpio))
        {
            throw ExcepcionValidacion(
                "Ya existe un usuario con ese nombre de usuario."
            );
        }

        Administrador nuevoAdministrador(usuarioLimpio, clave, nombre);
        administradores.push_back(nuevoAdministrador);
    }

    void registrarEmpleado(
        const string& usuario,
        const string& clave,
        const string& nombre
    )
    {
        string usuarioLimpio = ValidadorProducto::trim(usuario);

        if (existeUsuario(usuarioLimpio))
        {
            throw ExcepcionValidacion(
                "Ya existe un usuario con ese nombre de usuario."
            );
        }

        Empleado nuevoEmpleado(usuarioLimpio, clave, nombre);
        empleados.push_back(nuevoEmpleado);
    }

    Usuario* iniciarSesion(
        const string& usuario,
        const string& clave
    )
    {
        for (Administrador& admin : administradores)
        {
            if (admin.validarAcceso(usuario, clave))
            {
                return &admin;
            }
        }

        for (Empleado& empleado : empleados)
        {
            if (empleado.validarAcceso(usuario, clave))
            {
                return &empleado;
            }
        }

        throw ExcepcionValidacion(
            "Usuario o clave incorrectos."
        );
    }

    int cantidadUsuarios() const
    {
        return static_cast<int>(
            administradores.size() + empleados.size()
        );
    }

    vector<Administrador> obtenerAdministradores() const
    {
        return administradores;
    }

    vector<Empleado> obtenerEmpleados() const
    {
        return empleados;
    }
};
