#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // Crear una ventana
    sf::RenderWindow window(sf::VideoMode(800, 600), "Fondo con Parallax");

    // Cargar texturas
    sf::Texture backgroundTex;
    if (!backgroundTex.loadFromFile("assets/Fondo/night_background.png")) {
        std::cerr << "Error: No se pudo cargar la textura de fondo." << std::endl;
        return -1;
    }

    sf::Texture foregroundTex;
    if (!foregroundTex.loadFromFile("assets/Fondo/rocky-nowater-close.png")) {
        std::cerr << "Error: No se pudo cargar la textura del primer plano." << std::endl;
        return -1;
    }

    // Configurar sprites
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTex);
    backgroundSprite.setPosition(0.f, 0.f);

    sf::Sprite foregroundSprite;
    foregroundSprite.setTexture(foregroundTex);
    float scale = static_cast<float>(window.getSize().x) / static_cast<float>(foregroundTex.getSize().x);
    foregroundSprite.setScale(scale, scale);
    float yPos = static_cast<float>(window.getSize().y) - static_cast<float>(foregroundTex.getSize().y) * scale;
    foregroundSprite.setPosition(0.f, yPos);

    // Variables para el parallax
    sf::Clock clock;
    float scrollSpeed = 100.0f;
    sf::Vector2u bgSize = backgroundTex.getSize();
    float limitX = static_cast<float>(window.getSize().x) - static_cast<float>(bgSize.x);

    // Bucle principal
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        // Detectar movimiento
        bool isMoving = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
                        sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down);

        // Actualizar parallax
        if (isMoving) {
            backgroundSprite.move(-scrollSpeed * deltaTime, 0);
            // keep foreground static in this demo

            if (backgroundSprite.getPosition().x < limitX) {
                backgroundSprite.setPosition(0, 0);
                foregroundSprite.setPosition(0.f, static_cast<float>(window.getSize().y - foregroundTex.getSize().y));
            }
        }

        // Dibujar
        window.clear();
        window.draw(backgroundSprite);
        // No visible collision marker — collision is invisible in runtime
        window.draw(foregroundSprite);
        window.display();
    }

    return 0;
}