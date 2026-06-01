#include <iostream>
using namespace std;

int main() {
    int numeroInicial, numero;

    cout << "Introduce el numero inicial: ";
    cin >> numeroInicial;

    do {
        cout << "Introduce otro numero: ";
        cin >> numero;

        if (numero != numeroInicial) {
            if (numero > numeroInicial) {
                cout << "El numero es MAYOR que el inicial." << endl;
            } else {
                cout << "El numero es MENOR que el inicial." << endl;
            }
        }

    } while (numero != numeroInicial);

    cout << "Has introducido el mismo numero que el inicial. Fin del programa." << endl;

    return 0;
}