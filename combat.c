#include <stdio.h>
#include <stdlib.h>
#include "game_object.h"
typedef struct {
    int health;
    int maxHealth;
    int damage;
    char name[50];
} Enemy;
void createcombat(Game* game) {
    printf("\n=== COMBAT START ===\n");
    int location = game->level;
    int numEnemies = game->config.baseEnemyCount + (location * game->config.enemyScalePerLevel);
    if (numEnemies > 4) numEnemies = 4;
    Enemy enemies[4];
    for (int i = 0; i < numEnemies; i++) {
        int baseHealth = game->config.enemyBaseHealth;
        int baseDamage = game->config.enemyBaseDamage;
        float locationMultiplier = 1.0f + (game->config.difficultyMultiplier * location);
        
        enemies[i].maxHealth = baseHealth * locationMultiplier;
        enemies[i].health = enemies[i].maxHealth;
        enemies[i].damage = baseDamage * locationMultiplier;
        sprintf(enemies[i].name, "Enemy %d", i + 1);
    }
    printf("Facing %d enemies at level %d!\n", numEnemies, location);
    int combatActive = 1;
    while (combatActive) { //ally turn
        for (int i = 0; i < 3; i++) {
            if (game->champion[i].health > 0) {
                handlePlayerTurn(game, enemies, numEnemies, i);
                
                if (checkAllEnemiesDead(enemies, numEnemies)) {
                    printf("\nVictory!\n");
                    combatActive = 0;
                    break;
                }
            }
        }
        if (!combatActive) break;
        for (int i = 0; i < numEnemies; i++) {//enemies turn
            if (enemies[i].health > 0) {
                handleEnemyTurn(game, &enemies[i]);
                if (checkAllChampionsDead(game)) {
                    printf("\nDefeat!\n");
                    combatActive = 0;
                    break;
                }
            }
        }
    }
}
void handlePlayerTurn(Game* game, Enemy enemies[], int numEnemies, int championIndex) {
    Champion* c = &game->champion[championIndex];
    int choice;
    while (1) {
        printf("\n--- Champion %d's Turn (HP: %d/%d) ---\n", 
               championIndex + 1, c->health, c->maxHealth);
        printf("1. Attack\n");
        printf("2. Skill\n");
        printf("3. Items\n");
        printf("4. Run away\n");
        printf("Choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: {
                int target = selectEnemyTarget(enemies, numEnemies);
                if (target >= 0) {
                    enemies[target].health -= c->damage;
                    printf("Dealt %d damage to %s!\n", c->damage, enemies[target].name);
                    
                    if (enemies[target].health <= 0) {
                        enemies[target].health = 0;
                        printf("%s defeated!\n", enemies[target].name);
                    }
                }
                return;
            }
            case 2:
                printf("Skills not implemented yet.\n");
                break;
            case 3:
                printf("Items not implemented yet.\n");
                break;
            case 4:
                if (attemptRunAway(game)) { //escape combat code
                    printf("Successfully escaped!\n");
                } else {
                    printf("Failed to escape!\n");
                }
                return;
            default:
                printf("Invalid choice. Try again.\n");
                break;
        }
    }
}
int selectEnemyTarget(Enemy enemies[], int numEnemies) {
    printf("Select target:\n");
    for (int i = 0; i < numEnemies; i++) {
        if (enemies[i].health > 0) {
            printf("%d. %s (HP: %d/%d)\n", 
                   i + 1, enemies[i].name, enemies[i].health, enemies[i].maxHealth);
        }
    }
    int target;
    scanf("%d", &target);
    target--;
    if (target >= 0 && target < numEnemies && enemies[target].health > 0) {
        return target;
    }
    return -1;
}
void handleEnemyTurn(Game* game, Enemy* enemy) {//enemies attack won't be scripted but randomly attack ally
    int target = rand() % 3;
    while (game->champion[target].health <= 0) {
        target = (target + 1) % 3;
    }
    game->champion[target].health -= enemy->damage;//loop the attack til target an alive one instead of attack the corpse, also show damage taken and survive or not
    printf("%s attacks Champion %d for %d damage!\n", 
           enemy->name, target + 1, enemy->damage);
    if (game->champion[target].health <= 0) {
        game->champion[target].health = 0;
        printf("Champion %d has fallen!\n", target + 1);
    }
}
int attemptRunAway(Game* game) {
    int totalHP = 0;
    for (int i = 0; i < 3; i++) {
        totalHP += game->champion[i].health;
    }
    
    int runChance = rand() % 2;
    return (runChance == 1 || totalHP > 1);
}
int checkAllEnemiesDead(Enemy enemies[], int numEnemies) {
    for (int i = 0; i < numEnemies; i++) {
        if (enemies[i].health > 0) return 0;
    }
    return 1;
}
int checkAllChampionsDead(Game* game) {
    for (int i = 0; i < 3; i++) {
        if (game->champion[i].health > 0) return 0;
    }
    return 1;
}