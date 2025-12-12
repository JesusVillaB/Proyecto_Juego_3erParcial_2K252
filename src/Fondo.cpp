#include "../include/Fondo.hpp"
#include <stdexcept>
#include <iostream>

Fondo::Fondo(sf::RenderWindow& window) {
    if (!backgroundTex.loadFromFile("assets/Fondo/night_background.png")) {
        throw std::runtime_error("Error: No se pudo cargar la textura de fondo desde 'assets/Fondo/night_background.png'");
    }
    if (backgroundTex.getSize().x == 0 || backgroundTex.getSize().y == 0) {
        throw std::runtime_error("Error: La textura de fondo tiene tamaño inválido.");
    }
    backgroundSprite.setTexture(backgroundTex);
    // Ensure initial position is at origin
    backgroundSprite.setPosition(0.f, 0.f);
    bgSize = backgroundTex.getSize();
    // Compute limitX as signed float to avoid unsigned underflow
    limitX = static_cast<float>(window.getSize().x) - static_cast<float>(bgSize.x);
    scrollSpeed = 100.0f;

    if (!foregroundTex.loadFromFile("assets/Fondo/rocky-nowater-close.png")) {
        throw std::runtime_error("Error: No se pudo cargar la textura del primer plano desde 'assets/Fondo/rocky-nowater-mid.png'");
    }
    if (foregroundTex.getSize().x == 0 || foregroundTex.getSize().y == 0) {
        throw std::runtime_error("Error: La textura del primer plano tiene tamaño inválido.");
    }
    // Do not repeat; use a static sprite scaled to window width
    foreground.setTexture(foregroundTex);
    float texW = static_cast<float>(foregroundTex.getSize().x);
    float texH = static_cast<float>(foregroundTex.getSize().y);
    float winW = static_cast<float>(window.getSize().x);
    float winH = static_cast<float>(window.getSize().y);
    float scale = winW / texW;
    foreground.setScale(scale, scale);
    // Compute a top collision and floor positions. The floor (ground/foreground) should be at the bottom
    // and the collision line for the top barrier should be in the top half.
    collisionY = winH * 0.5f; // top collision at 50% (middle) of window height
    floorY = winH; // floor is the bottom of the window
    float yPos = winH - texH * scale; // place the foreground texture anchored to the bottom
    // clamp so the sprite top is not above window top and bottom not below the window bottom
    if (yPos < 0.0f) yPos = 0.0f;
    float maxY = winH - texH * scale;
    if (yPos > maxY) yPos = maxY;
    foreground.setPosition(0.f, yPos);
}

void Fondo::update(float deltaTime, bool isMovingRight, bool isMovingLeft) {
    float currentScrollSpeed = 0.0f;
    if (isMovingRight && backgroundSprite.getPosition().x > limitX) {
        currentScrollSpeed = -scrollSpeed;
    } else if (isMovingLeft && backgroundSprite.getPosition().x < 0) {
        currentScrollSpeed = scrollSpeed;
    }

    backgroundSprite.move(currentScrollSpeed * deltaTime, 0);
    // foreground is static; do not move it with the background

    if (backgroundSprite.getPosition().x <= limitX) {
        backgroundSprite.setPosition(limitX, 0);
        // do not reposition foreground; keep static
    } else if (backgroundSprite.getPosition().x >= 0) {
        backgroundSprite.setPosition(0, 0);
        // keep the foreground static at the bottom
    }
}

void Fondo::drawBackground(sf::RenderWindow& window) {
    window.draw(backgroundSprite);
}

void Fondo::drawForeground(sf::RenderWindow& window) {
    sf::IntRect rect = foreground.getTextureRect();
    // Ensure color is normal and sprite is drawn; if texture not present, draw a red rect at bottom
    foreground.setColor(sf::Color::White);
    if (rect.width > 0 && rect.height > 0) {
        window.draw(foreground);
    } else {
        sf::RectangleShape fallback(sf::Vector2f(static_cast<float>(window.getSize().x), 50.f));
        fallback.setFillColor(sf::Color::Red);
        fallback.setPosition(0.f, static_cast<float>(window.getSize().y - 50));
        window.draw(fallback);
    }
}

float Fondo::getFloorY() const {
    return floorY;
}

float Fondo::getCollisionY() const {
    return collisionY;
}

Fondo::~Fondo() {
    std::cout << "Destructor Fondo called" << std::endl;
}