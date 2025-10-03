#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game_data.h"
extern void createcombat(Game* game);
extern void openShop(Game* game);
extern void handleSideEvent(Game* game);
int explorer(Game* game) {
    printf("\n=== EXPLORATION ===\n");
    printf("Exploring the area...\n");
    for (int i = 1; i <= 3; i++) {
        printf("\n--- Exploration Step %d/3 ---\n", i);
        int scenario = rand() % 3;
        switch (scenario) {
            case 0:
                printf("*** COMBAT ENCOUNTER ***\n");
                printf("Enemies appear!\n");
                createcombat(game);
                break;
            case 1:
                printf("*** MERCHANT ENCOUNTER ***\n");
                printf("A traveling merchant approaches...\n");
                openShop(game);
                break;
            case 2:
                printf("*** SIDE EVENT ***\n");
                handleSideEvent(game);
                break;
        }
        if (i < 3) {
            printf("\nPress Enter to continue exploring...");
            getchar();
            getchar();
        }
    }
    printf("\nExploration complete! Returning to main menu.\n");
    printf("Press Enter...");
    getchar();
    getchar();
    return 0;
}
int getCurrentDanger() {
    return 2;
}

/* #include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game_data.h"
extern void createcombat(Game* game);
extern void openShop(Game* game);
int explorer(Game* game) {
    printf("\n=== EXPLORATION ===\n");
    printf("Exploring the area...\n");
    for (int i = 1; i <= 3; i++) {
        printf("\n--- Exploration Step %d/3 ---\n", i);
        int scenario = rand() % 3;
        switch (scenario) {
            case 0:
                printf("*** COMBAT ENCOUNTER ***\n");
                printf("Enemies appear!\n");
                createcombat(game);
                break;
            case 1:
                printf("*** MERCHANT ENCOUNTER ***\n");
                printf("A traveling merchant approaches...\n");
                openShop(game);
                break;
            case 2:
                printf("*** SIDE EVENT ***\n");
                handleSideEvent(game);
                break;
        }
        if (i < 3) {
            printf("Press Enter to continue exploring...");
            getchar();
            getchar();
        }
    }
    printf("\nExploration complete! Returning to main menu.\n");
    printf("Press Enter...");
    getchar();
    getchar();
    return 0;
}
void handleSideEvent(Game* game) {
    int eventType = rand() % 3;
    switch (eventType) {
        case 0:
            printf("You trigger a hidden trap!\n");
            printf("1. Attempt to disarm (Rogue skill)\n");
            printf("2. Take the damage\n");
            printf("Choice: ");
            int trapChoice;
            scanf("%d", &trapChoice);
            if (trapChoice == 1) {
                if (rand() % 100 < 50) {
                    printf("Successfully disarmed the trap!\n");
                } else {
                    printf("Failed! The trap activates!\n");
                }
            } else {
                printf("You take damage from the trap!\n");
            }
            break;
        case 1:
            printf("You discover a locked treasure chest!\n");
            printf("1. Attempt lockpicking (Rogue skill)\n");
            printf("2. Force it open\n");
            printf("3. Leave it\n");
            printf("Choice: ");
            int chestChoice;
            scanf("%d", &chestChoice);
            if (chestChoice == 1) {
                if (rand() % 100 < 60) {
                    printf("Lockpicked successfully! You find treasure inside.\n");
                } else {
                    printf("Lockpicking failed.\n");
                }
            } else if (chestChoice == 2) {
                printf("You force the chest open and find some gold.\n");
            } else {
                printf("You leave the chest alone.\n");
            }
            break; 
        case 2:
            printf("A merchant needs help!\n");
            printf("'Bandits stole my goods! Can you retrieve them?'\n");
            printf("1. Accept quest\n");
            printf("2. Decline\n");
            printf("Choice: ");
            int questChoice;
            scanf("%d", &questChoice);
            if (questChoice == 1) {
                printf("Quest accepted! Searching for the stolen goods...\n");
                if (rand() % 100 < 70) {
                    printf("You find the bandits guarding the goods!\n");
                    createcombat(game);
                    printf("Quest complete! The merchant rewards you.\n");
                } else {
                    printf("You find the goods abandoned. Quest complete!\n");
                }
            } else {
                printf("You decline the quest.\n");
            }
            break;
    }
}
int getCurrentDanger() {
    return 2;
} */