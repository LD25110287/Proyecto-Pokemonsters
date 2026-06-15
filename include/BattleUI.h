#ifndef BATTLEUI_H
#define BATTLEUI_H

#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <string>
#include "Pokemonster.h"

class BattleUI
{
public:
    BattleUI(const sf::Vector2u& windowSize);

    void setPlayerPokemon(Pokemonster* p);
    void setEnemyPokemon(Pokemonster* e);

    // ── Equipos: necesarios para el panel de cambio ───────────────────────────
    // teamIndices: los 3 índices de personaje. activeSlot: cuál está en campo.
    void setP1Team(const std::array<int,3>& team, int activeSlot);
    void setP2Team(const std::array<int,3>& team, int activeSlot);

    // Carga íconos de cambio (llámalo después de setP1Team/setP2Team)
    void loadSwitchIcons();

    void update();
    void draw(sf::RenderTarget& target);

    // Retorna índice de movimiento clickeado (0-3), -1 si nada
    int  handleMouseClick(sf::Vector2i mousePos, bool isPlayer1Turn);

    // Retorna slot de cambio clickeado (0-2) cuando el panel está abierto, -1 si nada
    // Cierra el panel si se hace click fuera de él.
    int  handleSwitchClick(sf::Vector2i mousePos);

    // Actualiza el slot activo después de un cambio real en combate
    void updateActiveSlot(bool isP1, int newSlot);

    // ¿Está el panel de cambio abierto actualmente?
    bool isSwitchPanelOpen() const { return switchPanelOpen_; }

    void updateMoveButtons(Pokemonster* pokemon, bool isPlayer1);
    void setActivePlayer(bool isPlayer1Turn);

    void loadAttributeIcons();
    void setPlayerAttribute(Attribute a) { p1Attribute = a; }
    void setEnemyAttribute (Attribute a) { p2Attribute = a; }

    // Nombres de personajes (para el panel de cambio)
    static const char* getCharName(int index);

private:
    void buildEnergyBar(std::vector<sf::RectangleShape>& cells,
                        float x, float y, sf::Color activeColor);
    void refreshButtons();
    void buildSwitchPanel();   // reconstruye el panel según el jugador activo

    sf::Vector2u winSize;
    Pokemonster* player;
    Pokemonster* enemy;
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

    sf::Text turnLabel;
    sf::Font font;

    static const sf::Color P1_BTN_ACTIVE;
    static const sf::Color P1_BTN_DISABLED;
    static const sf::Color P2_BTN_ACTIVE;
    static const sf::Color P2_BTN_DISABLED;
    static const sf::Color BTN_NO_ENERGY;

    // ── Íconos de atributo ────────────────────────────────────────────────────
    sf::Texture attrTexVa, attrTexVi, attrTexDa;
    sf::Sprite  p1AttrSprite, p2AttrSprite;
    Attribute   p1Attribute, p2Attribute;

    // ── Equipos ───────────────────────────────────────────────────────────────
    std::array<int,3> p1Team_;
    std::array<int,3> p2Team_;
    int               p1ActiveSlot_;
    int               p2ActiveSlot_;

    // ── Ícono de cambio de personaje ──────────────────────────────────────────
    sf::Texture switchTexP1_;      // cambio_P1.png (azul)
    sf::Texture switchTexP2_;      // cambio_P2.png (rojo)
    sf::Sprite  switchIcon_;       // el que se muestra (cambia según turno)
    bool        switchIconLoaded_;
    sf::FloatRect switchIconBounds_; // para hit-testing

    // ── Panel de selección de cambio ─────────────────────────────────────────
    bool switchPanelOpen_;

    // Fondo semitransparente del panel
    sf::RectangleShape switchPanelBg_;

    // Título "Selecciona personaje"
    sf::Text switchPanelTitle_;

    // 3 slots: fondo + texto nombre + indicador "EN CAMPO"
    std::array<sf::RectangleShape, 3> switchSlotBg_;
    std::array<sf::Text,           3> switchSlotName_;
    std::array<sf::Text,           3> switchSlotTag_;   // "EN CAMPO" o vacío

    // Portraits en el panel (texturas cargadas on-demand)
    std::array<sf::Texture, 6> portraitTextures_;
    std::array<bool,        6> portraitLoaded_;
    std::array<sf::Sprite,  3> switchSlotPortrait_;

    // Texturas de portraits por personaje
    static const char* PORTRAIT_PATHS[6];
};

#endif // BATTLEUI_H
