#ifndef BATTLEUI_H
#define BATTLEUI_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Pokemonster;

class BattleUI
{
public:
    BattleUI(const sf::Vector2u& windowSize);

    void setPlayerPokemon(Pokemonster* p);
    void setEnemyPokemon(Pokemonster* e);

    void update();
    void draw(sf::RenderTarget& target);

    // Retorna índice del ataque (0-3) o -1 si no hubo selección
    int handleMouseClick(sf::Vector2i mousePos);

private:
    sf::Vector2u winSize;

    Pokemonster* player;
    Pokemonster* enemy;

    // HP bars
    sf::RectangleShape playerHpBack;
    sf::RectangleShape playerHpFront;
    sf::RectangleShape enemyHpBack;
    sf::RectangleShape enemyHpFront;

    // Move buttons
    std::vector<sf::RectangleShape> moveButtons;
    std::vector<sf::Text> moveTexts;

    sf::Font font;
};

#endif // BATTLEUI_H
