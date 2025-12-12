#ifndef MENUELEMENT_HPP
#define MENUELEMENT_HPP

#include <SFML/Graphics.hpp>
#include <string>

class MenuElement {
public:
    MenuElement();
    ~MenuElement();

    bool loadTexture(const std::string& filePath);
    void setPosition(float x, float y);
    void setOrigin(float x, float y);
    sf::Vector2u getSize() const;
    void draw(sf::RenderWindow& window);

private:
    sf::Sprite sprite;
    sf::Texture texture;
};

#endif // MENUELEMENT_HPP