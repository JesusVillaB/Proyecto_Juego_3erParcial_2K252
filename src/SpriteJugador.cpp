#include "../include/SpriteJugador.hpp"
#include <stdexcept>
#include <iostream>

SpriteJugador::SpriteJugador(sf::RenderWindow& win) : 
    window(win), 
    animationClock(),
    frameTime(0.1f), 
    currentFrame(0), 
    numFrames(8), 
    frameWidthRun(0), 
    frameHeightRun(0), 
    reloadCurrentFrame(0), 
    reloadNumFrames(4), 
    frameWidthReload(0), 
    frameHeightReload(0), 
    waitClock(), 
    isWaiting(false), 
    attackFrames{0, 3, 4}, 
    numAttackFrames(5), 
    frameWidthAttack(0), 
    frameHeightAttack(0), 
    attackCurrentIndex(0), 
    isAttacking(false), 
    attackClock(),
    attackFrameTime(0.1f), 
    bulletSpeed(300.0f), 
    bulletFrameTime(0.05f), 
    bulletNumFrames(4), 
    bulletFrameWidth(0), 
    bulletFrameHeight(0), 
    position(), 
    maxTrail(8), 
    velocidad(200.0f) 
{
    // Cargar texturas
    if (!texture.loadFromFile("assets/Jugador/Player_run..png")) {
        throw std::runtime_error("Error loading player run texture");
    }
    if (!reloadTexture.loadFromFile("assets/Jugador/player_Reload.png")) {
        throw std::runtime_error("Error loading player reload texture");
    }
    if (!attackTexture.loadFromFile("assets/Jugador/Player_idle_attack.png")) {
        throw std::runtime_error("Error loading player attack texture");
    }
    if (!bulletTexture.loadFromFile("assets/Objetos/Bullet 1.png")) {
        throw std::runtime_error("Error loading bullet texture");
    }

    // Inicializar tamaños de frame después de cargar texturas
    frameWidthRun = texture.getSize().x / numFrames;
    frameHeightRun = texture.getSize().y;
    frameWidthReload = reloadTexture.getSize().x / reloadNumFrames;
    frameHeightReload = reloadTexture.getSize().y;
    frameWidthAttack = attackTexture.getSize().x / numAttackFrames;
    frameHeightAttack = attackTexture.getSize().y;
    bulletFrameWidth = bulletTexture.getSize().x / bulletNumFrames;
    bulletFrameHeight = bulletTexture.getSize().y;

    // Start the player near the bottom of the window (slightly above the bottom edge)
    position = sf::Vector2f(50.0f, window.getSize().y / 2.0f - frameHeightRun / 2.0f); // Left side of screen
}

void SpriteJugador::update(float deltaTime, float collisionY, float floorY) {
    // Detectar movimiento
    bool isMoving = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down);

    // Interrumpir animación de ataque si se suelta la tecla Space y limpiar trail inmediatamente
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && isAttacking) {
        isAttacking = false;
        attackTrail.clear();
        attackTrailFrame.clear();
    }

    // Ataque con space
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !isAttacking) {
        isAttacking = true;
        attackCurrentIndex = 0;
        attackClock.restart();

        // Limpiar trails
        runTrail.clear();
        runTrailFrame.clear();
        reloadTrail.clear();
        reloadTrailFrame.clear();

        // Agregar proyectil
        sf::Vector2f weaponPos = position + sf::Vector2f(frameWidthRun, frameHeightRun / 4.0f);
        projectiles.push_back({weaponPos, 0, sf::Clock()});
    }

    // Movimiento
    if (isMoving && !isAttacking) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            position.x += -velocidad * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            position.x += velocidad * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            position.y += -velocidad * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            position.y += velocidad * deltaTime;
        }

        // Agregar a runTrail
        runTrail.push_back(position);
        runTrailFrame.push_back(currentFrame);
        if (runTrail.size() > maxTrail) {
            runTrail.erase(runTrail.begin());
            runTrailFrame.erase(runTrailFrame.begin());
        }

        // Limpiar reloadTrail
        reloadTrail.clear();
        reloadTrailFrame.clear();
    } else if (!isAttacking) {
        // Limpiar runTrail
        runTrail.clear();
        runTrailFrame.clear();

        // Agregar a reloadTrail
        if (!isWaiting) {
            reloadTrail.push_back(position);
            reloadTrailFrame.push_back(reloadCurrentFrame);
            if (reloadTrail.size() > maxTrail) {
                reloadTrail.erase(reloadTrail.begin());
                reloadTrailFrame.erase(reloadTrailFrame.begin());
            }
        }

        // Limpiar attackTrail
        attackTrail.clear();
        attackTrailFrame.clear();
    }

    // Actualizar ataque using clock
    if (isAttacking) {
        if (attackClock.getElapsedTime().asSeconds() >= attackFrameTime) {
            attackCurrentIndex++;
            if (attackCurrentIndex >= attackFrames.size()) {
                isAttacking = false;
                attackTrail.clear();
                attackTrailFrame.clear();
            } else {
                attackClock.restart();
                // Agregar a attackTrail
                attackTrail.push_back(position);
                attackTrailFrame.push_back(attackFrames[attackCurrentIndex]);
                if (attackTrail.size() > maxTrail) {
                    attackTrail.erase(attackTrail.begin());
                    attackTrailFrame.erase(attackTrailFrame.begin());
                }
            }
        }
    }

    // Actualizar proyectiles
    for (auto it = projectiles.begin(); it != projectiles.end(); ) {
        it->position.x += bulletSpeed * deltaTime;
        if (it->clock.getElapsedTime().asSeconds() >= bulletFrameTime) {
            it->currentFrame = (it->currentFrame + 1) % bulletNumFrames;
            it->clock.restart();
        }
        if (it->position.x > window.getSize().x) {
            it = projectiles.erase(it);
        } else {
            ++it;
        }
    }

    // Actualizar frame de animación using animationClock
    if (animationClock.getElapsedTime().asSeconds() >= frameTime) {
        if (isMoving && !isAttacking) {
            currentFrame = (currentFrame + 1) % numFrames;
        } else if (!isAttacking) {
            if (!isWaiting) {
                int prevFrame = reloadCurrentFrame;
                reloadCurrentFrame = (reloadCurrentFrame + 1) % reloadNumFrames;
                if (reloadCurrentFrame == 0 && prevFrame == reloadNumFrames - 1) {
                    isWaiting = true;
                    waitClock.restart();
                }
            } else {
                if (waitClock.getElapsedTime().asSeconds() >= 8.0f) {
                    isWaiting = false;
                }
            }
        }
        animationClock.restart();
    }

    // Colisión: clamp posición del jugador para mantenerlo entre collisionY (top limit) and floorY (ground)
    if (position.x < 0) position.x = 0;
    if (position.x > window.getSize().x - frameWidthRun) position.x = window.getSize().x - frameWidthRun;
    if (position.y < collisionY) position.y = collisionY;
    if (position.y > floorY - frameHeightRun) position.y = floorY - frameHeightRun;
}

void SpriteJugador::draw(sf::RenderWindow& window) {
    // Dibujar trails
    for (size_t i = 0; i < runTrail.size(); ++i) {
        sf::Sprite trailSprite(texture);
        trailSprite.setPosition(runTrail[i]);
        trailSprite.setTextureRect(sf::IntRect(runTrailFrame[i] * frameWidthRun, 0, frameWidthRun, frameHeightRun));
        sf::Color color = sf::Color::White;
        color.a = static_cast<sf::Uint8>((i + 1) * 255 / runTrail.size());
        trailSprite.setColor(color);
        window.draw(trailSprite);
    }

    for (size_t i = 0; i < reloadTrail.size(); ++i) {
        sf::Sprite trailSprite(reloadTexture);
        trailSprite.setPosition(reloadTrail[i]);
        trailSprite.setTextureRect(sf::IntRect(reloadTrailFrame[i] * frameWidthReload, 0, frameWidthReload, frameHeightReload));
        sf::Color color = sf::Color::White;
        color.a = static_cast<sf::Uint8>((i + 1) * 255 / reloadTrail.size());
        trailSprite.setColor(color);
        window.draw(trailSprite);
    }

    for (size_t i = 0; i < attackTrail.size(); ++i) {
        sf::Sprite trailSprite(attackTexture);
        trailSprite.setPosition(attackTrail[i]);
        trailSprite.setTextureRect(sf::IntRect(attackTrailFrame[i] * frameWidthAttack, 0, frameWidthAttack, frameHeightAttack));
        sf::Color color = sf::Color::White;
        color.a = static_cast<sf::Uint8>((i + 1) * 255 / attackTrail.size());
        trailSprite.setColor(color);
        window.draw(trailSprite);
    }

    // Dibujar proyectiles
    for (const auto& proj : projectiles) {
        sf::Sprite bulletSprite(bulletTexture);
        bulletSprite.setPosition(proj.position);
        bulletSprite.setTextureRect(sf::IntRect(proj.currentFrame * bulletFrameWidth, 0, bulletFrameWidth, bulletFrameHeight));
        window.draw(bulletSprite);
    }

    // Dibujar jugador
    sf::Sprite playerSprite;
    if (isAttacking) {
        playerSprite.setTexture(attackTexture);
        playerSprite.setTextureRect(sf::IntRect(attackFrames[attackCurrentIndex] * frameWidthAttack, 0, frameWidthAttack, frameHeightAttack));
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
               sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        playerSprite.setTexture(texture);
        playerSprite.setTextureRect(sf::IntRect(currentFrame * frameWidthRun, 0, frameWidthRun, frameHeightRun));
    } else {
        playerSprite.setTexture(reloadTexture);
        playerSprite.setTextureRect(sf::IntRect(reloadCurrentFrame * frameWidthReload, 0, frameWidthReload, frameHeightReload));
    }
    playerSprite.setPosition(position);
    window.draw(playerSprite);
}

SpriteJugador::~SpriteJugador() {
    std::cout << "Destructor SpriteJugador called" << std::endl;
}