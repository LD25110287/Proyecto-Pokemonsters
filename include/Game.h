#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>

class Game
{
public:
    Game();
    ~Game();
    void run();
    
private:
    void handleEvents();
    void update();
    void render();
    
    sf::RenderWindow window;
    bool isRunning;
};

#endif // GAME_H
