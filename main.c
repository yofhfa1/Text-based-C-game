#include <stdio.h>
#include "fileio.h"
#include "game.h"
#include "explore.h"
#include "combat.h"
#include "game_object.h"
#include "character.h"
#include "movement.h"
#include "cJSON.h"

void showMainMenu(Game * game);

int main() {
    Game game;
    // game.map = malloc(sizeof(int) * game.mapSize * game.mapSize);
    // game.locationData = malloc(sizeof(LocationData) * game.mapSize);
    // game.initialized = 0;
    showMainMenu(&game);
}

void showMainMenu(Game * game) {
    printf("Welcome to our game. This is the main menu.");
    int choice;
    while (1) {
        // Game is already running, add new option
        if (game->initialized) {
            printf("1. Continue");
            printf("2. Save game");
            printf("3. View stats");
            printf("4. New game");
            printf("5. Load game");
            printf("6. Exit");
            choice = scanf("%d", &choice);
            getchar();

            switch (choice)
            {
                case 1:
                    doGameTick(game);
                    break;
                case 2:
                    //This is manual save so autosave = 0
                    saveGame(game);
                    break;
                case 3:
                    // viewStats(game);
                    printAndCountFormation(game);
                    break;
                case 4:
                    initGame(game);
                    doGameTick(game);
                    break;
                case 5:
                    loadGame(game);
                    doGameTick(game);
                    break;
                case 6:
                    return;
                default:
                    printf("Please input a number in range of 1-6");
            }
        } else {
            printf("1. New game");
            printf("2. Load game");
            printf("3. Exit");
            choice = scanf("%d", &choice);
            getchar();

            switch (choice)
            {
                case 1:
                    initGame(game);
                    doGameTick(game);
                    break;
                case 2:
                    loadGame(game);
                    doGameTick(game);
                    break;
                case 3:
                    return;
                default:
                    printf("Please input a number in range of 1-3");
            }
        }

    }
}