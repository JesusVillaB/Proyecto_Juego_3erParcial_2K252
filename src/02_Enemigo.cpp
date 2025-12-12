#include "../include/Enemigo.hpp"
#include <iostream>
#include <cstdlib> // for rand()
#include <cmath> // for sqrt, atan2

Enemigo::Enemigo(sf::RenderWindow& window) : 
    frameTime(0.1f), currentFrame(0), numFrames(8), frameWidth(0), frameHeight(0),
    attackFrameTime(0.2f), attackCurrentFrame(0), attackNumFrames(5), attackFrameWidth(0), attackFrameHeight(0), maxAttackTrail(5), isAttacking(false),
    moveInterval(1.0f), canMove(true),
    bulletFrameTime(0.05f), bulletNumFrames(4), bulletFrameWidth(0), bulletFrameHeight(0) {
    (void)window; // Suppress unused parameter warning
    if (!texture.loadFromFile("assets/Enemigos/Robot.png")) {
        std::cerr << "Error loading enemy texture" << std::endl;
        return;
    }
    frameWidth = texture.getSize().x / numFrames;
    frameHeight = texture.getSize().y;

    if (!attackTexture.loadFromFile("assets/Enemigos/Robot_disparo.png")) {
        std::cerr << "Error loading enemy attack texture" << std::endl;
        return;
    }
    attackFrameWidth = attackTexture.getSize().x / attackNumFrames;
    attackFrameHeight = attackTexture.getSize().y;

    if (!bulletTexture.loadFromFile("assets/Objetos/Bulle 1_enemy.png")) {
        std::cerr << "Error loading bullet texture" << std::endl;
        return;
    }
    bulletFrameWidth = bulletTexture.getSize().x / bulletNumFrames;
    bulletFrameHeight = bulletTexture.getSize().y;

    setTexture(texture);
    setPosition(sf::Vector2f(400, 300)); // Initial position
}

Enemigo::Enemigo(sf::RenderWindow& window, sf::Vector2f pos) : Enemigo(window) {
    setPosition(pos);
}

Enemigo::~Enemigo() {
    // Cleanup if needed
}

void Enemigo::patrullar() {
    if (!canMove) return; // No moverse si está atacando
    // Movimiento aleatorio pequeño: cada moveInterval, mover unos pocos pasos en x e y
    if (moveClock.getElapsedTime().asSeconds() >= moveInterval) {
        int deltaX = (rand() % 61) - 30; // -30 to 30
        int deltaY = (rand() % 61) - 30; // -30 to 30
        sf::Vector2f pos = getPosition();
        pos.x += deltaX;
        pos.y += deltaY;
        // Clamp to window bounds roughly
        if (pos.x < 0) pos.x = 0;
        if (pos.x > 800 - frameWidth) pos.x = 800 - frameWidth;
        if (pos.y < 0) pos.y = 0;
        if (pos.y > 600 - frameHeight) pos.y = 600 - frameHeight;
        setPosition(pos);
        moveClock.restart();
    }
}

void Enemigo::atacarJugador(sf::Vector2f playerPos) {
    (void)playerPos; // Suppress unused parameter warning
    if (isAttacking) return; // Ya está atacando
    isAttacking = true;
    canMove = false; // Detener movimiento
    attackCurrentFrame = 0;
    attackClock.restart();
    attackTrail.clear();
    attackTrailFrame.clear();

    // Al final del ataque, disparar proyectil
    // Se hará en update cuando termine la animación
}

void Enemigo::morir() {
    // Implement death logic
}

void Enemigo::update(float deltaTime, sf::Vector2f playerPos, float collisionY, float floorY) {
    (void)deltaTime; // Suppress unused parameter warning

    // Clamp position like SpriteJugador
    sf::Vector2f pos = getPosition();
    if (pos.x < 0) pos.x = 0;
    if (pos.x > 800 - frameWidth) pos.x = 800 - frameWidth;
    if (pos.y < collisionY) pos.y = collisionY;
    if (pos.y > floorY - frameHeight) pos.y = floorY - frameHeight;
    setPosition(pos);

    // Cada 1s, iniciar ataque
    static sf::Clock attackTimer;
    if (attackTimer.getElapsedTime().asSeconds() >= 1.0f && !isAttacking) {
        atacarJugador(playerPos);
        attackTimer.restart();
    }

    if (isAttacking) {
        // Animación de ataque
        if (attackClock.getElapsedTime().asSeconds() >= attackFrameTime) {
            attackCurrentFrame++;
            if (attackCurrentFrame >= attackNumFrames) {
                // Fin del ataque, disparar proyectil
                sf::Vector2f enemyPos = getPosition();
                sf::Vector2f direction = playerPos - enemyPos;
                float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                if (distance > 0) {
                    direction /= distance; // Normalizar
                    float speed = 200.0f; // Velocidad del proyectil
                    sf::Vector2f velocity = direction * speed;
                    // Añadir componente vertical para arco
                    velocity.y += 50.0f; // Arco hacia arriba inicialmente
                    projectiles.push_back({enemyPos, velocity, 0, sf::Clock(), {}, {}, 5});
                }
                isAttacking = false;
                canMove = true;
                attackTrail.clear();
                attackTrailFrame.clear();
            } else {
                attackClock.restart();
                // Add to attack trail
                attackTrail.push_back(getPosition());
                attackTrailFrame.push_back(attackCurrentFrame);
                if (attackTrail.size() > maxAttackTrail) {
                    attackTrail.erase(attackTrail.begin());
                    attackTrailFrame.erase(attackTrailFrame.begin());
                }
            }
        }
    } else {
        // Animación normal
        if (animationClock.getElapsedTime().asSeconds() >= frameTime) {
            currentFrame = (currentFrame + 1) % numFrames;
            animationClock.restart();
        }
    }

    // Update random movement if not attacking
    if (canMove) {
        patrullar();
    }

    // Update projectiles
    for (auto it = projectiles.begin(); it != projectiles.end(); ) {
        // Homing: recalcular dirección hacia playerPos
        sf::Vector2f direction = playerPos - it->position;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (distance > 0) {
            direction /= distance;
            float speed = 200.0f;
            it->velocity = direction * speed;
        }
        it->velocity.y += 100.0f * deltaTime; // Gravedad
        it->position += it->velocity * deltaTime;

        // Check collision with player
        sf::Vector2f diff = playerPos - it->position;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist < 30.0f) { // Assuming player radius ~30
            it = projectiles.erase(it);
            continue;
        }

        // Animación del proyectil
        if (it->clock.getElapsedTime().asSeconds() >= bulletFrameTime) {
            it->currentFrame = (it->currentFrame + 1) % bulletNumFrames;
            it->clock.restart();
            // Add to trail
            it->trail.push_back(it->position);
            it->trailFrame.push_back(it->currentFrame);
            if (it->trail.size() > it->maxTrail) {
                it->trail.erase(it->trail.begin());
                it->trailFrame.erase(it->trailFrame.begin());
            }
        }

        // Remover si sale de pantalla
        if (it->position.x < 0 || it->position.x > 800 || it->position.y < 0 || it->position.y > 600) {
            it = projectiles.erase(it);
        } else {
            ++it;
        }
    }
}

void Enemigo::draw(sf::RenderWindow& window) {
    if (isAttacking) {
        // Draw attack trail
        for (size_t i = 0; i < attackTrail.size(); ++i) {
            sf::Sprite trailSprite(attackTexture);
            trailSprite.setPosition(attackTrail[i]);
            trailSprite.setTextureRect(sf::IntRect(attackTrailFrame[i] * attackFrameWidth, 0, attackFrameWidth, attackFrameHeight));
            sf::Color color = sf::Color::White;
            color.a = static_cast<sf::Uint8>((i + 1) * 255 / attackTrail.size());
            trailSprite.setColor(color);
            window.draw(trailSprite);
        }

        // Draw current attack frame
        setTexture(attackTexture);
        setTextureRect(sf::IntRect(attackCurrentFrame * attackFrameWidth, 0, attackFrameWidth, attackFrameHeight));
    } else {
        // Draw normal frame
        setTexture(texture);
        setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
    }
    renderizar(window);

    // Draw projectiles
    for (const auto& proj : projectiles) {
        // Draw trail
        for (size_t i = 0; i < proj.trail.size(); ++i) {
            sf::Sprite trailSprite(bulletTexture);
            trailSprite.setPosition(proj.trail[i]);
            trailSprite.setTextureRect(sf::IntRect(proj.trailFrame[i] * bulletFrameWidth, 0, bulletFrameWidth, bulletFrameHeight));
            sf::Color color = sf::Color::White;
            color.a = static_cast<sf::Uint8>((i + 1) * 255 / proj.trail.size());
            trailSprite.setColor(color);
            window.draw(trailSprite);
        }

        // Draw current bullet
        sf::Sprite bulletSprite(bulletTexture);
        bulletSprite.setPosition(proj.position);
        bulletSprite.setTextureRect(sf::IntRect(proj.currentFrame * bulletFrameWidth, 0, bulletFrameWidth, bulletFrameHeight));
        window.draw(bulletSprite);
    }
}
