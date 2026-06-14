#include <iostream>
#include "include/Usuario.h"
#include "include/Excepciones.h"
#include "include/Validador.h"
#include "include/Producto.h"
#include "include/Inventario.h"
#include "include/Venta.h"
#include "include/Finanzas.h"
#include "include/Reportes.h"
#include "include/BaseDatos.h"
using namespace std;

int main()
{
    try
    {
        Inventario inventario;

        Producto p1 = inventario.agregarProducto("Coca Cola", 3.50, 20);
        Producto p2 = inventario.agregarProducto("Leche Gloria", 4.80, 15);

        Venta venta("Brayan");

        venta.agregarDetalle(inventario, p1.obtenerCodigo(), 2);
        venta.agregarDetalle(inventario, p2.obtenerCodigo(), 1);

        venta.confirmar(inventario);

        cout << "Venta registrada correctamente." << endl;
        cout << "Total: S/ " << venta.calcularTotal() << endl;

        cout << "Stock Coca Cola: "
             << inventario.buscarPorCodigo(p1.obtenerCodigo())->obtenerStock()
             << endl;

        //==========================================
        // REGISTRO DE VENTAS
        //==========================================

        RegistroVentas registro;

        registro.registrarVenta(venta);

        cout << "Ventas registradas: "
             << registro.obtenerCantidadVentas()
             << endl;

        cout << "Total vendido: S/ "
             << registro.calcularTotalVentas()
             << endl;

        //==========================================
        // FINANZAS
        //==========================================

        RegistroFinanciero finanzas;

        finanzas.registrarGasto("Pago de luz", 80.00);
        finanzas.registrarInversion("Compra de mercaderia", 500.00);

        cout << "Gastos: S/ "
             << finanzas.calcularTotalGastos()
             << endl;

        cout << "Inversiones: S/ "
             << finanzas.calcularTotalInversiones()
             << endl;

        //==========================================
        // REPORTE DIARIO
        //==========================================

        ReporteDiario reporteDiario(
            registro,
            finanzas,
            venta.obtenerFecha()
        );

        ResumenReporte resumenDiario = reporteDiario.generar();

        cout << "\n===== REPORTE DIARIO =====" << endl;
        cout << "Reporte: " << resumenDiario.titulo << endl;
        cout << "Fecha: " << resumenDiario.periodo << endl;
        cout << "Ventas: " << resumenDiario.cantidadVentas << endl;
        cout << "Total vendido: S/ " << resumenDiario.totalVendido << endl;
        cout << "Gastos: S/ " << resumenDiario.totalGastos << endl;
        cout << "Inversiones: S/ " << resumenDiario.totalInversiones << endl;
        cout << "Ganancia neta: S/ " << resumenDiario.gananciaNeta << endl;

        //==========================================
        // REPORTE MENSUAL
        //==========================================

        string mesActual = venta.obtenerFecha().substr(0, 7);

        ReporteMensual reporteMensual(
            registro,
            finanzas,
            mesActual
        );

        ResumenReporte resumenMensual = reporteMensual.generar();

        cout << "\n===== REPORTE MENSUAL =====" << endl;
        cout << "Reporte: " << resumenMensual.titulo << endl;
        cout << "Mes: " << resumenMensual.periodo << endl;
        cout << "Ventas: " << resumenMensual.cantidadVentas << endl;
        cout << "Total vendido: S/ " << resumenMensual.totalVendido << endl;
        cout << "Gastos: S/ " << resumenMensual.totalGastos << endl;
        cout << "Inversiones: S/ " << resumenMensual.totalInversiones << endl;
        cout << "Ganancia neta: S/ " << resumenMensual.gananciaNeta << endl;

        GestorUsuarios usuarios;

        usuarios.registrarAdministrador("admin", "1234", "Brayan Ticona");
        usuarios.registrarEmpleado("cajero", "1234", "Luis Perez");

        Usuario* usuarioActual = usuarios.iniciarSesion("admin", "1234");

        cout << "\n===== LOGIN =====" << endl;
        cout << "Usuario: " << usuarioActual->obtenerNombreUsuario() << endl;
        cout << "Rol: " << usuarioActual->obtenerRol() << endl;
        cout << "Puede gestionar usuarios: "
            << usuarioActual->puedeGestionarUsuarios()
            << endl;

            BaseDatos baseDatos("datos_tienda.txt");

            baseDatos.conectar();

            baseDatos.guardarLinea("Venta registrada correctamente");
            baseDatos.guardarLinea("Total vendido: S/ " + to_string(venta.calcularTotal()));
            baseDatos.guardarLinea("Ganancia neta: S/ " + to_string(resumenDiario.gananciaNeta));

            baseDatos.desconectar();

            cout << "\nDatos guardados correctamente." << endl;


    }
    catch (const ExcepcionTienda& error)
    {
        cout << "ERROR: " << error.what() << endl;
    }
    catch (const exception& error)
    {
        cout << "ERROR GENERAL: " << error.what() << endl;
    }

    return 0;
}
