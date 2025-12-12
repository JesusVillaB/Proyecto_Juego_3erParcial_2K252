#include "../include/Game.hpp"
#include <iostream>
#include <cstdlib>
#include <cmath>

Game::Game(sf::RenderWindow& win) : window(win), fondo(window), spriteJugador(window), currentState(PLAYING), spaceshipVisible(false) {
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

    // Load font and game over text
    if (!font.loadFromFile("assets/Fuentes/Boxy-Bold.ttf")) {
        std::cerr << "Failed to load font for game over" << std::endl;
    }
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(sf::Color::Red);
    sf::FloatRect bounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin(bounds.width / 2, bounds.height / 2);
    gameOverText.setPosition(400, 300);

    winText.setFont(font);
    winText.setString("You win!");
    winText.setCharacterSize(48);
    winText.setFillColor(sf::Color::Green);
    sf::FloatRect bounds2 = winText.getLocalBounds();
    winText.setOrigin(bounds2.width / 2, bounds2.height / 2);
    winText.setPosition(400, 300);

    // Load spaceship texture
    if (!spaceshipTexture.loadFromFile("assets/Objetos/SpaceshipNormal.png")) {
        throw std::runtime_error("Failed to load spaceship texture from 'assets/Objetos/SpaceshipNormal.png'");
    }
    spaceship.setTexture(spaceshipTexture);
}

void Game::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (currentState == PLAYING) {
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

            // Check collisions with projectiles
            for(auto& e : enemigos) {
                for(auto it = e.getProjectiles().begin(); it != e.getProjectiles().end(); ) {
                    sf::Vector2f diff = playerPos - it->position;
                    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                    if (dist < 30.0f) {
                        spriteJugador.takeDamage();
                        it = e.getProjectiles().erase(it);
                    } else {
                        ++it;
                    }
                }
            }

            // Check collisions with player projectiles and enemies
            for(auto it = spriteJugador.getProjectiles().begin(); it != spriteJugador.getProjectiles().end(); ) {
                bool hit = false;
                for(auto& e : enemigos) {
                    if (e.isExploding()) continue; // No collision if exploding
                    sf::Vector2f enemyPos = e.getPosition();
                    sf::Vector2f diff = enemyPos - it->position;
                    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                    if (dist < 40.0f) { // Assume enemy size
                        e.takeDamage();
                        hit = true;
                        break;
                    }
                }
                if (hit) {
                    it = spriteJugador.getProjectiles().erase(it);
                } else {
                    ++it;
                }
            }

            // Remove dead enemies
            for(auto it = enemigos.begin(); it != enemigos.end(); ) {
                if (it->isEnemyDead()) {
                    it = enemigos.erase(it);
                } else {
                    ++it;
                }
            }

            // Check if player is dead
            if (spriteJugador.isDead()) {
                currentState = GAME_OVER;
            }

            // Spaceship logic
            if (fondo.isAtEnd() && enemigos.empty() && !spaceshipVisible) {
                spaceshipVisible = true;
                spaceship.setPosition(window.getSize().x - spaceship.getGlobalBounds().width - 50, window.getSize().y - spaceship.getGlobalBounds().height - 50);
                std::cout << "Spaceship appeared at position: " << spaceship.getPosition().x << ", " << spaceship.getPosition().y << std::endl;
            }
            if (spaceshipVisible) {
                // Clamp spaceship position
                sf::Vector2f shipPos = spaceship.getPosition();
                if (shipPos.x < 0) shipPos.x = 0;
                if (shipPos.x > window.getSize().x - spaceship.getGlobalBounds().width) shipPos.x = window.getSize().x - spaceship.getGlobalBounds().width;
                if (shipPos.y < collisionY) shipPos.y = collisionY;
                if (shipPos.y > floorY - spaceship.getGlobalBounds().height) shipPos.y = floorY - spaceship.getGlobalBounds().height;
                spaceship.setPosition(shipPos);

                // Check collision with player
                sf::Vector2f diff = playerPos - spaceship.getPosition();
                float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                if (dist < 60.0f) {
                    spaceshipVisible = false;
                    currentState = WIN;
                }
            }

            // Limpiar y dibujar
            window.clear();
            // Draw both background and foreground first, then the player sprite on top
            fondo.drawBackground(window);
            fondo.drawForeground(window);
            for(auto& e : enemigos) {
                e.draw(window);
            }
            if (spaceshipVisible) {
                window.draw(spaceship);
            }
            spriteJugador.draw(window);
            window.display();
        } else if (currentState == GAME_OVER) {
            // Stop all animations and open a new GAME OVER window
            window.close();
            sf::RenderWindow goWindow(sf::VideoMode(600, 400), "GAME OVER");
            sf::Text goText = gameOverText;
            sf::FloatRect b = goText.getLocalBounds();
            goText.setOrigin(b.width/2, b.height/2);
            goText.setPosition(300, 200);
            while (goWindow.isOpen()) {
                sf::Event event;
                while (goWindow.pollEvent(event)) {
                    if (event.type == sf::Event::Closed)
                        goWindow.close();
                    if (event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed)
                        goWindow.close();
                }
                goWindow.clear(sf::Color::Black);
                goWindow.draw(goText);
                goWindow.display();
            }
            // After closing GO window, exit game loop
            break;
        } else if (currentState == WIN) {
            // Similar for WIN
            window.close();
            sf::RenderWindow winWindow(sf::VideoMode(600, 400), "YOU WIN");
            sf::Text wText = winText;
            sf::FloatRect b = wText.getLocalBounds();
            wText.setOrigin(b.width/2, b.height/2);
            wText.setPosition(300, 200);
            while (winWindow.isOpen()) {
                sf::Event event;
                while (winWindow.pollEvent(event)) {
                    if (event.type == sf::Event::Closed)
                        winWindow.close();
                    if (event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed)
                        winWindow.close();
                }
                winWindow.clear(sf::Color::Black);
                winWindow.draw(wText);
                winWindow.display();
            }
            // After closing WIN window, exit game loop
            break;
        }
    }

    std::cout << "Programa terminado exitosamente." << std::endl;
}

Game::~Game() {
    std::cout << "Destructor Game called" << std::endl;
}