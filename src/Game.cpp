#include "../include/Game.hpp"
#include <iostream>
#include <cstdlib>

Game::Game(sf::RenderWindow& win) : window(win), fondo(window), spriteJugador(window) {
    // No manual FPS cap; rely on animation timing in SpriteJugador
    std::cout << "Fondo y SpriteJugador creados." << std::endl;
    std::cout << "Entrando al bucle principal." << std::endl;

    // Initialize 7 random enemies from center onward
    float collisionY = fondo.getCollisionY();
    float floorY = fondo.getFloorY();
    for(int i = 0; i < 7; ++i) {
        float x = 400 + (rand() % 401); // 400 to 800
        float y = collisionY + (rand() % static_cast<int>(floorY - collisionY - 64)); // assuming frameHeight ~64
        sf::Vector2f pos(x, y);
        enemigos.emplace_back(window, pos);
    }
}

void Game::run() {
    // Ensure sprite is placed on the floor at start
    float initialFloorY = fondo.getFloorY();
    float initialCollisionY = fondo.getCollisionY();
    spriteJugador.update(0.0f, initialCollisionY, initialFloorY);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();

        // Detectar direcciones de movimiento
        bool isMovingRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
        bool isMovingLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);

        // Actualizar Fondo y SpriteJugador (pass floorY for consistent collision)
        fondo.update(deltaTime, isMovingRight, isMovingLeft);
        float floorY = fondo.getFloorY();
        float collisionY = fondo.getCollisionY();
        spriteJugador.update(deltaTime, collisionY, floorY);
        sf::Vector2f playerPos = spriteJugador.getPosition();
        for(auto& e : enemigos) {
            e.update(deltaTime, playerPos, collisionY, floorY);
        }

        // Limpiar y dibujar
        window.clear();
        // Draw both background and foreground first, then the player sprite on top
        fondo.drawBackground(window);
        fondo.drawForeground(window);
        for(auto& e : enemigos) {
            e.draw(window);
        }
        spriteJugador.draw(window);
        window.display();
    }

    std::cout << "Programa terminado exitosamente." << std::endl;
}

Game::~Game() {
    std::cout << "Destructor Game called" << std::endl;
}