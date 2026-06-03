#include <iostream>
#include <set>

class MiClase {
private:
    int id;

public:
    // Constructor
    MiClase(int _id) : id(_id) {}

    // Método para obtener el ID (opcional, para el print)
    int getId() const {
        return id;
    }

    // Sobrecarga del operador < (Imprescindible para std::set)
    // El método DEBE ser const y recibir una referencia const
    bool operator<(const MiClase& otra) const {
        return this->id < otra.id;
    }
};

int main() {
    // Creamos el set de objetos MiClase
    std::set<MiClase> miSet;

    // Creamos las instancias con el mismo ID como en tu ejemplo
    MiClase m(1);
    MiClase m1(1);
    MiClase m2(5); // Una extra para probar

    // Intentamos insertar la primera (m)
    auto resultado1 = miSet.insert(m);
    if (resultado1.second) {
        std::cout << "m (ID: " << m.getId() << ") agregada con exito.\n";
    } else {
        std::cout << "m (ID: " << m.getId() << ") no se pudo agregar.\n";
    }

    // Intentamos insertar la segunda (m1) con ID duplicado
    auto resultado2 = miSet.insert(m1);
    if (resultado2.second) {
        std::cout << "m1 (ID: " << m1.getId() << ") agregada con exito.\n";
    } else {
        std::cout << "m1 (ID: " << m1.getId() << ") NO se agrego (ID duplicado).\n";
    }

    // Insertamos la tercera (m2) con ID diferente
    miSet.insert(m2);

    // Verificamos el tamaño final del set
    std::cout << "Tamano final del set: " << miSet.size() << " (Deberia ser 2)\n";

    return 0;
}