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
    enum State { PLAYING, GAME_OVER, WIN };
    sf::RenderWindow& window;
    Fondo fondo;
    SpriteJugador spriteJugador;
    std::vector<Enemigo> enemigos;
    sf::Clock clock;
    State currentState;
    sf::Font font;
    sf::Text gameOverText;
    sf::Text winText;
    sf::Texture spaceshipTexture;
    sf::Sprite spaceship;
    bool spaceshipVisible;
};

#endif // GAME_HPP