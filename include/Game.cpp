#include "Game.h"

Game::Game() : isRunning(true)
{
    window.create(sf::VideoMode({800, 600}), "Pokemon Game");
    window.setFramerateLimit(60);
}

Game::~Game()
{
    window.close();
}

void Game::run()
{
    while (window.isOpen() && isRunning)
    {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents()
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
            isRunning = false;
        }
        
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == sf::Keyboard::Key::Escape)
            {
                window.close();
                isRunning = false;
            }
        }
    }
}

void Game::update()
{
    // Lógica de actualización del juego
}

void Game::render()
{
    window.clear(sf::Color::Black);
    
    // Aquí se dibujarán los elementos del juego
    
    window.display();
}
