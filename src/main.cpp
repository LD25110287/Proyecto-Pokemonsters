#include "../include/Game.h"
#include "../include/MenuScreen.h"
#include "../include/CharacterSelect.h"
#include "../include/AudioManager.h"

int main()
{
    AudioManager::playIntro();

    while (true)
    {
        // ── 1. Menú principal ─────────────────────────────────────────────────
        AudioManager::playMenuMusic();
        MenuScreen menu;
        menu.run();

        if (!menu.shouldLaunchGame())
            return 0;

        // ── 2. Selección de personajes ────────────────────────────────────────
        AudioManager::playMenuMusic();
        CharacterSelect selector;
        selector.run();

        if (!selector.shouldLaunchBattle())
            continue;

        const auto& p1Team = selector.getPlayer1Team();
        const auto& p2Team = selector.getPlayer2Team();
        int stage           = selector.getSelectedStage();

        // ── 3. Serie KoF ──────────────────────────────────────────────────────
        // p1TeamIdx/p2TeamIdx: qué slot (0-2) del equipo entra al campo
        // al INICIO de cada ronda, según el formato de eliminación KoF.
        int p1TeamIdx = 0;
        int p2TeamIdx = 0;

        int p1Wins = 0;
        int p2Wins = 0;
        int roundNumber = 1;

        Pokemonster* survivorP1 = nullptr;
        Pokemonster* survivorP2 = nullptr;

        AudioManager::playBattleMusic();

        while (p1Wins < 2 && p2Wins < 2 &&
               p1TeamIdx < 3 && p2TeamIdx < 3)
        {
            // Constructor con equipos COMPLETOS de 3, indicando qué slot
            // entra al campo en esta ronda. Esto permite que el panel
            // "Cambiar personaje" dentro de la batalla muestre a los
            // 3 integrantes reales del equipo (no el mismo x3).
            Game game(p1Team, p2Team, stage,
                      survivorP1, survivorP2, roundNumber,
                      p1TeamIdx, p2TeamIdx);
            game.run();

            int w = game.getWinner();

            if (w == 1)
            {
                // J1 ganó: su personaje sobrevive, J2 pasa al siguiente
                p1Wins++;
                survivorP1 = new Pokemonster(game.getPlayer());
                survivorP2 = nullptr;
                p2TeamIdx++;
            }
            else // w == 2 (no puede haber empate en un juego por turnos)
            {
                p2Wins++;
                survivorP2 = new Pokemonster(game.getEnemy());
                survivorP1 = nullptr;
                p1TeamIdx++;
            }

            roundNumber++;

            if (p1TeamIdx >= 3) { delete survivorP1; survivorP1 = nullptr; }
            if (p2TeamIdx >= 3) { delete survivorP2; survivorP2 = nullptr; }
        }

        delete survivorP1;
        delete survivorP2;

        // Vuelve al menú para otra partida
    }

    return 0;
}
