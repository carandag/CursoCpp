#pragma once          // Evita que el compilador procese este archivo más de una vez si se incluye desde varios .cpp

#include <iostream>   // cout, cerr
#include <vector>     // std::vector
#include <string>     // std::string
#include <memory>     // unique_ptr, make_unique
#include <map>        // std::map (registro de la fábrica)
#include <functional> // std::function (tipo de los constructores registrados)
#include <sstream>    // istringstream (parsear la línea de texto)
#include <stdexcept>  // runtime_error

// ============================================================
//  INTERFAZ BASE: Figura
//  - dibujar()  → Dibujable (interfaz pura del día anterior)
//  - clone()    → Prototype
//  - calcularArea(), getNombre() → contrato de cada figura
// ============================================================
class Figura {
public:
    virtual ~Figura() = default;
    // Destructor virtual. Regla de oro en C++: si una clase tiene métodos virtuales (es decir, se va a usar con polimorfismo), su destructor también debe ser virtual. 
    // Sin esto, hacer delete a través de un puntero base no llamaría al destructor del hijo → memoria perdida y comportamiento indefinido.

    // --- Prototype: cada figura sabe clonarse a sí misma ---
    virtual std::unique_ptr<Figura> clone() const = 0;
    // Prototype. Método virtual puro: cada subclase concreta está obligada a implementarlo. Devuelve unique_ptr<Figura> (no Figura por valor, que causaría slicing, 
    // ni puntero crudo que habría que liberar manualmente). El const indica que clonar no modifica la figura original.

    // --- Contrato de la figura ---
    virtual void        dibujar()        const = 0;
    virtual double      calcularArea()   const = 0;
    virtual std::string getNombre()      const = 0;
    // Contrato que toda figura debe cumplir. Son = 0 (virtuales puros), así que Figura es una clase abstracta: no se puede instanciar directamente, 
    // solo a través de sus hijos. Todos son const porque leer datos no debería modificar el objeto.

    // --- Posición en el canvas ---
    virtual int  getX() const = 0;
    virtual int  getY() const = 0;
    virtual void mover(int nx, int ny) = 0;
    // La posición en el canvas forma parte del contrato de cualquier figura. mover no es const porque sí modifica el estado interno (las coordenadas)
};


// ============================================================
//  CRTP helper: evita repetir clone() en cada subclase
//  Cada figura concreta hereda de Clonable<Ella misma>.
// ============================================================
template<typename Derived>
class Clonable : public Figura {
public:
    std::unique_ptr<Figura> clone() const override {
        return std::make_unique<Derived>(static_cast<const Derived&>(*this));
    }
};
   // CRTP (Curiously Recurring Template Pattern): la idea es que cuando escribes class Circulo : public Clonable<Circulo>, el compilador genera una versión de Clonable 
   // donde Derived = Circulo. Entonces clone() se convierte en:   return std::make_unique<Circulo>(static_cast<const Circulo&>(*this));
   // Es decir, llama al constructor de copia de Circulo con *this. El resultado es una copia exacta del objeto, envuelta en un unique_ptr<Figura>.
   // El static_cast es necesario porque this dentro de Clonable es de tipo Clonable*, no Derived*. El cast le dice al compilador "confía en mí, en tiempo de ejecución esto 
   // es realmente un Derived". Es seguro porque la única forma de tener un Clonable<Circulo> es siendo un Circulo.
   // Con CRTP, esa implementación está escrita una sola vez para todas las figuras.

   // class Circulo : public Clonable<Circulo> {
   // public:
   //       Circulo(double radio, std::string color, int x = 0, int y = 0);
   // Hereda de Clonable<Circulo>, no directamente de Figura. Eso le da clone() gratis. Los parámetros x = 0, y = 0 son valores por defecto: si no se especifica posición, 
   // la figura empieza en el origen.

// ============================================================
//  FIGURAS CONCRETAS
//  Cada una implementa solo lo que le es propio.
//  Para añadir una nueva figura: crear la clase aquí y
//  registrarla en FabricaFiguras (sin tocar nada más).
// ============================================================

class Circulo : public Clonable<Circulo> {
public:
    Circulo(double radio, std::string color, int x = 0, int y = 0);

    void        dibujar()      const override;
    double      calcularArea() const override;
    std::string getNombre()    const override;
    int         getX()         const override;
    int         getY()         const override;
    void        mover(int nx, int ny) override;
    // El override es importante: le dice al compilador "esto debe existir en la clase base". Si te equivocas en el nombre o la firma, el compilador te avisa en lugar de 
    // crear silenciosamente un método nuevo que nunca se llama.

private:
    double      radio_;
    std::string color_;
    int         x_, y_;
};

// Los datos son private. Nadie fuera de la clase los toca directamente. El guion bajo final (radio_) es una convención para distinguir atributos de parámetros de función.
// Rectangulo sigue exactamente la misma estructura, pero con ancho_ y alto_ en lugar de radio_.


class Rectangulo : public Clonable<Rectangulo> {
public:
    Rectangulo(double ancho, double alto, std::string color, int x = 0, int y = 0);

    void        dibujar()      const override;
    double      calcularArea() const override;
    std::string getNombre()    const override;
    int         getX()         const override;
    int         getY()         const override;
    void        mover(int nx, int ny) override;

private:
    double      ancho_, alto_;
    std::string color_;
    int         x_, y_;
};


// ============================================================
//  FACTORY METHOD con registro dinámico (Good-b del apunte)
//
//  Formato de línea:
//    circulo    <radio> <color> <x> <y>
//    rectangulo <ancho> <alto>  <color> <x> <y>
//
//  Para añadir una figura nueva:
//    1. Definir la clase (hereda de Clonable<T>).
//    2. Llamar FabricaFiguras::registrar() con su nombre y lambda.
//    Sin tocar FabricaFiguras en sí → OCP cumplido.
// ============================================================
class FabricaFiguras {
public:
    using Constructor = std::function<std::unique_ptr<Figura>(std::istringstream&)>;

    // using crea un alias de tipo. Constructor es el tipo de cualquier función (o lambda) que reciba un istringstream& y devuelva un unique_ptr<Figura>. 
    // Sin el alias, tendríamos que escribir ese tipo completo en todas partes.
    // std::function es un "envoltorio" que puede guardar cualquier callable: lambdas, funciones libres, functores. Necesitamos esto porque los lambdas que registra 
    // cada figura tienen tipos distintos internamente.

    static void registrar(const std::string& nombre, Constructor c);
    static std::unique_ptr<Figura> crear(const std::string& linea);

    // Dos métodos estáticos: no hace falta crear ninguna instancia de FabricaFiguras para usarlos. registrar añade un tipo al mapa. crear parsea una línea y 
    // llama al constructor registrado correspondiente.

private:
    static std::map<std::string, Constructor>& registro();
};

// El mapa es privado y se accede solo a través de este método estático. Por qué un método y no directamente static std::map<...> registro_? Porque así aplicamos 
// el Singleton de Meyers al mapa: se inicializa en la primera llamada, sin el static initialization order fiasco que tendría una variable estática de clase normal.

// ============================================================
//  SINGLETON: Canvas
//  Un único lienzo activo en toda la aplicación.
//  Punto de acceso global: Canvas::instancia()
// ============================================================
class Canvas {
public:
    static Canvas& instancia();
    // El único punto de acceso. Devuelve referencia (no puntero) para dejar claro que no puede ser nullptr y que no hay que liberar nada.

    // --- API pública ---
    void anadir(std::unique_ptr<Figura> fig);
    void imprimir() const;
    // API básica. anadir toma ownership de la figura (por eso unique_ptr por valor, no por referencia). imprimir es const porque solo lee.

    // Métodos auxiliares necesarios para los Commands (undo/redo)
    std::unique_ptr<Figura>                     extraer(const Figura* ptr);
    std::vector<std::unique_ptr<Figura>>        vaciar();
    void restaurar(std::vector<std::unique_ptr<Figura>> backup);

    // Acceso al índice de una figura por puntero (para mover)
    Figura* buscar(const Figura* ptr) const;
    // Estos cuatro métodos son los que necesitan los Commands para implementar undo/redo. No forman parte de la "API de usuario" del canvas, sino de la 
    // "API de infraestructura" para el sistema de comandos. extraer saca una figura específica. vaciar devuelve todas y deja el canvas vacío. restaurar repone un backup. 
    // buscar comprueba si una figura sigue en el canvas (para validar antes de mover).

    // No se puede copiar ni mover: es un Singleton
    Canvas(const Canvas&)            = delete;
    Canvas& operator=(const Canvas&) = delete;
    Canvas(Canvas&&)                 = delete;
    Canvas& operator=(Canvas&&)      = delete;

    // Las cuatro defensas del Singleton. Sin estas líneas, el compilador generaría automáticamente constructor de copia, asignación de copia, constructor de movimiento 
    // y asignación de movimiento. Alguien podría hacer Canvas c = Canvas::instancia() y romper la unicidad. Con = delete eso da error de compilación.

private:
    Canvas()  = default;
    ~Canvas() = default;

    std::vector<std::unique_ptr<Figura>> figuras_;
};
// Constructor y destructor privados: nadie puede crear ni destruir un Canvas desde fuera. Solo instancia() puede hacerlo (y lo hace implícitamente la primera 
// vez que se llama, a través de la variable estática local).

// ============================================================
//  COMMAND — interfaz base
// ============================================================
class Command {
public:
    virtual ~Command() = default;
    virtual void ejecutar()  = 0;
    virtual void deshacer()  = 0;
};
// Interfaz mínima: dos métodos virtuales puros. El destructor virtual es obligatorio por las mismas razones que en Figura. El GestorComandos trabaja con 
// unique_ptr<Command> y nunca necesita saber qué tipo concreto hay dentro: solo llama a ejecutar() o deshacer().

// ============================================================
//  COMMAND — AnadirFiguraCommand
//  Añade una figura al canvas. Deshacer la extrae.
// ============================================================
class AnadirFiguraCommand : public Command {
public:
    explicit AnadirFiguraCommand(std::unique_ptr<Figura> fig);
    void ejecutar() override;
    void deshacer() override;

private:
    std::unique_ptr<Figura> figura_;   // propiedad mientras no se ejecutó / al deshacer
    const Figura*           ptr_ = nullptr; // referencia débil para encontrarla en el canvas
};
// explicit en el constructor evita conversiones implícitas accidentales.
// El comando tiene dos punteros a la misma figura en momentos distintos de su vida:
// figura_ tiene el ownership. Está ocupado mientras la figura no está en el canvas (antes de ejecutar, o después de deshacer).
// ptr_ es un puntero de observación (sin ownership). Lo usamos para localizar la figura dentro del vector del canvas cuando necesitamos extraerla al deshacer.
// La figura viaja: figura_ → canvas (al ejecutar) → figura_ de vuelta (al deshacer).

// ============================================================
//  COMMAND — MoverFiguraCommand
//  Mueve una figura a (nx, ny). Deshacer vuelve a (ox, oy).
//
//  El comando guarda el estado anterior para revertirlo:
//  esto es la esencia de "un verbo convertido en sustantivo".
// ============================================================
class MoverFiguraCommand : public Command {
public:
    MoverFiguraCommand(Figura* fig, int nx, int ny);
    void ejecutar() override;
    void deshacer() override;

private:
    Figura* figura_;
    int nx_, ny_;   // destino
    int ox_, oy_;   // origen (guardado al ejecutar para poder deshacer)
};
// figura_ es un puntero de observación: el canvas sigue siendo el dueño. El comando solo necesita poder llamar mover() sobre ella.
// nx_, ny_ son el destino. ox_, oy_ se rellenan en ejecutar() con la posición actual antes de mover: son la memoria que permite deshacer. 
// En el constructor aún no los conocemos (la figura podría haberse movido antes), por eso se guardan en el momento de ejecutar.

// ============================================================
//  GESTOR DE COMANDOS
//  Mantiene la pila de hechos y deshechos.
//  La GUI solo habla con este gestor, nunca con el Canvas
//  directamente.
// ============================================================
class GestorComandos {
public:
    void ejecutar(std::unique_ptr<Command> c);
    void deshacer();
    void rehacer();

private:
    std::vector<std::unique_ptr<Command>> hechos_;
    std::vector<std::unique_ptr<Command>> deshechos_;
};
// Dos pilas. hechos_ es la historia hacia atrás (undo). deshechos_ es la historia hacia adelante (redo). Cuando se ejecuta una acción nueva, 
// deshechos_ se vacía: no tiene sentido "rehacer" algo si has hecho algo distinto después.
// El gestor no sabe nada de figuras, canvas ni tipos concretos de comandos. Solo habla el idioma de Command. Esto es DIP en acción

// ============================================================
//  REGISTRO DE FIGURAS
//  Llamar una vez al inicio (desde main o init()) para
//  registrar todos los tipos conocidos en la fábrica.
//  Añadir una figura nueva = añadir una llamada aquí.
// ============================================================
void registrarFiguras();