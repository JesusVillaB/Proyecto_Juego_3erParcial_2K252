#include "../include/MenuElement.hpp"
#include <iostream>

MenuElement::MenuElement() {
    // Constructor
}

MenuElement::~MenuElement() {
    // Destructor
}

bool MenuElement::loadTexture(const std::string& filePath) {
    if (!texture.loadFromFile(filePath)) {
        std::cerr << "Error loading menu element texture: " << filePath << std::endl;
        return false;
    }
    sprite.setTexture(texture);
    return true;
}

void MenuElement::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

void MenuElement::setOrigin(float x, float y) {
    sprite.setOrigin(x, y);
}

sf::Vector2u MenuElement::getSize() const {
    return texture.getSize();
}

void MenuElement::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}