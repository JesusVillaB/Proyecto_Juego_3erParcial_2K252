#include "../include/Game.hpp"
#include <iostream>

Game::Game() : window(sf::VideoMode(800, 600), "Sprite Jugador"), fondo(window), spriteJugador(window) {
    // Logged from main; avoid duplicate
    std::cout << "Ventana creada exitosamente." << std::endl;
    // No manual FPS cap; rely on animation timing in SpriteJugador
    std::cout << "Fondo y SpriteJugador creados." << std::endl;
    std::cout << "Entrando al bucle principal." << std::endl;
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

        // Limpiar y dibujar
        window.clear();
        // Draw both background and foreground first, then the player sprite on top
        fondo.drawBackground(window);
        fondo.drawForeground(window);
        spriteJugador.draw(window);
        window.display();
    }

    std::cout << "Programa terminado exitosamente." << std::endl;
}

Game::~Game() {
    std::cout << "Destructor Game called" << std::endl;
}