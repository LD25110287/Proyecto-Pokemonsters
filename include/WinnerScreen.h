#ifndef WINNERSCREEN_H
#define WINNERSCREEN_H

#include <SFML/Graphics.hpp>
#include <array>
#include <string>

// ── WinnerScreen ──────────────────────────────────────────────────────────────
// Pantalla final de la serie: muestra GANADOR.png de fondo, "JUGADOR X" debajo
// del cartel y los 3 retratos del equipo ganador. Click en cualquier lugar
// regresa al menú principal (cierra la ventana).
class WinnerScreen
{
public:
    // winnerNumber: 1 o 2. winnerTeam: los 3 índices de personaje (0-5) del ganador.
    WinnerScreen(int winnerNumber, const std::array<int,3>& winnerTeam);

    void run();

private:
    void loadAssets();
    void render();

    sf::RenderWindow window;
    sf::Font         font;

    sf::Texture bgTexture;
    sf::Sprite  bgSprite;

    int                winnerNumber_;
    std::array<int,3>  winnerTeam_;

    sf::Texture portraitTextures_[3];
    sf::Sprite  portraitSprites_[3];
    bool        portraitLoaded_[3];
};

#endif // WINNERSCREEN_H
