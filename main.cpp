// ============================================================
//  Sistema Integral de Gestion de Tienda
//  LP2 – Proyecto Final
//  Compilar: g++ -std=c++17 -Wall -Wextra -o tienda main.cpp
// ============================================================
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>

#include "include/Excepciones.h"
#include "include/Validador.h"
#include "include/Producto.h"
#include "include/Inventario.h"
#include "include/Venta.h"
#include "include/Finanzas.h"
#include "include/Reportes.h"
#include "include/Usuario.h"
#include "include/BaseDatos.h"

// ============================================================
//  Estado global del sistema
// ============================================================
struct SistemaTienda {
    SistemaLogin     login;
    Inventario       inventario;
    std::vector<Venta> ventas;
    RegistroFinanciero finanzas;
    BaseDatos        bd{"tienda.db"};
};

// ============================================================
//  Utilidades de consola
// ============================================================
void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausar() {
    std::cout << "\n  Presione ENTER para continuar...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void titulo(const std::string& texto) {
    std::string borde(texto.size() + 4, '=');
    std::cout << "\n  " << borde << "\n"
              << "  | " << texto << " |\n"
              << "  " << borde << "\n\n";
}

// ============================================================
//  Modulo 2: Gestion de Productos
// ============================================================
void menuProductos(SistemaTienda& s) {
    bool volver = false;
    while (!volver) {
        titulo("GESTION DE PRODUCTOS");
        std::cout << "  1. Registrar nuevo producto\n"
                  << "  2. Listar todos los productos\n"
                  << "  3. Actualizar precio\n"
                  << "  4. Actualizar stock\n"
                  << "  5. Buscar producto por codigo\n"
                  << "  6. Productos con stock bajo\n"
                  << "  0. Volver\n";

        int op = EntradaSegura::leerOpcionMenu(0, 6);

        try {
            if (op == 1) {
                titulo("REGISTRAR PRODUCTO");
                std::string nombre = EntradaSegura::leerTexto(
                    "  Nombre del producto: ",
                    ValidadorProducto::NOMBRE_MIN,
                    ValidadorProducto::NOMBRE_MAX);

                double precio = EntradaSegura::leerDouble(
                    "  Precio (S/): ",
                    ValidadorProducto::PRECIO_MIN,
                    ValidadorProducto::PRECIO_MAX);

                int stock = EntradaSegura::leerEntero(
                    "  Stock inicial: ",
                    ValidadorProducto::STOCK_MIN,
                    ValidadorProducto::STOCK_MAX);

                s.inventario.registrarProducto(nombre, precio, stock);

                // Guardar en BD
                const Producto& p = s.inventario.obtenerTodos().back();
                s.bd.guardarProducto(p.getCodigo(), p.getNombre(), p.getPrecio(), p.getStock());

                std::cout << "\n  [OK] Producto registrado con codigo "
                          << p.getCodigo() << ".\n";

            } else if (op == 2) {
                titulo("LISTA DE PRODUCTOS");
                s.inventario.listarTodos();

            } else if (op == 3) {
                titulo("ACTUALIZAR PRECIO");
                int codigo = EntradaSegura::leerEntero("  Codigo del producto: ", 1, 999999);
                Producto* p = s.inventario.buscarPorCodigo(codigo);  // lanza si no existe
                std::cout << "  Producto: " << p->getNombre()
                          << " | Precio actual: S/ "
                          << std::fixed << std::setprecision(2) << p->getPrecio() << "\n";

                double nuevoPrecio = EntradaSegura::leerDouble(
                    "  Nuevo precio (S/): ",
                    ValidadorProducto::PRECIO_MIN,
                    ValidadorProducto::PRECIO_MAX);

                s.inventario.actualizarPrecio(codigo, nuevoPrecio);
                s.bd.guardarProducto(p->getCodigo(), p->getNombre(), p->getPrecio(), p->getStock());
                std::cout << "  [OK] Precio actualizado.\n";

            } else if (op == 4) {
                titulo("ACTUALIZAR STOCK");
                int codigo = EntradaSegura::leerEntero("  Codigo del producto: ", 1, 999999);
                Producto* p = s.inventario.buscarPorCodigo(codigo);
                std::cout << "  Producto: " << p->getNombre()
                          << " | Stock actual: " << p->getStock() << "\n";

                int nuevoStock = EntradaSegura::leerEntero(
                    "  Nuevo stock: ",
                    ValidadorProducto::STOCK_MIN,
                    ValidadorProducto::STOCK_MAX);

                s.inventario.actualizarStock(codigo, nuevoStock);
                s.bd.guardarProducto(p->getCodigo(), p->getNombre(), p->getPrecio(), p->getStock());
                std::cout << "  [OK] Stock actualizado.\n";

            } else if (op == 5) {
                titulo("BUSCAR PRODUCTO");
                int codigo = EntradaSegura::leerEntero("  Codigo: ", 1, 999999);
                Producto* p = s.inventario.buscarPorCodigo(codigo);
                Producto::imprimirCabecera();
                p->imprimirFila();

            } else if (op == 6) {
                titulo("PRODUCTOS CON STOCK BAJO");
                int umbral = EntradaSegura::leerEntero("  Umbral de stock bajo: ", 0, 1000);
                s.inventario.listarConStockBajo(umbral);

            } else {
                volver = true;
            }
        } catch (const TiendaException& e) {
            std::cout << "\n  [ERROR - " << e.getModulo() << "] " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cout << "\n  [ERROR INESPERADO] " << e.what() << "\n";
        }

        if (!volver) pausar();
    }
}

// ============================================================
//  Modulo 4: Registro de ventas
// ============================================================
void menuVentas(SistemaTienda& s) {
    bool volver = false;
    while (!volver) {
        titulo("REGISTRO DE VENTAS");
        std::cout << "  1. Nueva venta\n"
                  << "  2. Ver historial de ventas\n"
                  << "  3. Ver detalle de una venta\n"
                  << "  0. Volver\n";

        int op = EntradaSegura::leerOpcionMenu(0, 3);

        try {
            if (op == 1) {
                titulo("NUEVA VENTA");
                if (s.inventario.estaVacio())
                    throw VentaException("No hay productos en el inventario");

                std::string vendedor = s.login.getUsuarioActual()->getNombre();
                Venta venta(vendedor);

                bool agregarMas = true;
                while (agregarMas) {
                    std::cout << "\n  -- Agregar producto --\n";
                    s.inventario.listarTodos();

                    int codigo = EntradaSegura::leerEntero("  Codigo del producto (0 = terminar): ", 0, 999999);
                    if (codigo == 0) break;

                    int cantidad = EntradaSegura::leerEntero("  Cantidad: ", 1, ValidadorProducto::STOCK_MAX);

                    try {
                        venta.agregarDetalle(s.inventario, codigo, cantidad);
                        std::cout << "  [OK] Producto agregado.\n";
                    } catch (const TiendaException& e) {
                        std::cout << "  [!] " << e.what() << "\n";
                    }

                    std::cout << "  Total parcial: S/ "
                              << std::fixed << std::setprecision(2) << venta.calcularTotal() << "\n";

                    std::cout << "  Agregar otro producto? (1=Si / 0=No): ";
                    int resp = EntradaSegura::leerOpcionMenu(0, 1);
                    agregarMas = (resp == 1);
                }

                if (venta.cantidadItems() == 0)
                    throw VentaVaciaException();

                venta.imprimirTicket();
                std::cout << "  Confirmar venta? (1=Si / 0=Cancelar): ";
                int confirmar = EntradaSegura::leerOpcionMenu(0, 1);

                if (confirmar == 1) {
                    venta.confirmar(s.inventario);
                    // Persistir
                    s.bd.guardarVenta(venta.getId(), venta.getVendedor(),
                                      venta.getFecha(), venta.getHora(),
                                      venta.calcularTotal());
                    for (const DetalleVenta& d : venta.getDetalles())
                        s.bd.guardarDetalleVenta(venta.getId(),
                            d.getCodigoProducto(), d.getNombre(),
                            d.getCantidad(), d.getPrecioUnitario());
                    // Actualizar stock en BD
                    for (const Producto& p : s.inventario.obtenerTodos())
                        s.bd.guardarProducto(p.getCodigo(), p.getNombre(),
                                             p.getPrecio(), p.getStock());

                    s.ventas.push_back(std::move(venta));
                    std::cout << "  [OK] Venta registrada exitosamente.\n";
                } else {
                    std::cout << "  Venta cancelada.\n";
                }

            } else if (op == 2) {
                titulo("HISTORIAL DE VENTAS");
                if (s.ventas.empty()) {
                    std::cout << "  (Sin ventas registradas)\n";
                } else {
                    double totalAcumulado = 0;
                    for (const Venta& v : s.ventas) {
                        std::cout << "  " << v << "\n";
                        totalAcumulado += v.calcularTotal();
                    }
                    std::cout << "  " << std::string(60, '-') << "\n";
                    std::cout << "  Total acumulado: S/ "
                              << std::fixed << std::setprecision(2) << totalAcumulado << "\n";
                }

            } else if (op == 3) {
                titulo("DETALLE DE VENTA");
                int id = EntradaSegura::leerEntero("  ID de la venta: ", 1, 999999);
                bool encontrada = false;
                for (const Venta& v : s.ventas) {
                    if (v.getId() == id) {
                        v.imprimirTicket();
                        encontrada = true;
                        break;
                    }
                }
                if (!encontrada) throw VentaException("Venta #" + std::to_string(id) + " no encontrada");

            } else {
                volver = true;
            }
        } catch (const TiendaException& e) {
            std::cout << "\n  [ERROR - " << e.getModulo() << "] " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cout << "\n  [ERROR INESPERADO] " << e.what() << "\n";
        }

        if (!volver) pausar();
    }
}

// ============================================================
//  Modulo 5: Inversiones y gastos
// ============================================================
void menuFinanzas(SistemaTienda& s) {
    bool volver = false;
    while (!volver) {
        titulo("INVERSIONES Y GASTOS");
        std::cout << "  1. Registrar inversion\n"
                  << "  2. Registrar gasto\n"
                  << "  3. Ver todos los movimientos\n"
                  << "  4. Ver balance actual\n"
                  << "  0. Volver\n";

        int op = EntradaSegura::leerOpcionMenu(0, 4);

        try {
            if (op == 1) {
                titulo("REGISTRAR INVERSION");
                double monto    = EntradaSegura::leerDouble("  Monto (S/): ", 0.01, 1000000.0);
                std::string desc = EntradaSegura::leerTexto("  Descripcion: ", 3, 100);
                std::string prov = EntradaSegura::leerTexto("  Proveedor (Enter=ninguno): ", 0, 60);
                int unidades = EntradaSegura::leerEntero("  Unidades (0=no aplica): ", 0, ValidadorProducto::STOCK_MAX);
                std::string cat  = EntradaSegura::leerTexto("  Categoria (Enter=Inversion): ", 0, 40);

                s.finanzas.agregarInversion(monto, desc, prov, unidades,
                                            cat.empty() ? "Inversion" : cat);
                std::cout << "  [OK] Inversion registrada.\n";

            } else if (op == 2) {
                titulo("REGISTRAR GASTO");
                double monto    = EntradaSegura::leerDouble("  Monto (S/): ", 0.01, 1000000.0);
                std::string desc = EntradaSegura::leerTexto("  Descripcion: ", 3, 100);
                std::cout << "  Tipo: 1=Fijo  2=Variable -> ";
                int tipo = EntradaSegura::leerOpcionMenu(1, 2);
                std::string cat  = EntradaSegura::leerTexto("  Categoria (Enter=Gasto): ", 0, 40);

                s.finanzas.agregarGasto(monto, desc, tipo == 1,
                                        cat.empty() ? "Gasto" : cat);
                std::cout << "  [OK] Gasto registrado.\n";

            } else if (op == 3) {
                titulo("MOVIMIENTOS FINANCIEROS");
                s.finanzas.listarTodos();

            } else if (op == 4) {
                titulo("BALANCE ACTUAL");
                std::cout << "  Total inversiones : S/ "
                          << std::fixed << std::setprecision(2)
                          << s.finanzas.calcularTotalInversiones() << "\n"
                          << "  Total gastos      : S/ "
                          << s.finanzas.calcularTotalGastos() << "\n"
                          << "  " << std::string(36, '-') << "\n"
                          << "  Balance neto      : S/ "
                          << s.finanzas.calcularBalance() << "\n";

            } else {
                volver = true;
            }
        } catch (const TiendaException& e) {
            std::cout << "\n  [ERROR - " << e.getModulo() << "] " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cout << "\n  [ERROR INESPERADO] " << e.what() << "\n";
        }

        if (!volver) pausar();
    }
}

// ============================================================
//  Modulos 6 y 7: Reportes
// ============================================================
void menuReportes(SistemaTienda& s) {
    // Verificar permisos
    if (!s.login.getUsuarioActual()->puedeVerReportes())
        throw PermisoInsuficienteException("ver reportes");

    bool volver = false;
    while (!volver) {
        titulo("REPORTES");
        std::cout << "  1. Reporte diario (hoy)\n"
                  << "  2. Reporte diario (fecha especifica)\n"
                  << "  3. Reporte mensual (mes actual)\n"
                  << "  4. Reporte mensual (mes especifico)\n"
                  << "  0. Volver\n";

        int op = EntradaSegura::leerOpcionMenu(0, 4);

        try {
            if (op == 1) {
                ReporteDiario rep;
                rep.generar(s.ventas, s.finanzas, s.inventario);

            } else if (op == 2) {
                std::string fecha = EntradaSegura::leerTexto(
                    "  Fecha (YYYY-MM-DD): ", 10, 10);
                ReporteDiario rep(fecha);
                rep.generar(s.ventas, s.finanzas, s.inventario);

            } else if (op == 3) {
                ReporteMensual rep;
                rep.generar(s.ventas, s.finanzas, s.inventario);

            } else if (op == 4) {
                std::string mes = EntradaSegura::leerTexto(
                    "  Mes (YYYY-MM): ", 7, 7);
                ReporteMensual rep(mes);
                rep.generar(s.ventas, s.finanzas, s.inventario);

            } else {
                volver = true;
            }
        } catch (const TiendaException& e) {
            std::cout << "\n  [ERROR - " << e.getModulo() << "] " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cout << "\n  [ERROR INESPERADO] " << e.what() << "\n";
        }

        if (!volver) pausar();
    }
}

// ============================================================
//  Menu principal
// ============================================================
void menuPrincipal(SistemaTienda& s) {
    bool salir = false;
    while (!salir) {
        titulo("SISTEMA DE GESTION DE TIENDA");
        std::cout << "  Usuario: " << s.login.getUsuarioActual()->getNombre()
                  << " [" << s.login.getUsuarioActual()->getRol() << "]\n\n"
                  << "  1. Gestion de productos\n"
                  << "  2. Control de inventario\n"
                  << "  3. Registro de ventas\n"
                  << "  4. Inversiones y gastos\n"
                  << "  5. Reporte diario\n"
                  << "  6. Reporte mensual\n"
                  << "  7. Cerrar sesion\n"
                  << "  0. Salir del sistema\n";

        int op = EntradaSegura::leerOpcionMenu(0, 7);

        try {
            switch (op) {
                case 1: menuProductos(s); break;
                case 2:
                    titulo("INVENTARIO");
                    s.inventario.listarTodos();
                    std::cout << "  Valor total del inventario: S/ "
                              << std::fixed << std::setprecision(2)
                              << s.inventario.calcularValorTotalInventario() << "\n";
                    pausar();
                    break;
                case 3: menuVentas(s); break;
                case 4: menuFinanzas(s); break;
                case 5: {
                    ReporteDiario rep;
                    rep.generar(s.ventas, s.finanzas, s.inventario);
                    pausar();
                    break;
                }
                case 6: {
                    ReporteMensual rep;
                    rep.generar(s.ventas, s.finanzas, s.inventario);
                    pausar();
                    break;
                }
                case 7:
                    s.login.cerrarSesion();
                    salir = true;
                    break;
                case 0:
                    std::cout << "  Saliendo del sistema. Hasta luego.\n";
                    s.login.cerrarSesion();
                    exit(0);
            }
        } catch (const PermisoInsuficienteException& e) {
            std::cout << "\n  [ACCESO DENEGADO] " << e.what() << "\n";
            pausar();
        } catch (const TiendaException& e) {
            std::cout << "\n  [ERROR - " << e.getModulo() << "] " << e.what() << "\n";
            pausar();
        } catch (const std::exception& e) {
            std::cout << "\n  [ERROR CRITICO] " << e.what() << "\n";
            pausar();
        }
    }
}

// ============================================================
//  Punto de entrada
// ============================================================
int main() {
    // Configuracion de la consola (evita problemas con acentos en Windows)
    std::cout << std::boolalpha;

    SistemaTienda sistema;

    // Inicializar base de datos
    try {
        sistema.bd.abrir();
    } catch (const BaseDatosException& e) {
        std::cerr << "[ADVERTENCIA] Base de datos: " << e.what()
                  << "\nEl sistema funcionara sin persistencia.\n";
        // No es fatal: el sistema puede funcionar en memoria
    }

    // Datos de prueba para facilitar la evaluacion
    try {
        sistema.inventario.registrarProducto("Coca Cola 500ml", 2.50, 100);
        sistema.inventario.registrarProducto("Arroz 1kg", 3.80, 200);
        sistema.inventario.registrarProducto("Aceite 1L", 7.50, 50);
        sistema.inventario.registrarProducto("Azucar 1kg", 2.90, 150);
        sistema.inventario.registrarProducto("Jabon Lavaplatos", 1.20, 3); // bajo stock
    } catch (const TiendaException& e) {
        // Si ya existen (carga desde BD futura), ignorar
    }

    // ── Bucle de login ───────────────────────────────────────
    while (true) {
        titulo("BIENVENIDO AL SISTEMA DE TIENDA");
        std::cout << "  Ingrese sus credenciales\n\n";

        try {
            sistema.login.loginInteractivo();
            std::cout << "\n  Bienvenido, "
                      << sistema.login.getUsuarioActual()->getNombre() << "!\n";
            pausar();
            menuPrincipal(sistema);

        } catch (const AutenticacionException& e) {
            // Demasiados intentos u otro error grave de autenticacion
            std::cout << "\n  [SEGURIDAD] " << e.what() << "\n";
            std::cout << "  El sistema se cerrara por seguridad.\n";
            return 1;
        } catch (const std::exception& e) {
            std::cout << "\n  [ERROR CRITICO] " << e.what() << "\n";
            return 2;
        }
    }
}
