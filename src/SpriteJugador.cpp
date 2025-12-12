#include "../include/SpriteJugador.hpp"
#include <stdexcept>
#include <iostream>
#include <cmath>

SpriteJugador::SpriteJugador(sf::RenderWindow& win) : 
    window(win), 
    hitCount(0),
    isHit(false),
    shakeOffset(0.0f),
    isDying(false),
    deathFrame(0),
    numDeathFrames(10), // Assume 10 frames
    deathFrameWidth(0),
    deathFrameHeight(0),
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

    // Load health textures
    if (!healthTextures[0].loadFromFile("assets/Efectos visuales/VIDA_10.png")) {
        throw std::runtime_error("Error loading VIDA_10 texture");
    }
    if (!healthTextures[1].loadFromFile("assets/Efectos visuales/VIDA_8.png")) {
        throw std::runtime_error("Error loading VIDA_8 texture");
    }
    if (!healthTextures[2].loadFromFile("assets/Efectos visuales/VIDA_7.png")) {
        throw std::runtime_error("Error loading VIDA_7 texture");
    }
    if (!healthTextures[3].loadFromFile("assets/Efectos visuales/VIDA_4.png")) {
        throw std::runtime_error("Error loading VIDA_4 texture");
    }
    if (!healthTextures[4].loadFromFile("assets/Efectos visuales/VIDA_1.png")) {
        throw std::runtime_error("Error loading VIDA_1 texture");
    }
    healthSprite.setTexture(healthTextures[0]);
    healthSprite.setPosition(10, 10); // Top left

    // Load death texture
    if (!deathTexture.loadFromFile("assets/Jugador/Player_death_type1.png")) {
        throw std::runtime_error("Error loading death texture");
    }
    deathFrameWidth = deathTexture.getSize().x / numDeathFrames;
    deathFrameHeight = deathTexture.getSize().y;
    deathSprite.setTexture(deathTexture);
    deathSprite.setPosition(position); // Will update to current position

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
    if (isDying) {
        // Advance death animation according to 1 second total duration
        if (deathFrame < numDeathFrames) {
            if (deathClock.getElapsedTime().asSeconds() >= deathFrameTime) {
                deathFrame++;
                deathClock.restart();
            }
        }
        return; // No other updates during death
    }

    // Hit vibration
    if (isHit) {
        if (hitClock.getElapsedTime().asSeconds() > 0.5f) {
            isHit = false;
        }
    }

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

    // Apply hit vibration (compute offset only, do not change position)
    if (isHit) {
        // Increase amplitude and slow frequency for visibility
        shakeOffset = 8.0f * std::sin(hitClock.getElapsedTime().asSeconds() * 12.0f);
        if (hitClock.getElapsedTime().asSeconds() > 0.5f) {
            isHit = false; // stop vibration after 0.5s
        }
    } else {
        shakeOffset = 0.0f;
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

    // Draw projectiles
    for (const auto& proj : projectiles) {
        sf::Sprite bulletSprite(bulletTexture);
        bulletSprite.setPosition(proj.position);
        bulletSprite.setTextureRect(sf::IntRect(proj.currentFrame * bulletFrameWidth, 0, bulletFrameWidth, bulletFrameHeight));
        window.draw(bulletSprite);
    }

    if (isDying) {
        // During death animation, only draw death frames and health; clear everything else
        // Center the death sprite so frames don't shift
        if (deathFrame >= numDeathFrames) deathFrame = numDeathFrames - 1;
        deathSprite.setTextureRect(sf::IntRect(deathFrame * deathFrameWidth, 0, deathFrameWidth, deathFrameHeight));
        deathSprite.setOrigin(deathFrameWidth / 2.0f, deathFrameHeight / 2.0f);
        sf::Vector2f centerPos = position + sf::Vector2f(frameWidthRun / 2.0f, frameHeightRun / 2.0f);
        deathSprite.setPosition(centerPos);
        window.draw(deathSprite);
    } else {
        // Dibujar jugador centrado (usando pivote en centro de frame) y aplicar vibración en la posición de dibujo
        sf::Sprite playerSprite;
        int pw = frameWidthRun;
        int ph = frameHeightRun;
        if (isAttacking) {
            playerSprite.setTexture(attackTexture);
            playerSprite.setTextureRect(sf::IntRect(attackFrames[attackCurrentIndex] * frameWidthAttack, 0, frameWidthAttack, frameHeightAttack));
            // use attack frame size for origin
            playerSprite.setOrigin(frameWidthAttack / 2.0f, frameHeightAttack / 2.0f);
            // center position based on player's run frame center
            playerSprite.setPosition(position + sf::Vector2f(pw/2.0f + shakeOffset, ph/2.0f));
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
                   sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            playerSprite.setTexture(texture);
            playerSprite.setTextureRect(sf::IntRect(currentFrame * frameWidthRun, 0, frameWidthRun, frameHeightRun));
            playerSprite.setOrigin(pw / 2.0f, ph / 2.0f);
            playerSprite.setPosition(position + sf::Vector2f(pw/2.0f + shakeOffset, ph/2.0f));
        } else {
            playerSprite.setTexture(reloadTexture);
            playerSprite.setTextureRect(sf::IntRect(reloadCurrentFrame * frameWidthReload, 0, frameWidthReload, frameHeightReload));
            // For interpolation, use reload frame size origin if different
            playerSprite.setOrigin(frameWidthReload / 2.0f, frameHeightReload / 2.0f);
            playerSprite.setPosition(position + sf::Vector2f(pw/2.0f + shakeOffset, ph/2.0f));
        }
        window.draw(playerSprite);
    }

    // Draw health
    window.draw(healthSprite);
}


void SpriteJugador::takeDamage() {
    hitCount++;
    isHit = true;
    hitClock.restart();
    if (hitCount == 1) {
        healthSprite.setTexture(healthTextures[1]); // VIDA_8
    } else if (hitCount == 2) {
        healthSprite.setTexture(healthTextures[2]); // VIDA_7
    } else if (hitCount == 3) {
        healthSprite.setTexture(healthTextures[3]); // VIDA_4
    } else if (hitCount == 4) {
        healthSprite.setTexture(healthTextures[4]); // VIDA_1
        // Clean previous visuals so the death animation is the only thing visible
        runTrail.clear();
        runTrailFrame.clear();
        reloadTrail.clear();
        reloadTrailFrame.clear();
        attackTrail.clear();
        attackTrailFrame.clear();
        projectiles.clear();

        isHit = false;
        shakeOffset = 0.0f;

        isDying = true;
        deathFrame = 0;
        deathClock.restart();
        deathFrameTime = 1.0f / static_cast<float>(numDeathFrames);
    } else {
        // Trigger immediate visible vibration for non-fatal hits
        isHit = true;
        shakeOffset = 8.0f; // immediate visible offset
        hitClock.restart();
    }
}

bool SpriteJugador::isDead() const {
    return hitCount >= 4 && deathFrame >= numDeathFrames;
}

SpriteJugador::~SpriteJugador() {
    (void)window;
    std::cout << "Destructor SpriteJugador called" << std::endl;
}

