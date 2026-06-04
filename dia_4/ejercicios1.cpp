#include "ejercicios1.h"

// ============================================================
//  CIRCULO — implementación
// ============================================================
Circulo::Circulo(double radio, std::string color, int x, int y)
    : radio_(radio), color_(std::move(color)), x_(x), y_(y) {}

    // Lista de inicialización: los miembros se construyen directamente con los valores pasados, sin asignación posterior. std::move(color) evita copiar el string: 
    // transfiere el contenido al atributo color_ y deja el parámetro vacío. Como color es una copia local (pasada por valor), moverla es seguro y más eficiente.

void Circulo::dibujar() const {
    std::cout << "[Circulo] color=" << color_
              << "  radio=" << radio_
              << "  area="  << calcularArea()
              << "  pos=("  << x_ << "," << y_ << ")\n";
}

// Llama a calcularArea() en lugar de duplicar la fórmula. Si la fórmula cambia, solo hay un sitio donde tocarla.
// Implementaciones de una línea: directas, sin lógica adicional. Rectangulo sigue el mismo patrón
double      Circulo::calcularArea() const { return 3.14159 * radio_ * radio_; }
std::string Circulo::getNombre()    const { return "Circulo"; }
int         Circulo::getX()         const { return x_; }
int         Circulo::getY()         const { return y_; }
void        Circulo::mover(int nx, int ny) { x_ = nx; y_ = ny; }


// ============================================================
//  RECTANGULO — implementación
// ============================================================
Rectangulo::Rectangulo(double ancho, double alto, std::string color, int x, int y)
    : ancho_(ancho), alto_(alto), color_(std::move(color)), x_(x), y_(y) {}

void Rectangulo::dibujar() const {
    std::cout << "[Rectangulo] color=" << color_
              << "  " << ancho_ << "x" << alto_
              << "  area="  << calcularArea()
              << "  pos=("  << x_ << "," << y_ << ")\n";
}
double      Rectangulo::calcularArea() const { return ancho_ * alto_; }
std::string Rectangulo::getNombre()    const { return "Rectangulo"; }
int         Rectangulo::getX()         const { return x_; }
int         Rectangulo::getY()         const { return y_; }
void        Rectangulo::mover(int nx, int ny) { x_ = nx; y_ = ny; }


// ============================================================
//  FABRICA DE FIGURAS — implementación
// ============================================================
void FabricaFiguras::registrar(const std::string& nombre, Constructor c) {
    registro()[nombre] = std::move(c);
}
// Llama a registro() para obtener el mapa y añade la entrada. std::move(c) mueve el lambda en lugar de copiarlo (los lambdas con capturas pueden ser costosos de copiar).

std::unique_ptr<Figura> FabricaFiguras::crear(const std::string& linea) {
    std::istringstream in(linea);
    std::string tipo;
    in >> tipo;
    // Convierte la línea de texto en un stream del que podemos leer token a token. Lee el primer token: el nombre del tipo ("circulo", "rectangulo"...).

    auto it = registro().find(tipo);
    if (it == registro().end())
        throw std::runtime_error("Figura desconocida: " + tipo);

    return it->second(in);
}

// Busca el nombre en el mapa. Si no existe, lanza excepción. Si existe, it->second es el lambda registrado, y lo llama con el stream para que lea el resto 
// de los parámetros y construya la figura. El if/else por tipos ha desaparecido completamente.

std::map<std::string, FabricaFiguras::Constructor>& FabricaFiguras::registro() {
    // Singleton de Meyers escondido: vive mientras dure el programa
    static std::map<std::string, Constructor> m;
    return m;
}
// Singleton de Meyers aplicado al mapa. La variable m se crea la primera vez que se llama a registro() y vive hasta el final del programa. 
// Devolver por referencia permite modificar el mapa desde registrar() y leerlo desde crear().

// ============================================================
//  CANVAS (Singleton) — implementación
// ============================================================
Canvas& Canvas::instancia() {
    static Canvas unica;   // thread-safe desde C++11
    return unica;
}
// El Singleton de Meyers: unica se inicializa exactamente una vez, la primera vez que se llama. El estándar C++11 garantiza que esta inicialización es thread-safe. 
// Devolvemos referencia para que el llamador no pueda liberar el objeto.

void Canvas::anadir(std::unique_ptr<Figura> fig) {
    figuras_.push_back(std::move(fig));
}
// push_back con move transfiere el ownership al vector. Después de esta línea, fig está vacío y el vector es el dueño.

void Canvas::imprimir() const {
    std::cout << "\n======= CANVAS =======\n";
    if (figuras_.empty()) {
        std::cout << "  (vacío)\n";
    } else {
        for (const auto& f : figuras_)
            f->dibujar();
    }
    std::cout << "======================\n\n";
}

std::unique_ptr<Figura> Canvas::extraer(const Figura* ptr) {
    for (auto it = figuras_.begin(); it != figuras_.end(); ++it) {
        if (it->get() == ptr) {
            auto fig = std::move(*it);
            figuras_.erase(it);
            return fig;
        }
    }
    return nullptr;
}
// Busca la figura cuyo puntero crudo coincide con ptr. it->get() obtiene el puntero crudo del unique_ptr. Al encontrarla, mueve el unique_ptr fuera del vector 
// (dejando esa posición en estado "movido"), borra esa posición del vector, y devuelve la figura. Si no la encuentra, devuelve nullptr.

std::vector<std::unique_ptr<Figura>> Canvas::vaciar() {
    std::vector<std::unique_ptr<Figura>> backup;
    backup.swap(figuras_);
    return backup;
}
// swap intercambia el contenido de dos vectores en O(1): backup se queda con todas las figuras y figuras_ queda vacío. Luego se devuelve backup por valor; 
// el compilador aplica RVO (Return Value Optimization) y evita la copia.

void Canvas::restaurar(std::vector<std::unique_ptr<Figura>> backup) {
    figuras_ = std::move(backup);
}
// Mueve el vector de backup al atributo interno, descartando cualquier cosa que hubiera. El move evita copiar todos los unique_ptr.

Figura* Canvas::buscar(const Figura* ptr) const {
    for (const auto& f : figuras_)
        if (f.get() == ptr) return f.get();
    return nullptr;
}
// Recorre el vector buscando el puntero. Devuelve el puntero crudo (solo para observación, sin transferir ownership) o nullptr. 
// El main usa esto para validar que una figura sigue en el canvas antes de intentar moverla.

// ============================================================
//  COMMAND: AnadirFiguraCommand
// ============================================================
AnadirFiguraCommand::AnadirFiguraCommand(std::unique_ptr<Figura> fig)
    : figura_(std::move(fig)) {}
// El constructor toma ownership de la figura moviéndola a figura_. En este momento la figura NO está en el canvas.

void AnadirFiguraCommand::ejecutar() {
    ptr_    = figura_.get();
    Canvas::instancia().anadir(std::move(figura_));
}
// Primero guardamos el puntero crudo en ptr_ (antes de mover, porque después figura_ quedará vacío). Luego transferimos la figura al canvas. 
// Ahora figura_ está vacío y ptr_ nos permite localizar la figura en el canvas si necesitamos deshacerlo

void AnadirFiguraCommand::deshacer() {
    figura_ = Canvas::instancia().extraer(ptr_);
}
// extraer busca la figura por ptr_, la saca del canvas, y nos la devuelve. El unique_ptr vuelve a vivir en figura_. Si se vuelve a llamar ejecutar() (redo), el ciclo se repite

// ============================================================
//  COMMAND: MoverFiguraCommand
// ============================================================
MoverFiguraCommand::MoverFiguraCommand(Figura* fig, int nx, int ny)
    : figura_(fig), nx_(nx), ny_(ny), ox_(0), oy_(0) {}
// ox_ y oy_ se inicializan a 0 pero no importa: se sobreescribirán en ejecutar() antes de usarse.

void MoverFiguraCommand::ejecutar() {
    ox_ = figura_->getX();   // guardamos posición original para el undo
    oy_ = figura_->getY();
    figura_->mover(nx_, ny_);
}
// Primero capturamos la posición actual (el "origen" para el undo). Luego movemos. El orden importa: si lo hiciéramos al revés, getX()/getY() 
// nos darían la posición nueva, no la original

void MoverFiguraCommand::deshacer() {
    figura_->mover(ox_, oy_);
}
// Simplemente vuelve a la posición guardada

// ============================================================
//  GESTOR DE COMANDOS
// ============================================================
void GestorComandos::ejecutar(std::unique_ptr<Command> c) {
    c->ejecutar();
    hechos_.push_back(std::move(c));
    deshechos_.clear();    // acción nueva invalida el historial de redo
}
// Ejecuta primero, luego guarda. deshechos_.clear() destruye todos los comandos deshechos: si haces algo nuevo después de varios undos, ya no puedes rehacer lo anterior.

void GestorComandos::deshacer() {
    if (hechos_.empty()) {
        std::cout << "[Gestor] Nada que deshacer.\n";
        return;
    }
    auto c = std::move(hechos_.back());
    hechos_.pop_back();
    c->deshacer();
    deshechos_.push_back(std::move(c));
}
// Saca el último comando hecho, lo deshace, y lo empuja a la pila de deshechos. El std::move en hechos_.back() extrae el unique_ptr del vector sin copiarlo, 
// dejando esa posición en estado movido antes de que pop_back la elimine

void GestorComandos::rehacer() {
    if (deshechos_.empty()) {
        std::cout << "[Gestor] Nada que rehacer.\n";
        return;
    }
    auto c = std::move(deshechos_.back());
    deshechos_.pop_back();
    c->ejecutar();
    hechos_.push_back(std::move(c));
}
// Simétrico al deshacer: saca de la pila de deshechos, ejecuta de nuevo, y vuelve a la pila de hechos


// ============================================================
//  REGISTRO DE FIGURAS
//  Solo este bloque debe tocarse al añadir una figura nueva.
//  Formato de línea desde la "GUI" (cin):
//    circulo    <radio> <color> <x> <y>
//    rectangulo <ancho> <alto>  <color> <x> <y>
// ============================================================
void registrarFiguras() {
    FabricaFiguras::registrar("circulo", [](std::istringstream& in) {
        double radio; std::string color; int x, y;
        in >> radio >> color >> x >> y;
        return std::make_unique<Circulo>(radio, color, x, y);
    });

    FabricaFiguras::registrar("rectangulo", [](std::istringstream& in) {
        double ancho, alto; std::string color; int x, y;
        in >> ancho >> alto >> color >> x >> y;
        return std::make_unique<Rectangulo>(ancho, alto, color, x, y);
    });

    // Para añadir un nuevo tipo: copiar el bloque de arriba, sin tocar nada más.
}
// Cada lambda recibe el stream ya posicionado después del nombre del tipo (la fábrica ya leyó "circulo" o "rectangulo"). El lambda lee los parámetros que le 
// corresponden y construye la figura. Este es el único sitio que cambia cuando añades un tipo nuevo. La fábrica, el canvas y el gestor no se tocan

// ============================================================
//  MAIN — simula la interfaz gráfica (cin como "GUI por texto")
// ============================================================
int main() {
    registrarFiguras();

    GestorComandos gestor;

    std::cout << "=== Mini-Paint (Factory+Prototype+Singleton+Command) ===\n"
              << "Comandos disponibles:\n"
              << "  add <tipo> <params>   — añadir figura al canvas\n"
              << "  move <indice> <x> <y> — mover figura (índice desde 0)\n"
              << "  dup <indice>          — duplicar figura (Prototype)\n"
              << "  show                  — mostrar canvas\n"
              << "  undo                  — deshacer última acción\n"
              << "  redo                  — rehacer\n"
              << "  quit                  — salir\n\n"
              << "Ejemplos:\n"
              << "  add circulo 5.0 Azul 10 20\n"
              << "  add rectangulo 4.0 3.0 Naranja 0 0\n"
              << "  move 0 50 50\n"
              << "  dup 0\n\n";

    // Punteros crudos de solo observación para MoverFiguraCommand.
    // El Canvas es el único propietario (unique_ptr).
    std::vector<Figura*> indice;   // índice de acceso rápido por posición
    // indice es un vector de punteros de observación que nos permite acceder a las figuras por posición numérica desde la línea de comandos. 
    // El canvas sigue siendo el único dueño; aquí solo guardamos referencias para el move y el dup.

    std::string linea;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, linea)) break;
        if (linea.empty()) continue;

        std::istringstream iss(linea);
        std::string cmd;
        iss >> cmd;

        // ---- ADD ----
        if (cmd == "add") {
            std::string resto;
            std::getline(iss >> std::ws, resto);
            try {
                auto fig = FabricaFiguras::crear(resto);
                indice.push_back(fig.get());
                gestor.ejecutar(std::make_unique<AnadirFiguraCommand>(std::move(fig)));
                std::cout << "Figura añadida (índice " << indice.size() - 1 << ").\n";
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << "\n";
            }
            // Primero creamos la figura, guardamos su puntero crudo en indice, y luego la movemos dentro del comando. El orden importa: fig.get() debe llamarse 
            // antes de std::move(fig), porque después fig queda vacío.

        // ---- MOVE ----
        } else if (cmd == "move") {
            size_t idx; int nx, ny;
            iss >> idx >> nx >> ny;
            if (idx >= indice.size() || Canvas::instancia().buscar(indice[idx]) == nullptr) {
                std::cerr << "Error: índice inválido o figura ya eliminada.\n";
                continue;
            }
            gestor.ejecutar(std::make_unique<MoverFiguraCommand>(indice[idx], nx, ny));
            std::cout << "Figura movida a (" << nx << "," << ny << ").\n";
            // La validación doble: primero que el índice esté en rango, luego que la figura siga en el canvas (podría haberse eliminado con un undo). 
            // Solo si pasa ambas, creamos el comando

        // ---- DUP (Prototype) ----
        } else if (cmd == "dup") {
            size_t idx;
            iss >> idx;
            if (idx >= indice.size() || Canvas::instancia().buscar(indice[idx]) == nullptr) {
                std::cerr << "Error: índice inválido.\n";
                continue;
            }
            // clone() → Prototype en acción: el cliente no sabe qué tipo es
            auto clon = indice[idx]->clone();
            indice.push_back(clon.get());
            gestor.ejecutar(std::make_unique<AnadirFiguraCommand>(std::move(clon)));
            std::cout << "Figura duplicada (índice " << indice.size() - 1 << ").\n";
            // clone() es la línea clave. El main no sabe si indice[idx] es un Circulo o un Rectangulo. No le importa. Le pide que se clone y recibe una copia perfecta. 
            // Luego la trata exactamente igual que cualquier figura nueva

        // ---- SHOW ----
        } else if (cmd == "show") {
            Canvas::instancia().imprimir();

        // ---- UNDO ----
        } else if (cmd == "undo") {
            gestor.deshacer();

        // ---- REDO ----
        } else if (cmd == "redo") {
            gestor.rehacer();

        // ---- QUIT ----
        } else if (cmd == "quit") {
            break;

        } else {
            std::cerr << "Comando desconocido: " << cmd << "\n";
        }
    }

    return 0;
}