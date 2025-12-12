#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SFML/Graphics.hpp>
#include <functional>

class Button {
public:
    Button();
    ~Button();

    bool loadTexture(const std::string& filePath);
    void setPosition(float x, float y);
    void setCallback(std::function<void()> callback);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);

private:
    sf::Sprite sprite;
    sf::Texture texture;
    std::function<void()> onClick;
};

#endif // BUTTON_HPP