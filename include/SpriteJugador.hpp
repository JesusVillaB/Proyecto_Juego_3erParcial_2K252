#ifndef SPRITEJUGADOR_HPP
#define SPRITEJUGADOR_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class SpriteJugador {
public:
    SpriteJugador(sf::RenderWindow& window);
    ~SpriteJugador();
    void update(float deltaTime, float collisionY, float floorY);
    void draw(sf::RenderWindow& window);

private:
    sf::RenderWindow& window;

    // Texturas
    sf::Texture texture;
    sf::Texture reloadTexture;
    sf::Texture attackTexture;
    sf::Texture bulletTexture;

    // Variables para animación
    sf::Clock animationClock;
    float frameTime;
    int currentFrame;
    int numFrames;
    int frameWidthRun;
    int frameHeightRun;

    int reloadCurrentFrame;
    int reloadNumFrames;
    int frameWidthReload;
    int frameHeightReload;
    sf::Clock waitClock;
    bool isWaiting;

    std::vector<int> attackFrames;
    int numAttackFrames;
    int frameWidthAttack;
    int frameHeightAttack;
    size_t attackCurrentIndex;
    bool isAttacking;
    sf::Clock attackClock;
    float attackFrameTime;

    // Variables para bala
    float bulletSpeed;
    float bulletFrameTime;
    int bulletNumFrames;
    int bulletFrameWidth;
    int bulletFrameHeight;

    // Posición y trails
    sf::Vector2f position;
    std::vector<sf::Vector2f> runTrail;
    std::vector<int> runTrailFrame;
    std::vector<sf::Vector2f> reloadTrail;
    std::vector<int> reloadTrailFrame;
    std::vector<sf::Vector2f> attackTrail;
    std::vector<int> attackTrailFrame;
    size_t maxTrail;

    struct Projectile {
        sf::Vector2f position;
        int currentFrame;
        sf::Clock clock;
    };
    std::vector<Projectile> projectiles;

    float velocidad; // pixels per second
};

#endif // SPRITEJUGADOR_HPP