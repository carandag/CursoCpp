#include <iostream>
#include <vector>
#include <string>
#include <memory>

////////////////INTERFAZ (Clase abstracta pura)//////////////////////
class Dibujable {
public:
    virtual void dibujar() const = 0; // Método virtual puro
    virtual ~Dibujable() = default;   // Destructor virtual vital para herencia
};

//////////////// CLASE BASE /////////////////////////////////////////
class Figura : public Dibujable {
protected:
    std::string color;

public:
    Figura(std::string col) : color(col) {}
    virtual ~Figura() = default;

    // Se sobreescriben con cada uno que meta
    virtual double calcularArea() const = 0; 
    virtual std::string getNombre() const = 0;

    // Implementación de la interfaz
    void dibujar() const override {
        std::cout << "Mostrando " << getNombre() 
                  << " de color " << color 
                  << " con un area de: " << calcularArea() << " unidades." << std::endl;
    }
};

///////////////////////HERENCIA/////////////////////////////////////////////////
class Circulo : public Figura {
private:
    double radio;

public:
    Circulo(double r, std::string col) : Figura(col), radio(r) {}

    double calcularArea() const override {
        return 3.14159 * radio * radio; // PI * r^2
    }

    std::string getNombre() const override {
        return "Circulo";
    }
};

class Cuadrado : public Figura {
private:
    double lado;

public:
    Cuadrado(double l, std::string col) : Figura(col), lado(l) {}

    double calcularArea() const override {
        return lado * lado; // l^2
    }

    std::string getNombre() const override {
        return "Cuadrado";
    }
};

/////////////////////////////////CANVAS/////////////////////////////////////
class Canvas {
private:
    // Guardamos punteros inteligentes (unique_ptr) a la clase base Figura
    std::vector<std::unique_ptr<Figura>> figuras;

public:
    // Añadir cualquier figura que herede de Figura
    void añadirFigura(std::unique_ptr<Figura> fig) {
        figuras.push_back(std::move(fig));
        std::cout << "-> ¡Figura añadida al Canvas con exito!" << std::endl;
    }

    // Imprimir todo lo que hay en el canvas
    void imprimirCanvas() const {
        std::cout << "\n--- CONTENIDO DEL CANVAS ---" << std::endl;
        if (figuras.empty()) {
            std::cout << "El canvas esta vacio." << std::endl;
            return;
        }
        
        for (const auto& fig : figuras) {
            fig->dibujar(); // Llama al método polimórfico
        }
        std::cout << "----------------------------\n" << std::endl;
    }
};

////////////////////////////////MAIN//////////////////////////////////////////////
int main() {
    Canvas miCanvas;

    // Creamos figuras usando unique_ptr para gestionar la memoria automáticamente
    auto circuloAzul = std::make_unique<Circulo>(5.0, "Azul");
    auto cuadradoNaranja = std::make_unique<Cuadrado>(4.0, "Naranja");
    auto circuloNaranja = std::make_unique<Circulo>(2.5, "Naranja");

    // Añadimos las figuras al canvas
    miCanvas.añadirFigura(std::move(circuloAzul));
    miCanvas.añadirFigura(std::move(cuadradoNaranja));
    miCanvas.añadirFigura(std::move(circuloNaranja));

    // Imprimimos el resultado en el "Canvas"
    miCanvas.imprimirCanvas();

    return 0;
}