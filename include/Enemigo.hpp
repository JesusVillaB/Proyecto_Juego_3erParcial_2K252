#pragma once
#include "Personaje.hpp"
#include <vector>
#include <SFML/Graphics.hpp>

using std::vector;

class Objeto; // forward

class Enemigo : public Personaje {
private:
    vector<Objeto*> objetos;

    // Animación normal
    sf::Texture texture;
    sf::Clock animationClock;
    float frameTime;
    int currentFrame;
    int numFrames;
    int frameWidth;
    int frameHeight;

    // Animación de ataque
    sf::Texture attackTexture;
    sf::Clock attackClock;
    float attackFrameTime;
    int attackCurrentFrame;
    int attackNumFrames;
    int attackFrameWidth;
    int attackFrameHeight;
    vector<sf::Vector2f> attackTrail;
    vector<int> attackTrailFrame;
    size_t maxAttackTrail;
    bool isAttacking;

    // Movimiento aleatorio
    sf::Clock moveClock;
    float moveInterval; // tiempo entre movimientos aleatorios
    bool canMove; // si puede moverse o está atacando

    // Proyectiles
    sf::Texture bulletTexture;
    struct Projectile {
        sf::Vector2f position;
        sf::Vector2f velocity;
        int currentFrame;
        sf::Clock clock;
        vector<sf::Vector2f> trail;
        vector<int> trailFrame;
        size_t maxTrail;
        bool isHoming;
    };
    vector<Projectile> projectiles;
    float bulletFrameTime;
    int bulletNumFrames;
    int bulletFrameWidth;
    int bulletFrameHeight;

    // Health and death
    int hitCount;
    bool isDead;
    // Explosion animation
    bool exploding;
    int explosionFrame;
    int numExplosionFrames;
    float explosionFrameTime;
    sf::Texture explosionTexture;
    sf::Sprite explosionSprite;
    sf::Clock explosionClock;

    // Fallback explosion shape (circle)
    sf::CircleShape explosionShape;

public:
    vector<Projectile>& getProjectiles() { return projectiles; }
    bool isExploding() const { return exploding; }
    void takeDamage();
    bool isEnemyDead() const;
    void update(float deltaTime, sf::Vector2f playerPos, float collisionY, float floorY);
    void draw(sf::RenderWindow& window);
    Enemigo(sf::RenderWindow& window);
    Enemigo(sf::RenderWindow& window, sf::Vector2f pos);
    ~Enemigo();

private:
    void init(sf::RenderWindow& window);

    void patrullar(); // ahora para movimiento aleatorio
    void atacarJugador(sf::Vector2f playerPos);
    void morir();
};