#ifndef BATTLEUI_H
#define BATTLEUI_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Pokemonster.h"  // necesario para Attribute

class BattleUI
{
public:
    BattleUI(const sf::Vector2u& windowSize);

    void setPlayerPokemon(Pokemonster* p);
    void setEnemyPokemon(Pokemonster* e);

    void update();
    void draw(sf::RenderTarget& target);

    // isPlayer1Turn: true = mostrar/detectar ataques de J1, false = J2
    int  handleMouseClick(sf::Vector2i mousePos, bool isPlayer1Turn);
    void updateMoveButtons(Pokemonster* pokemon, bool isPlayer1);

    // Cambia el panel al jugador activo (actualiza colores y textos)
    void setActivePlayer(bool isPlayer1Turn);

    // Carga los íconos de atributo (llamado desde Game)
    void loadAttributeIcons();
    // Asigna el atributo de cada jugador para mostrarlo en HUD
    void setPlayerAttribute(Attribute a) { p1Attribute = a; }
    void setEnemyAttribute (Attribute a) { p2Attribute = a; }

private:
    void buildEnergyBar(std::vector<sf::RectangleShape>& cells,
                        float x, float y, sf::Color activeColor);
    void refreshButtons();   // recarga textos + colores del panel según activeIsP1

    sf::Vector2u winSize;
    Pokemonster* player;   // Jugador 1
    Pokemonster* enemy;    // Jugador 2
    bool         activeIsP1;

    // ── Barras de HP ──────────────────────────────────────────────────────────
    sf::RectangleShape playerHpBack,  playerHpFront;
    sf::RectangleShape enemyHpBack,   enemyHpFront;

    // ── Barras de energía ─────────────────────────────────────────────────────
    std::vector<sf::RectangleShape> playerEnergyCells;
    std::vector<sf::RectangleShape> enemyEnergyCells;

    // ── Panel único de botones (abajo-derecha) ────────────────────────────────
    std::vector<sf::RectangleShape> moveButtons;
    std::vector<sf::Text>           moveTexts;
    std::vector<sf::Text>           moveCostTexts;

    // Etiqueta encima de los botones: "TURNO: Jugador 1" / "TURNO: Jugador 2"
    sf::Text turnLabel;

    sf::Font font;

    // Colores por jugador
    static const sf::Color P1_BTN_ACTIVE;
    static const sf::Color P1_BTN_DISABLED;
    static const sf::Color P2_BTN_ACTIVE;
    static const sf::Color P2_BTN_DISABLED;
    static const sf::Color BTN_NO_ENERGY;

    // ── Íconos de atributo ────────────────────────────────────────────────────
    sf::Texture attrTexVa, attrTexVi, attrTexDa;
    sf::Sprite  p1AttrSprite, p2AttrSprite;
    Attribute   p1Attribute, p2Attribute;
};

#endif // BATTLEUI_H
