#pragma once
#include "Personaje.hpp"
#include <vector>

using std::vector;

class Objeto; // forward
class Habilidad; // forward

class Jugador : public Personaje {
private:
    // vector<Objeto*> inventario;
    // vector<Habilidad*> habilidades;

public:
    Jugador() {}
    ~Jugador() {}

    void usarObjeto(Objeto* obj) { /* implement */ }
    void activarHabilidad(Habilidad* hab) { /* implement */ }
    void saltar() { /* implement */ }
    void disparar() { /* implement */ }
};