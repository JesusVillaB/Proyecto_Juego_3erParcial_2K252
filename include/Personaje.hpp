#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using sf::Vector2f;
using std::string;
using std::vector;

// Forward declarations and minimal definitions for dependencies
class Sonido {
public:
    virtual void reproducir() {}
    void detener() {}
    void pausar() {}
};

class Objeto {
public:
    virtual void usar() {}
    void recoger() {}
    virtual void actualizar() {}
};

class Habilidad {
public:
    void activar() {}
    void desactivar() {}
    void actualizar() {}
};

class Generador;

class EfectoVisual;

class SacudidaCamara {
public:
    void iniciar() {}
    void actualizar() {}
};

class Particula {
public:
    void actualizar() {}
    void renderizar() {}
};

class EfectoEspecial {
public:
    void activar() {}
    void desactivar() {}
    void actualizar() {}
};

class EfectoVisual {
public:
    vector<Particula*> particulas;
    SacudidaCamara* sacudidaCamara;
    void reproducir() {}
    void actualizar() {}
    void renderizar() {}
};

class Generador {
public:
    void generarEnemigo() {}
    void actualizar() {}
};

class Personaje {
private:
    string nombre;
    Vector2f posicion;
    Vector2f velocidad;
    int vida;
    // Sonido* sonidoPaso;
    sf::Sprite sprite;

public:
    Personaje() : vida(100) {} // , sonidoPaso(nullptr)
    virtual ~Personaje() {}

    void setTexture(const sf::Texture& texture) { sprite.setTexture(texture); }
    void setPosition(const sf::Vector2f& pos) { posicion = pos; sprite.setPosition(pos); }
    sf::Vector2f getPosition() const { return posicion; }
    void setTextureRect(const sf::IntRect& rect) { sprite.setTextureRect(rect); }

    void mover() { /* implement */ }
    void recibirDanio(int danio) { vida -= danio; }
    void atacar() { /* implement */ }
    virtual void actualizar() { /* implement */ }
    virtual void renderizar(sf::RenderWindow& window) { window.draw(sprite); }
};