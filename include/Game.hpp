#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "../include/SpriteJugador.hpp"
#include "../include/Fondo.hpp"

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    sf::RenderWindow window;
    Fondo fondo;
    SpriteJugador spriteJugador;
    sf::Clock clock;
};

#endif // GAME_HPP