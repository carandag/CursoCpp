#include <iostream>
#include <vector>
#include <memory>
#include <utility>

class MiClase {
private:
    int* datos;
    size_t tamano;

public:
    // Constructor normal
    MiClase(size_t tam) : tamano(tam) {
        datos = new int[tamano];
        std::cout << "[Constructor] Recursos asignados de tamano " << tamano << "\n";
    }

    // Destructor
    ~MiClase() {
        delete[] datos;
        std::cout << "[Destructor] Recursos liberados\n";
    }

    // 1. Constructor de Copia (Reemplazado/Explícito)
    MiClase(const MiClase& otro) : tamano(otro.tamano) {
        // <-- PONER PUNTO DE INTERRUPCIÓN AQUÍ
        datos = new int[tamano];
        for (size_t i = 0; i < tamano; ++i) {
            datos[i] = otro.datos[i];
        }
        std::cout << "[Copia] Copia profunda realizada\n";
    }

    // 2. Constructor de Movimiento (Reemplazado + noexcept)
    MiClase(MiClase&& otro) noexcept : datos(otro.datos), tamano(otro.tamano) {
        // <-- PONER PUNTO DE INTERRUPCIÓN AQUÍ
        otro.datos = nullptr; // Robamos el recurso y dejamos al original en un estado seguro
        otro.tamano = 0;
        std::cout << "[Movimiento] Recurso transferido (noexcept)\n";
    }

    // Deshabilitamos el operador de asignación para simplificar el ejemplo
    MiClase& operator=(const MiClase&) = delete;
    MiClase& operator=(MiClase&&) = delete;
};

int main() {
    std::cout << "--- PRUEBA 1: Vector y Copia ---\n";
    MiClase objeto1(100);
    
    std::vector<MiClase> listaObjetos;
    // Al hacer push_back de un lvalue (objeto existente), se dispara el constructor de copia
    std::cout << "Insertando en el vector (deberia saltar el breakpoint de copia):\n";
    listaObjetos.push_back(objeto1); 

    std::cout << "\n--- PRUEBA 2: unique_ptr y Movimiento ---\n";
    // Creamos un unique_ptr que gestiona una instancia de MiClase
    std::unique_ptr<MiClase> ptr1 = std::make_unique<MiClase>(500);

    // unique_ptr NO se puede copiar, por lo que estamos obligados a moverlo.
    // Al usar std::move, forzamos la semántica de movimiento.
    std::cout << "Moviendo el unique_ptr (deberia saltar el breakpoint de movimiento):\n";
    std::unique_ptr<MiClase> ptr2 = std::move(ptr1); 

    std::cout << "\n--- Fin de Main (Limpieza automatica) ---\n";
    return 0;
}