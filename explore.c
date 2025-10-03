#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game_data.h"
int getWeightedScenario(int weights[], int size) {
    int total = 0;
    for (int i = 0; i < size; i++) {
        total += weights[i];
    }
    int r = rand() % total;
    int cumulative = 0;
    for (int i = 0; i < size; i++) {
        cumulative += weights[i];
        if (r < cumulative) return i;
    }
    return size - 1;
}
int explorer(Game* game) {
    printf("\n=== EXPLORATION ===\n");
    printf("Exploring the area...\n");
    int steps = game->config->exploration_steps;
    int* weights = game->config->scenario_weights;
    for (int i = 1; i <= steps; i++) {
        printf("\n--- Exploration Step %d/%d ---\n", i, steps);
        int scenario = getWeightedScenario(weights, 3);
        switch (scenario) {
            case 0:
                printf("*** COMBAT ENCOUNTER ***\n");
                createcombat(game);
                break;
            case 1:
                printf("*** MERCHANT ENCOUNTER ***\n");
                openShop(game);
                break;
            case 2:
                printf("*** SIDE EVENT ***\n");
                handleSideEvent(game);
                break;
        }
        if (i < steps) {
            printf("\nPress Enter to continue exploring...");
            getchar();
        }
    }
    printf("\nExploration complete! Returning to main menu.\n");
    printf("Press Enter...");
    getchar();
    return 0;
}
