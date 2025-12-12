#include "../include/SoundManager.hpp"
#include <iostream>

SoundManager::SoundManager() {
    // Constructor
}

SoundManager::~SoundManager() {
    stopMusic();
}

bool SoundManager::loadMusic(const std::string& filePath) {
    if (!music.openFromFile(filePath)) {
        std::cerr << "Error loading music: " << filePath << std::endl;
        return false;
    }
    music.setLoop(true); // Loop the music
    return true;
}

void SoundManager::playMusic() {
    music.play();
}

void SoundManager::pauseMusic() {
    music.pause();
}

void SoundManager::stopMusic() {
    music.stop();
}

bool SoundManager::isPlaying() const {
    return music.getStatus() == sf::Music::Playing;
}