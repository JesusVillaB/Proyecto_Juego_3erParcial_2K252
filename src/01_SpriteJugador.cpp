#include <SFML/Graphics.hpp>
#include "../include/Jugador.hpp"
#include <vector>
#include <cmath>

using std::vector;

float velocidad = 200.0f; // pixels per second

struct Projectile {
    sf::Vector2f position;
    int currentFrame;
    sf::Clock clock;
};

int main() {
    // Crear una ventana
    sf::RenderWindow window(sf::VideoMode(800, 600), "Sprite del Jugador Animado con Efecto de Movimiento");

    // Cargar la textura del jugador corriendo
    sf::Texture texture;
    if (!texture.loadFromFile("assets/Jugador/Player_run..png")) {
        return -1;
    }

    // Cargar la textura del jugador recargando
    sf::Texture reloadTexture;
    if (!reloadTexture.loadFromFile("assets/Jugador/player_Reload.png")) {
        return -1;
    }

    // Cargar la textura del ataque
    sf::Texture attackTexture;
    if (!attackTexture.loadFromFile("assets/Jugador/Player_idle_attack.png")) {
        return -1;
    }

    // Cargar la textura de la bala
    sf::Texture bulletTexture;
    if (!bulletTexture.loadFromFile("assets/Objetos/Bullet 1.png")) {
        return -1;
    }

    // Crear un objeto Jugador
    Jugador* jugador = new Jugador();

    // Variables para la animación de correr
    sf::Clock clock;
    float frameTime = 0.1f; // Tiempo de 0.1 segundos entre cada frame
    int currentFrame = 0;
    int numFrames = 8; // Número total de frames en la animación de correr
    int frameWidthRun = texture.getSize().x / numFrames;
    int frameHeightRun = texture.getSize().y;

    // Variables para la animación de recargar
    int reloadCurrentFrame = 0;
    int reloadNumFrames = 4; // Número total de frames en la animación de recargar
    int frameWidthReload = reloadTexture.getSize().x / reloadNumFrames;
    int frameHeightReload = reloadTexture.getSize().y;
    sf::Clock waitClock;
    bool isWaiting = false;

    // Variables para la animación de ataque
    vector<int> attackFrames = {0, 3, 4}; // Primer, cuarto y quinto frame
    int numAttackFrames = 5; // Asumiendo 5 frames totales
    int frameWidthAttack = attackTexture.getSize().x / numAttackFrames;
    int frameHeightAttack = attackTexture.getSize().y;
    int attackCurrentIndex = 0;
    bool isAttacking = false;
    sf::Clock attackClock;
    float attackFrameTime = 0.1f;

    // Variables para la bala
    float bulletSpeed = 300.0f; // Velocidad de la bala
    float bulletFrameTime = 0.05f;
    int bulletNumFrames = 4;
    int bulletFrameWidth = bulletTexture.getSize().x / bulletNumFrames;
    int bulletFrameHeight = bulletTexture.getSize().y;

    // Declarar variables para posición y trails
    // Initial player position: bottom of the window
    sf::Vector2f position(window.getSize().x / 2.0f - frameWidthRun / 2.0f, window.getSize().y - frameHeightRun - 10.0f);
    vector<sf::Vector2f> runTrail;
    vector<int> runTrailFrame;
    vector<sf::Vector2f> reloadTrail;
    vector<int> reloadTrailFrame;
    vector<sf::Vector2f> attackTrail;
    vector<int> attackTrailFrame;
    int maxTrail = 8; // Máximo número de frames en el trail
    vector<Projectile> projectiles;

    // Bucle principal
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Calcular delta time
        float deltaTime = clock.restart().asSeconds();

        // Detectar si se está moviendo
        bool isMoving = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
                        sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down);

        // Ataque con space
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !isAttacking) {
            isAttacking = true;
            attackCurrentIndex = 0;
            attackClock.restart();

            // Limpiar trails de run y reload
            runTrail.clear();
            runTrailFrame.clear();
            reloadTrail.clear();
            reloadTrailFrame.clear();

            // Agregar proyectil desde el límite derecho del frame (1/4 abajo de la parte superior)
            sf::Vector2f weaponPos = position + sf::Vector2f(frameWidthRun, frameHeightRun / 4.0f);
            projectiles.push_back({weaponPos, 0, sf::Clock()});
        }

        // Movimiento del jugador con teclado (horizontal y vertical)
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

            // Agregar posición y frame al runTrail
            runTrail.push_back(position);
            runTrailFrame.push_back(currentFrame);
            if (runTrail.size() > maxTrail) {
                runTrail.erase(runTrail.begin());
                runTrailFrame.erase(runTrailFrame.begin());
            }

            // Limpiar reloadTrail cuando se mueve
            reloadTrail.clear();
            reloadTrailFrame.clear();
        } else if (!isAttacking) {
            // Limpiar runTrail cuando está detenido
            runTrail.clear();
            runTrailFrame.clear();

            // Agregar posición y frame al reloadTrail para efecto estático con 4 frames superpuestos
            if (!isWaiting) {
                reloadTrail.push_back(position);
                reloadTrailFrame.push_back(reloadCurrentFrame);
                if (reloadTrail.size() > maxTrail) {
                    reloadTrail.erase(reloadTrail.begin());
                    reloadTrailFrame.erase(reloadTrailFrame.begin());
                }
            }

            // Limpiar attackTrail cuando no se está atacando
            attackTrail.clear();
            attackTrailFrame.clear();
        }

        // Actualizar ataque
        if (isAttacking) {
            if (attackClock.getElapsedTime().asSeconds() >= attackFrameTime) {
                attackCurrentIndex++;
                if (attackCurrentIndex >= attackFrames.size()) {
                    isAttacking = false;
                    // Limpiar attackTrail al terminar el ataque
                    attackTrail.clear();
                    attackTrailFrame.clear();
                } else {
                    attackClock.restart();
                }
            }

            // Agregar posición y frame al attackTrail
            attackTrail.push_back(position);
            attackTrailFrame.push_back(attackFrames[attackCurrentIndex]);
            if (attackTrail.size() > maxTrail) {
                attackTrail.erase(attackTrail.begin());
                attackTrailFrame.erase(attackTrailFrame.begin());
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

        // Colisión: clamp posición del jugador entre top collision and floor at bottom
        float collisionY = window.getSize().y * 0.5f; // collision at 50% (middle) of window height
        float floorY = window.getSize().y; // floor at bottom
        if (position.x < 0) position.x = 0;
        if (position.x > window.getSize().x - frameWidthRun) position.x = window.getSize().x - frameWidthRun;
        if (position.y < collisionY) position.y = collisionY;
        if (position.y > floorY - frameHeightRun) position.y = floorY - frameHeightRun;

        // Actualizar posición
        jugador->setPosition(position);

        // Actualizar el jugador
        jugador->actualizar();

        // Actualizar el frame de la animación
        static sf::Clock animationClock;
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

        // Establecer la textura y rectángulo del sprite
        if (isAttacking) {
            jugador->setTexture(attackTexture);
            jugador->setTextureRect(sf::IntRect(attackFrames[attackCurrentIndex] * frameWidthAttack, 0, frameWidthAttack, frameHeightAttack));
        } else if (isMoving) {
            jugador->setTexture(texture);
            jugador->setTextureRect(sf::IntRect(currentFrame * frameWidthRun, 0, frameWidthRun, frameHeightRun));
        } else {
            jugador->setTexture(reloadTexture);
            jugador->setTextureRect(sf::IntRect(reloadCurrentFrame * frameWidthReload, 0, frameWidthReload, frameHeightReload));
        }

        // Limpiar la ventana
        window.clear();

        // Dibujar el runTrail (efecto de movimiento)
        for (size_t i = 0; i < runTrail.size(); ++i) {
            sf::Sprite trailSprite(texture);
            trailSprite.setPosition(runTrail[i]);
            trailSprite.setTextureRect(sf::IntRect(runTrailFrame[i] * frameWidthRun, 0, frameWidthRun, frameHeightRun));
            sf::Color color = sf::Color::White;
            color.a = static_cast<sf::Uint8>((i + 1) * 255 / runTrail.size());
            trailSprite.setColor(color);
            window.draw(trailSprite);
        }

        // Dibujar el reloadTrail (efecto estático con frames superpuestos)
        for (size_t i = 0; i < reloadTrail.size(); ++i) {
            sf::Sprite trailSprite(reloadTexture);
            trailSprite.setPosition(reloadTrail[i]);
            trailSprite.setTextureRect(sf::IntRect(reloadTrailFrame[i] * frameWidthReload, 0, frameWidthReload, frameHeightReload));
            sf::Color color = sf::Color::White;
            color.a = static_cast<sf::Uint8>((i + 1) * 255 / reloadTrail.size());
            trailSprite.setColor(color);
            window.draw(trailSprite);
        }

        // Dibujar el attackTrail (efecto de ataque con frames superpuestos)
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

        // Renderizar el jugador actual
        jugador->renderizar(window);

        // Mostrar la ventana
        window.display();
    }

    // Liberar memoria
    delete jugador;

    return 0;
}
