#ifndef FONDO_HPP
#define FONDO_HPP

#include <SFML/Graphics.hpp>

class Fondo {
public:
    Fondo(sf::RenderWindow& window);
    ~Fondo();
    void update(float deltaTime, bool isMovingRight, bool isMovingLeft);
    void drawBackground(sf::RenderWindow& window);
    void drawForeground(sf::RenderWindow& window);
    float getFloorY() const; // returns the floor Y coordinate where the player should stand
    float getCollisionY() const; // returns the top collision Y coordinate (upper boundary)

private:
    sf::Texture backgroundTex;
    sf::Sprite backgroundSprite;
    sf::Texture foregroundTex;
    sf::Sprite foreground;
    sf::Clock scrollClock;
    float scrollSpeed;
    float limitX;
    sf::Vector2u bgSize;
    float floorY;
    float collisionY; // top collision line (player cannot go above this y)
};

#endif // FONDO_HPP