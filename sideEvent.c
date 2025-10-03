#include <stdio.h>
#include <stdlib.h>
#include "game_data.h"
#include "combat.h" 
void handleSideEvent(Game* game) {
    int eventType = rand() % 3;
    switch (eventType) {
        case 0:
            handleTrapEvent(game);
            break;
        case 1:
            handleChestEvent(game);
            break;
        case 2: 
            handleMerchantQuest(game);
            break;
    }
}
void handleTrapEvent(Game* game) {
    printf("You trigger a hidden trap!\n");
    printf("1. Attempt to disarm (Rogue skill)\n");
    printf("2. Take the damage\n");
    printf("Choice: ");
    int trapChoice;
    scanf("%d", &trapChoice);
    if (trapChoice == 1) {
        int disarmChance = 50; 
        if (rand() % 100 < disarmChance) {
            printf("Successfully disarmed the trap!\n");
            printf("You gain 15 EXP for your skill.\n");
        } else {
            printf("Failed! The trap activates!\n");
            printf("Your party takes damage!\n");
        }
    } else {
        printf("You brace yourself and take the trap damage!\n");
        printf("Your party takes full damage!\n");
    }
}
void handleChestEvent(Game* game) {
    printf("You discover a locked treasure chest!\n");
    printf("1. Attempt lockpicking (Rogue skill)\n");
    printf("2. Force it open (damages contents)\n");
    printf("3. Leave it\n");
    printf("Choice: ");
    int chestChoice;
    scanf("%d", &chestChoice);
    switch (chestChoice) {
        case 1: {
            int lockpickChance = 60; 
            if (rand() % 100 < lockpickChance) {
                int goldFound = 30 + rand() % 40; 
                printf("Lockpicked successfully!\n");
                printf("You find %d gold inside!\n", goldFound);
                if (rand() % 100 < 20) {
                    printf("You also find a special item!\n");
                }
            } else {
                printf("Lockpicking failed.\n");
                printf("The lock jams. You can try forcing it or leave.\n");
            }
            break;
        }
        case 2: {
            int goldFound = (20 + rand() % 30) / 2;
            printf("You force the chest open!\n");
            printf("Some contents are damaged, but you recover %d gold.\n", goldFound);
            break;
        }
        case 3:
            printf("You decide to leave the chest alone.\n");
            printf("Perhaps another adventurer will have better luck...\n");
            break;

        default:
            printf("Invalid choice. Leaving the chest.\n");
            break;
    }
}
