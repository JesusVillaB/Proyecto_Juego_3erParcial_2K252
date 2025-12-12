#pragma once
#include "Personaje.hpp"
#include <vector>

using std::vector;

class Objeto; // forward

class Enemigo : public Personaje {
private:
    vector<Objeto*> objetos;

public:
    Enemigo();
    ~Enemigo();

    void patrullar();
    void atacarJugador();
    void morir();
};