#include <SFML/Graphics.hpp>
#include "../include/Game.hpp"
#include <iostream>

int main() {
    try {
        std::cout << "Iniciando el programa..." << std::endl;

        Game game;
        game.run();

        std::cout << "Programa terminado exitosamente." << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Excepción capturada: " << e.what() << std::endl;
        return -1;
    }
}