#include "../include/Enemigo.hpp"
#include <iostream>
#include <cstdlib> // for rand()
#include <cmath> // for sqrt, atan2
#include <algorithm> // for max

Enemigo::Enemigo(sf::RenderWindow& window) : 
    frameTime(0.1f), currentFrame(0), numFrames(8), frameWidth(0), frameHeight(0),
    attackFrameTime(0.2f), attackCurrentFrame(0), attackNumFrames(5), attackFrameWidth(0), attackFrameHeight(0), maxAttackTrail(5), isAttacking(false),
    moveInterval(1.0f), canMove(true),
    bulletFrameTime(0.05f), bulletNumFrames(4), bulletFrameWidth(0), bulletFrameHeight(0),
    hitCount(0), isDead(false), exploding(false), explosionFrame(0), numExplosionFrames(7), explosionFrameTime(0.4f / 7.0f) {
    init(window);
    setPosition(sf::Vector2f(400, 300));
}

Enemigo::Enemigo(sf::RenderWindow& window, sf::Vector2f pos) : 
    frameTime(0.1f), currentFrame(0), numFrames(8), frameWidth(0), frameHeight(0),
    attackFrameTime(0.2f), attackCurrentFrame(0), attackNumFrames(5), attackFrameWidth(0), attackFrameHeight(0), maxAttackTrail(5), isAttacking(false),
    moveInterval(1.0f), canMove(true),
    bulletFrameTime(0.05f), bulletNumFrames(4), bulletFrameWidth(0), bulletFrameHeight(0),
    hitCount(0), isDead(false), exploding(false), explosionFrame(0), numExplosionFrames(7), explosionFrameTime(0.4f / 7.0f) {
    init(window);
    setPosition(pos);
}

void Enemigo::init(sf::RenderWindow& window) {
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

    if (!explosionTexture.loadFromFile("assets/Efectos visuales/Explosion enemiga.png")) {
        std::cerr << "Error loading explosion texture" << std::endl;
        // Do not return; allow enemy to exist without explosion texture
    } else {
        explosionSprite.setTexture(explosionTexture);
        int explosionFrameWidth = explosionTexture.getSize().x / numExplosionFrames;
        int explosionFrameHeight = explosionTexture.getSize().y;
        explosionSprite.setOrigin(explosionFrameWidth / 2.0f, explosionFrameHeight / 2.0f);

        // Calculate desired diameter
        float desiredDiameter = static_cast<float>(std::max(frameWidth, frameHeight)) * 1.8f;
        // Initialize circle-based shape with desired diameter
        explosionShape.setRadius(desiredDiameter / 2.0f);
        explosionShape.setOrigin(desiredDiameter / 2.0f, desiredDiameter / 2.0f);
        explosionShape.setFillColor(sf::Color(255,140,0,255));

        std::cout << "Loaded explosion texture: size=" << explosionTexture.getSize().x << "x" << explosionTexture.getSize().y << " frames=" << numExplosionFrames << "\n";
    }

    setTexture(texture);
    setPosition(sf::Vector2f(400, 300)); // Initial position
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
                    // 50% homing, 50% deviate with random direction
                    if (rand() % 100 < 50) {
                        // Homing
                        projectiles.push_back({enemyPos, velocity, 0, sf::Clock(), {}, {}, 5, true});
                    } else {
                        // Deviate: random direction
                        float angle = (rand() % 360) * 3.14159f / 180.0f;
                        direction = sf::Vector2f(std::cos(angle), std::sin(angle));
                        velocity = direction * speed;
                        projectiles.push_back({enemyPos, velocity, 0, sf::Clock(), {}, {}, 5, false});
                    }
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
        // Homing: recalcular dirección hacia playerPos only if homing
        if (it->isHoming) {
            sf::Vector2f direction = playerPos - it->position;
            float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            if (distance > 0) {
                direction /= distance;
                float speed = 200.0f;
                it->velocity = direction * speed;
            }
        }
        it->velocity.y += 100.0f * deltaTime; // Gravedad
        it->position += it->velocity * deltaTime;

        // Do not erase projectiles here when they reach the player; Game will handle collisions and deletion
        (void)playerPos; // unused here
        // Keep projectile movement and animation


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

        // Remover si sale de pantalla (doubled range)
        if (it->position.x < -800 || it->position.x > 1600 || it->position.y < -600 || it->position.y > 1200) {
            it = projectiles.erase(it);
        } else {
            ++it;
        }
    }

    // Update explosion
    if (exploding) {
        if (explosionFrame < numExplosionFrames - 1) {
            if (explosionClock.getElapsedTime().asSeconds() >= explosionFrameTime) {
                explosionFrame++;
                std::cout << "Explosion frame " << explosionFrame << " / " << numExplosionFrames << "\n";
                explosionClock.restart();
            }
        } else {
            // We're on last frame; keep it visible for one more frameTime then mark for removal
            if (explosionClock.getElapsedTime().asSeconds() >= explosionFrameTime) {
                std::cout << "Explosion finished\n";
                isDead = true;
            }
        }
    }
}

void Enemigo::draw(sf::RenderWindow& window) {
    if (exploding) {
        if (explosionFrame < numExplosionFrames) {
            // Use circle-based animation with a growing radius and fading alpha
            float desiredDiameter = static_cast<float>(std::max(frameWidth, frameHeight)) * 1.8f;
            float radius = (desiredDiameter * (static_cast<float>(explosionFrame + 1) / static_cast<float>(numExplosionFrames))) / 2.0f;
            explosionShape.setRadius(radius);
            explosionShape.setOrigin(radius, radius);
            // Fade out alpha across frames
            sf::Uint8 alpha = static_cast<sf::Uint8>(255.0f * (1.0f - static_cast<float>(explosionFrame) / static_cast<float>(numExplosionFrames - 1)));
            sf::Color c = explosionShape.getFillColor();
            c.a = alpha;
            explosionShape.setFillColor(c);
            window.draw(explosionShape);
        }
        return; // Don't draw enemy
    }

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

void Enemigo::takeDamage() {
    hitCount++;
    if (hitCount >= 4 && !exploding) {
        // Start explosion animation at enemy center and clear enemy visuals
        exploding = true;
        explosionFrame = 0;
        // Ensure frame timing is based on number of frames (total 0.4s)
        if (numExplosionFrames <= 0) numExplosionFrames = 1;
        explosionFrameTime = 0.4f / static_cast<float>(numExplosionFrames);
        explosionClock.restart();

        // Set explosion position to current enemy position (centered)
        sf::Vector2f enemyPos = getPosition();
        explosionSprite.setPosition(enemyPos.x + frameWidth / 2.0f, enemyPos.y + frameHeight / 2.0f);
        explosionShape.setPosition(enemyPos.x + frameWidth / 2.0f, enemyPos.y + frameHeight / 2.0f);

        // Log start
        std::cout << "Enemy explosion started at (" << enemyPos.x << "," << enemyPos.y << ")\n";

        // Clear any lingering visuals and projectiles at this enemy
        projectiles.clear();
        attackTrail.clear();
        attackTrailFrame.clear();

        // Immediately set the first frame visible and setup circle radius
        if (numExplosionFrames > 0) {
            float desiredDiameter = static_cast<float>(std::max(frameWidth, frameHeight)) * 1.8f;
            float radius = desiredDiameter * (1.0f / (2.0f * static_cast<float>(numExplosionFrames)));
            explosionShape.setRadius(radius);
            explosionShape.setOrigin(radius, radius);
            explosionShape.setFillColor(sf::Color(255,140,0,255));
        }

        // Stop movement/attacking
        canMove = false;
        isAttacking = false;
    }
}

bool Enemigo::isEnemyDead() const {
    return isDead;
}
