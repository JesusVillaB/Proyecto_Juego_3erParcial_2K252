#include "../include/Button.hpp"
#include <iostream>

Button::Button() {
    // Constructor
}

Button::~Button() {
    // Destructor
}

bool Button::loadTexture(const std::string& filePath) {
    if (!texture.loadFromFile(filePath)) {
        std::cerr << "Error loading button texture: " << filePath << std::endl;
        return false;
    }
    sprite.setTexture(texture);
    return true;
}

void Button::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

void Button::setCallback(std::function<void()> callback) {
    onClick = callback;
}

void Button::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::FloatRect bounds = sprite.getGlobalBounds();
        if (bounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            if (onClick) {
                onClick();
            }
        }
    }
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}