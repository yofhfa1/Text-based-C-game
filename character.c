#include <stdio.h>
#include "game_object.h"
#include "character.h"

void editFormation(Game * game) {
    int choice;
    while (1) {
        int menuIndex = 0;
        int binaryArray[4] = {0, 0, 0, 0};

        printf("Formation editor menu.\n");
        int championAmount = printAndCountFormation(game);
        if (championAmount < 3) {
            printf("%d. Add new champion.\n", ++menuIndex);
            binaryArray[0] = 1;
        }
        if (championAmount > 0) {
            printf("%d. Remove champion.\n", ++menuIndex);
            printf("%d. Change champion.\n", ++menuIndex);
            printf("%d. Quit.\n", ++menuIndex);
            binaryArray[1] = 1;
            binaryArray[2] = 1;
            binaryArray[3] = 1;
        }
        printf("Your choice: ");
        scanf("%d", &choice);
        choice = findBinaryMapping(binaryArray, choice, 4);
        switch (choice) {
        case 0:
            addChampion(game, championAmount);
            continue;
        case 1:
            removeChampion(game, championAmount);
            continue;
        case 2:
            changeChampion(game, championAmount);
            continue;
        case 3:
            return;

        default:
            printf("Invalid choice\n");
            continue;
        }
    }
}

void addChampion(Game * game, int championAmount) {
    while (1) {
        printf("Select champion you want to add\n");
        for (int i = 0;i <= ELF;i++) {
            printf("%d. %s\n", (enum Class) i+1, champion_string[i]);
        }
        int choice;
        printf("Your choice: \n");
        scanf("%d", &choice);
        choice--;
        if (choice < 0 || choice > ELF) {
            printf("Invalid choice\n");
            continue;
        }
        Champion champion;
        champion.level = 0;
        champion.health = 100;
        champion.maxHealth = 100;
        champion.xp = 0;
        champion.damage = 10;
        champion.class = (enum Class) choice;
        game->champion[championAmount] = champion;
        break;
    }
}

void removeChampion(Game * game, int championAmount) {
    while (1) {
        printf("Select champion you want to remove\n");
        for (int i = 0;i < championAmount;i++) {
            printf("%d. %s lv: %d, xp: %d\n", i+1, champion_string[game->champion[i].class],
                game->champion[i].level, game->champion[i].xp);
        }
        printf("%d. Quit\n", championAmount+1);
        printf("Your choice: \n");
        int choice;
        scanf("%d", &choice);
        if (choice < 0 || choice > championAmount+1) {
            printf("Invalid choice!\n");
            continue;
        }
        if (choice == championAmount) return;
        choice--;
        game->champion[choice].maxHealth = 0;
        game->champion[choice].health = 0;
        // Defragment champion list
        if (choice < championAmount-1) {
            for (int i=choice;i < championAmount-1;i++) {
                game->champion[i] = game->champion[i+1];
                game->champion[i+1].maxHealth = 0;
                game->champion[i+1].health = 0;
            }
        }
        break;
    }
}

void changeChampion(Game * game, int championAmount) {
    while (1) {
        printf("Select champion you want to change\n");
        for (int i = 0;i < championAmount;i++) {
            printf("%d. %s lv: %d, xp: %d\n", i+1,
                champion_string[game->champion[i].class], game->champion[i].level,
                game->champion[i].xp);
        }
        printf("%d. Quit\n", championAmount+1);
        printf("Your choice: ");
        int choice;
        scanf("%d", &choice);
        choice -= 1;
        if (choice < 0 || choice > championAmount) {
            printf("Invalid choice!");
            continue;
        }
        if (choice == championAmount) return;
        printf("Select champion you want to replace %s lv: %d\n", 
            champion_string[game->champion[choice].class], game->champion[choice].level);
        for (int i = 0;i <= ELF;i++) {
            printf("%d. %s\n", (enum Class) i+1, champion_string[i]);
        }
        printf("%d. Quit\n", ELF+2);
        printf("Your choice: ");
        int choice1;
        scanf("%d", &choice1);
        choice1--;
        if (choice1 < 0 || choice1 > ELF+1) {
            printf("Invalid choice!\n");
            continue;
        }
        if (choice1 == ELF + 1) return;
        Champion champion;
        champion.level = 0;
        champion.xp = 0;
        champion.health = 100;
        champion.maxHealth = 100;
        champion.damage = 10;
        champion.class = (enum Class) choice1;
        game->champion[choice] = champion;
        break;
    }
}

void addXp(Game * game, int xp) {
    for (int i=0;i < 3;i++) {
        // Check if a champion is in this slot
        if (game->champion[i].maxHealth > 0) {
            game->champion[i].xp += xp;
            if (game->champion[i].xp > game->config.baseLvUpXp *
                game->champion[i].level * game->config.lvUpXpMul) {
                game->champion[i].xp -= game->config.baseLvUpXp *
                    game->champion[i].level * game->config.lvUpXpMul;
            }
        }
    }
}

int printAndCountFormation(Game * game) {
    int counter = 0;
    for (int i = 0;i < 3;i++) {
        Champion champion = game->champion[i];
        // Check if there is a champion
        if (champion.maxHealth > 0) {
            counter++;
            printf("Champion %d:\n", i+1);
            printf("\tLevel: %d\n", champion.level);
            printf("\tCurrent health: %d/%d\n", champion.health, champion.maxHealth);
            printf("\tDamage: %d\n", champion.damage);
            printf("\tXP: %d\n", champion.xp);
            printf("\tClass: %s\n", champion_string[champion.class]);
            printf("\n");
        }
    }
    return counter;
}