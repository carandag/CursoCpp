#include <iostream>
using namespace std;

int main() {
    int numero;
    bool esPrimo = true;

    cout << "Introduce un numero: ";
    cin >> numero;

    if (numero <= 1) {
        esPrimo = false;
    } else {
        for (int i = 2; i * i <= numero; i++) {
            if (numero % i == 0) {
                esPrimo = false;
                break;
            }
        }
    }

    if (esPrimo) {
        cout << "El numero " << numero << " es primo." << endl;
    } else {
        cout << "El numero " << numero << " NO es primo." << endl;
    }

    return 0;
}