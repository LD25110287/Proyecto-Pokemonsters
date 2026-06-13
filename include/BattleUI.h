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

    // Retorna índice del ataque (0-3) o -1 si no hubo click válido
    int handleMouseClick(sf::Vector2i mousePos);
    void updateMoveButtons(Pokemonster* pokemon);

private:
    void buildEnergyBar(std::vector<sf::RectangleShape>& cells,
                        float x, float y, sf::Color activeColor);

    sf::Vector2u winSize;
    Pokemonster* player;
    Pokemonster* enemy;

    // Barras de HP
    sf::RectangleShape playerHpBack,  playerHpFront;
    sf::RectangleShape enemyHpBack,   enemyHpFront;

    // Barras de energía (10 celdas cada una)
    std::vector<sf::RectangleShape> playerEnergyCells;
    std::vector<sf::RectangleShape> enemyEnergyCells;

    // Botones de movimiento
    std::vector<sf::RectangleShape> moveButtons;
    std::vector<sf::Text>           moveTexts;
    std::vector<sf::Text>           moveCostTexts;  // muestra el costo de energía

    sf::Font font;

    // Colores base para cada botón (por slot)
    static const sf::Color BTN_ACTIVE;
    static const sf::Color BTN_DISABLED;
};

#endif // BATTLEUI_H
