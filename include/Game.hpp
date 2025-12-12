#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "../include/SpriteJugador.hpp"
#include "../include/Fondo.hpp"
#include "../include/Enemigo.hpp"

class Game {
public:
    Game(sf::RenderWindow& window);
    ~Game();
    void run();

private:
    sf::RenderWindow& window;
    Fondo fondo;
    SpriteJugador spriteJugador;
    std::vector<Enemigo> enemigos;
    sf::Clock clock;
};

#endif // GAME_HPP