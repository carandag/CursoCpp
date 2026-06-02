#include <iostream>
#include <string>

using namespace std;

// Declaración adelantada
class ListaPizza;

class Nodo {
private:
    string ingrediente;
    Nodo* siguiente;

public:
    Nodo(string ing) {
        ingrediente = ing;
        siguiente = nullptr;
    }

    // La clase ListaPizza puede acceder a los atributos privados
    friend class ListaPizza;
};

class ListaPizza {
private:
    Nodo* cabeza;

public:
    ListaPizza() {
        cabeza = nullptr;
    }

    // Insertar ingrediente al final
    void agregarIngrediente(string ingrediente) {
        Nodo* nuevo = new Nodo(ingrediente);

        if (cabeza == nullptr) {
            cabeza = nuevo;
        } else {
            Nodo* aux = cabeza;

            while (aux->siguiente != nullptr) {
                aux = aux->siguiente;
            }

            aux->siguiente = nuevo;
        }
    }

    // Mostrar todos los ingredientes
    void mostrarPizza() {
        if (cabeza == nullptr) {
            cout << "La pizza no tiene ingredientes." << endl;
            return;
        }

        Nodo* aux = cabeza;

        cout << "Ingredientes de la pizza:" << endl;

        while (aux != nullptr) {
            cout << "- " << aux->ingrediente << endl;
            aux = aux->siguiente;
        }
    }

    // Liberar memoria
    ~ListaPizza() {
        Nodo* aux;

        while (cabeza != nullptr) {
            aux = cabeza;
            cabeza = cabeza->siguiente;
            delete aux;
        }
    }
};

int main() {
    ListaPizza pizza;

    pizza.agregarIngrediente("Tomate");
    pizza.agregarIngrediente("Mozzarella");
    pizza.agregarIngrediente("Jamon");
    pizza.agregarIngrediente("Champinones");
    pizza.agregarIngrediente("Aceitunas");

    pizza.mostrarPizza();

    return 0;
}