#include <stdlib.h>
#include "game_object.h"

void explore(Game * game) {
    int i;
    for (i = 0; i < game->config.explorationTurn;i++) {
        printf("Exploring...");
        int chance = rand()%100;
        // Todo fix the config of exploring 
        if (chance > game->config.combatChance) {
            createCombat(game);
        } else if (chance > game->config.combatChance + game->config.tradeChance) {
            openShop(game);
        } else { //if (chance )
            createMinigame(game);
        }
    }
}