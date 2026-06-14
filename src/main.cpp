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
        // Índices actuales en el equipo de cada jugador
        int p1TeamIdx = 0;  // qué personaje del equipo J1 está en campo
        int p2TeamIdx = 0;  // qué personaje del equipo J2 está en campo

        int p1Wins = 0;
        int p2Wins = 0;
        int roundNumber = 1;

        // Sobrevivientes: nullptr = hay que crear uno nuevo
        Pokemonster* survivorP1 = nullptr;
        Pokemonster* survivorP2 = nullptr;

        AudioManager::playBattleMusic();

        // La serie dura hasta que alguien gane 2 rondas o se agoten los personajes.
        // Al ser un juego por turnos NO puede haber empate: cada ronda gana
        // exactamente J1 o J2, así que p1Wins+p2Wins sube 1 por ronda y la
        // serie nunca supera 3 rondas (roundNumber máximo = 3).
        while (p1Wins < 2 && p2Wins < 2 &&
               p1TeamIdx < 3 && p2TeamIdx < 3)
        {
            int p1CharIdx = p1Team[p1TeamIdx];
            int p2CharIdx = p2Team[p2TeamIdx];

            // Crear la ronda KoF
            Game game(p1CharIdx, p2CharIdx, stage,
                      survivorP1, survivorP2, roundNumber);
            game.run();

            int w = game.getWinner();

            if (w == 1)
            {
                // J1 ganó: su personaje sobrevive, J2 pasa al siguiente
                p1Wins++;
                survivorP1 = new Pokemonster(game.getPlayer()); // copia con vida actual
                survivorP2 = nullptr;
                p2TeamIdx++;
            }
            else // w == 2 (no puede haber empate en un juego por turnos)
            {
                // J2 ganó: su personaje sobrevive, J1 pasa al siguiente
                p2Wins++;
                survivorP2 = new Pokemonster(game.getEnemy());
                survivorP1 = nullptr;
                p1TeamIdx++;
            }

            roundNumber++;

            // Limpiar punteros dinámicos si ya no se usarán
            if (p1TeamIdx >= 3) { delete survivorP1; survivorP1 = nullptr; }
            if (p2TeamIdx >= 3) { delete survivorP2; survivorP2 = nullptr; }
        }

        // Limpiar al salir de la serie
        delete survivorP1;
        delete survivorP2;

        // Vuelve al menú para otra partida
    }

    return 0;
}
