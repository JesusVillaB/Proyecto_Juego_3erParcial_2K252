#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include "../include/SoundManager.hpp"
#include "../include/Button.hpp"
#include "../include/MenuElement.hpp"

class Menu {
public:
    Menu(sf::RenderWindow& window);
    ~Menu();

    bool run(); // Returns true if start game

private:
    enum State { MAIN, INSTRUCTIONS };

    sf::RenderWindow& window;
    SoundManager soundManager;
    MenuElement background;
    Button musicButton;
    Button startButton;
    Button infoButton;
    Button exitButton;
    sf::Font font;
    sf::Text titleText;
    sf::Text instructionsText;
    State currentState;
    bool musicOn;
    bool startGame;
};

#endif // MENU_HPP