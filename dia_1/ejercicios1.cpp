#include <iostream>
using namespace std;

int main() {
    int numero;

    cout << "Introduce un numero: ";
    cin >> numero;

    if (numero % 2 == 0) {
        cout << "El numero " << numero << " es divisible entre 2 y es exacto." << endl;
    } else {
        cout << "El numero " << numero << " NO es divisible entre 2." << endl;
    }

    return 0;
}