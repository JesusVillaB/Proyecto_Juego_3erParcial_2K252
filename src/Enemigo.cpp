#include "../include/Enemigo.hpp"
#include <iostream>

Enemigo::Enemigo(sf::RenderWindow& window) : frameTime(0.1f), currentFrame(0), numFrames(8), frameWidth(0), frameHeight(0), maxTrail(8) {
    if (!texture.loadFromFile("assets/Enemigos/Robot.png")) {
        std::cerr << "Error loading enemy texture" << std::endl;
        return;
    }

    frameWidth = texture.getSize().x / numFrames;
    frameHeight = texture.getSize().y;

    setTexture(texture);
    setPosition(sf::Vector2f(400, 300)); // Initial position
}

Enemigo::~Enemigo() {
    // Cleanup if needed
}

void Enemigo::patrullar() {
    // Simple patrol: move left and right
    static bool movingRight = true;
    sf::Vector2f pos = getPosition();
    if (movingRight) {
        pos.x += 1.0f;
        if (pos.x > 600) movingRight = false;
    } else {
        pos.x -= 1.0f;
        if (pos.x < 200) movingRight = true;
    }
    setPosition(pos);
}

void Enemigo::atacarJugador() {
    // Implement attack logic
}

void Enemigo::morir() {
    // Implement death logic
}

void Enemigo::update(float deltaTime) {
    // Update animation
    if (animationClock.getElapsedTime().asSeconds() >= frameTime) {
        currentFrame = (currentFrame + 1) % numFrames;
        animationClock.restart();

        // Add to trail
        trail.push_back(getPosition());
        trailFrame.push_back(currentFrame);
        if (trail.size() > maxTrail) {
            trail.erase(trail.begin());
            trailFrame.erase(trailFrame.begin());
        }
    }

    // Update patrol
    patrullar();
}

void Enemigo::draw(sf::RenderWindow& window) {
    // Draw trail
    for (size_t i = 0; i < trail.size(); ++i) {
        sf::Sprite trailSprite(texture);
        trailSprite.setPosition(trail[i]);
        trailSprite.setTextureRect(sf::IntRect(trailFrame[i] * frameWidth, 0, frameWidth, frameHeight));
        sf::Color color = sf::Color::White;
        color.a = static_cast<sf::Uint8>((i + 1) * 255 / trail.size());
        trailSprite.setColor(color);
        window.draw(trailSprite);
    }

    // Draw current frame
    setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
    renderizar(window);
}
