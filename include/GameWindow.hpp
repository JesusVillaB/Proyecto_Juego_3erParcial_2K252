#pragma once
#include <SFML/Graphics.hpp>
#include <string>
using namespace sf;
using namespace std;

class GameWindow : public sf::RenderWindow {
public:
    GameWindow(unsigned int width, unsigned int height, const sf::String& title = "")
        : sf::RenderWindow(sf::VideoMode(width, height), title) {
    }

    bool isOpen() {
        return window.isOpen();
    }

    void close() {
        window.close();
    }

    void clear() {
        window.clear();
    }

    void display() {
        window.display();
    }

    void draw(const Drawable& drawable) {
        window.draw(drawable);
    }

    bool pollEvent(Event& event) {
        return window.pollEvent(event);
    }

    Vector2u getSize() {
        return window.getSize();
    }

private:
    RenderWindow window;
};