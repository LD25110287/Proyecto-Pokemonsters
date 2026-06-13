#include "../include/Game.h"
#include "../include/MenuScreen.h"
#include "../include/CharacterSelect.h"

int main()
{
    // Bucle principal: permite regresar al menú después de una partida
    while (true)
    {
        // ── 1. Menú principal ─────────────────────────────────────────────────
        MenuScreen menu;
        menu.run();

        if (!menu.shouldLaunchGame())
            return 0;   // El jugador eligió Salir

        // ── 2. Selección de personajes (3 por jugador) ────────────────────────
        CharacterSelect selector;
        selector.run();

        if (!selector.shouldLaunchBattle())
            continue;   // Volvió al menú sin jugar

        // ── 3. Serie de 3 rondas ──────────────────────────────────────────────
        const auto& p1Team = selector.getPlayer1Team();
        const auto& p2Team = selector.getPlayer2Team();
        int stage          = selector.getSelectedStage();

        int p1Wins = 0;
        int p2Wins = 0;

        for (int round = 0; round < 3; ++round)
        {
            // Si ya hay 2 victorias, la serie terminó antes
            if (p1Wins >= 2 || p2Wins >= 2)
                break;

            // Crear y correr la batalla de esta ronda
            Game game(p1Team[round], p2Team[round], stage);
            game.run();

            // Registrar quién ganó esta ronda
            if (game.getWinner() == 1)
                p1Wins++;
            else if (game.getWinner() == 2)
                p2Wins++;

            // El escenario puede cambiar entre rondas o mantenerse
            // (actualmente se mantiene el mismo elegido)
        }

        // La serie termina → vuelve al while(true) y regresa al menú

    }   // fin while(true)

    return 0;
}
