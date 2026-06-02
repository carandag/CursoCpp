#include <iostream>
using namespace std;

int main() {
    int numeros[100]; // Array para almacenar hasta 100 números
    int num;
    int contador = 0;

    cout << "Introduce numeros (0 para terminar):" << endl;

    do {
        cin >> num;

        if (num != 0) {
            numeros[contador] = num;
            contador++;
        }
    } while (num != 0);

    cout << "\nNumeros introducidos:" << endl;

    for (int i = 0; i < contador; i++) {
        cout << numeros[i] << " ";
    }

    cout << endl;

    return 0;
}