#include "../include/Menu.hpp"
#include <iostream>

Menu::Menu(sf::RenderWindow& win) : window(win), currentState(MAIN), musicOn(true), startGame(false) {
    // Load background
    if (!background.loadTexture("assets/Menu/Fondo_menu_2.png")) {
        std::cerr << "Failed to load menu background" << std::endl;
    }
    sf::Vector2u bgSize = background.getSize();
    background.setOrigin(bgSize.x / 2.0f, 0.0f); // Origin at top-center
    background.setPosition(400, -30); // Raised by 40 pixels

    // Load music
    if (!soundManager.loadMusic("assets/Efectos de sonido/goofy_theme.mp3")) {
        std::cerr << "Failed to load menu music" << std::endl;
    }
    soundManager.playMusic();

    // Music button
    if (!musicButton.loadTexture("assets/Menu/musicOn.png")) {
        std::cerr << "Failed to load musicOn texture" << std::endl;
    }
    musicButton.setPosition(10, 500); // Bottom left, raised a bit
    musicButton.setCallback([this]() {
        if (musicOn) {
            soundManager.pauseMusic();
            musicOn = false;
            musicButton.loadTexture("assets/Menu/musicOff.png");
        } else {
            soundManager.playMusic();
            musicOn = true;
            musicButton.loadTexture("assets/Menu/musicOn.png");
        }
    });

    // Start button
    if (!startButton.loadTexture("assets/Menu/right.png")) {
        std::cerr << "Failed to load right texture" << std::endl;
    }
    startButton.setPosition((800 - 64) / 2, 500); // Centered bottom, raised
    startButton.setCallback([this]() {
        soundManager.stopMusic();
        startGame = true;
    });

    // Info button
    if (!infoButton.loadTexture("assets/Menu/information.png")) {
        std::cerr << "Failed to load information texture" << std::endl;
    }
    infoButton.setPosition(800 - 64 - 10 - 20, 500); // Right side, moved left by 20
    infoButton.setCallback([this]() {
        currentState = INSTRUCTIONS;
    });

    // Exit button for instructions
    if (!exitButton.loadTexture("assets/Menu/exit.png")) {
        std::cerr << "Failed to load exit texture" << std::endl;
    }
    exitButton.setPosition(700, 500); // Moved left and up
    exitButton.setCallback([this]() {
        currentState = MAIN;
    });

    // Load font
    if (!font.loadFromFile("assets/Fuentes/Boxy-Bold.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
    }

    // Title text
    titleText.setFont(font);
    titleText.setString("Instrucciones de Juego - IRONCLAD");
    titleText.setCharacterSize(24);
    titleText.setFillColor(sf::Color::White);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.width / 2, 0);
    titleText.setPosition(400, 50);

    // Instructions text
    instructionsText.setFont(font);
    instructionsText.setString("Sobrevive al campo de batalla y alcanza la nave de salida para\n completar la mision.\n\n1. Elimina a todos los enemigos del nivel.\nSolo cuando el area este completamente despejada, la nave\n de extraccion aparecera y se habilitara el acceso para finalizar\n el juego.\n\n2. Manten tu barra de vida.\nSi tu barra de vida llega a cero, tu personaje sera destruido y el juego\nterminara en Game Over.\n\n3. Alcanza la nave de salida.\nTras derrotar a todos los enemigos, avanza hasta el final del mapa\n donde aparecera la nave de salida. Tomala para asegurar la victoria.");
    instructionsText.setCharacterSize(14);
    instructionsText.setFillColor(sf::Color::White);
    instructionsText.setPosition(50, 100);
}

Menu::~Menu() {
    soundManager.stopMusic();
}

bool Menu::run() {
    while (window.isOpen() && !startGame) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }
            if (currentState == MAIN) {
                musicButton.handleEvent(event, window);
                startButton.handleEvent(event, window);
                infoButton.handleEvent(event, window);
            } else if (currentState == INSTRUCTIONS) {
                exitButton.handleEvent(event, window);
            }
        }

        if (currentState == MAIN) {
            window.clear(sf::Color::Black);
        } else if (currentState == INSTRUCTIONS) {
            window.clear(sf::Color(173, 216, 230)); // Light blue
        }
        if (currentState == MAIN) {
            background.draw(window);
            musicButton.draw(window);
            startButton.draw(window);
            infoButton.draw(window);
        } else if (currentState == INSTRUCTIONS) {
            window.draw(titleText);
            window.draw(instructionsText);
            exitButton.draw(window);
        }
        window.display();
    }
    return startGame;
}