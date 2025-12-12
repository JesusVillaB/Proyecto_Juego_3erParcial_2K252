#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <SFML/Audio.hpp>
#include <string>

class SoundManager {
public:
    SoundManager();
    ~SoundManager();

    bool loadMusic(const std::string& filePath);
    void playMusic();
    void pauseMusic();
    void stopMusic();
    bool isPlaying() const;

private:
    sf::Music music;
};

#endif // SOUNDMANAGER_HPP