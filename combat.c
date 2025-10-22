#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game_object.h"
void printCombatStatus(Game *game, Monster enemies[], int enemyCount);
int selectTarget(Monster enemies[], int enemyCount);
int selectAlly(Game *game);
void showMonsterStats(Monster *m, int index);
void viewStatsMenu(Game *game, Monster enemies[], int enemyCount);
void useSkill(Champion *c, Game *game, Monster enemies[], int enemyCount);
void initCombat(Game *game);
void createCombat(Game *game);
void useItem(Game *game) {
    printf("\n=== Use Item (stub) ===\n");
    printf("No items implemented. Returning to combat.\n");
}
static void sanitizeChampion(Champion *c) {
    if (c == NULL) return;
    if (c->maxHealth <= 0 || c->maxHealth > 1000000) c->maxHealth = 100;
    if (c->health < 0) c->health = 0;
    if (c->health > c->maxHealth) c->health = c->maxHealth;
    if (c->damage < -1000000 || c->damage > 1000000) c->damage = 10;
    if (c->level < 0 || c->level > 100) c->level = 0;
    if (c->xp < 0 || c->xp > 100000000) c->xp = 0;
}
static void sanitizeAllChampions(Game *game) {
    if (!game) return;
    for (int i = 0; i < 3; i++) sanitizeChampion(&game->champion[i]);
}
typedef enum { LST_SINGLE_ENEMY, LST_AOE_ENEMY, LST_SINGLE_ALLY, LST_AOE_ALLY } LocalTarget;
typedef enum { LTE_DAMAGE, LTE_DEBUFF, LTE_HEAL, LTE_BUFF } LocalType;
typedef struct {
    char name[40];
    LocalTarget target;
    LocalType type;
    int value;
    int cooldown;
} LocalSkill;
static LocalSkill localDefaultSkills[4] = {
    {"Strike", LST_SINGLE_ENEMY, LTE_DAMAGE, 15, 1},
    {"Cleave", LST_AOE_ENEMY, LTE_DAMAGE, 8, 2},
    {"Mend", LST_SINGLE_ALLY, LTE_HEAL, 20, 3},
    {"Inspire", LST_AOE_ALLY, LTE_BUFF, 5, 4}
};
static const char *local_effect_target_string[] = {
    "SINGLE_ENEMY",
    "AOE_ENEMY",
    "SINGLE_ALLY",
    "AOE_ALLY"
};
static const char *local_effect_type_string[] = {
    "DAMAGE",
    "DEBUFF",
    "HEAL",
    "BUFF"
};
static int localCooldowns[3][4] = {{0}};
static int championIndexOf(Game *game, Champion *c) {
    if (!game || !c) return -1;
    for (int i = 0; i < 3; i++) if (&game->champion[i] == c) return i;
    return -1;
}
static void decrementLocalCooldowns(void) {
    for (int ci = 0; ci < 3; ci++) {
        for (int si = 0; si < 4; si++) {
            if (localCooldowns[ci][si] > 0) localCooldowns[ci][si]--;
        }
    }
}
void useSkill(Champion *c, Game *game, Monster enemies[], int enemyCount) {
    printf("\n=== Skills ===\n");
    int ci = championIndexOf(game, c);
    if (ci < 0) ci = 0;
    for (int s = 0; s < 4; s++) {
        LocalSkill *ls = &localDefaultSkills[s];
        printf("[%d] %s - %s (%s) Value:%d CD:%d%s\n",
               s+1, ls->name, local_effect_target_string[ls->target],
               local_effect_type_string[ls->type], ls->value, localCooldowns[ci][s],
               localCooldowns[ci][s] > 0 ? " *ON COOLDOWN*" : "");
    }
    printf("[0] Cancel\nSelect skill: ");
    int choice;
    if (scanf("%d", &choice) != 1) { while (getchar()!='\n'); printf("Invalid input.\n"); return; }
    if (choice == 0) return;
    if (choice < 1 || choice > 4) { printf("Invalid skill\n"); return; }
    int sidx = choice - 1;
    if (localCooldowns[ci][sidx] > 0) { printf("Skill on cooldown (%d)\n", localCooldowns[ci][sidx]); return; }
    LocalSkill *ls = &localDefaultSkills[sidx];
    localCooldowns[ci][sidx] = ls->cooldown;
    switch (ls->target) {
        case LST_SINGLE_ENEMY: {
            int target = selectTarget(enemies, enemyCount);
            if (target < 0) return;
            if (ls->type == LTE_DAMAGE) {
                enemies[target].health -= ls->value;
                if (enemies[target].health < 0) enemies[target].health = 0;
                printf("%s used %s on %s (%d dmg)\n", champion_string[c->class], ls->name, enemies[target].name, ls->value);
            } else if (ls->type == LTE_DEBUFF) {
                enemies[target].damage -= ls->value;
                if (enemies[target].damage < 0) enemies[target].damage = 0;
                printf("%s used %s on %s (-%d dmg)\n", champion_string[c->class], ls->name, enemies[target].name, ls->value);
            }
            break;
        }
        case LST_AOE_ENEMY: {
            if (ls->type == LTE_DAMAGE) {
                for (int i=0;i<enemyCount;i++) if (enemies[i].health>0) { enemies[i].health -= ls->value; if (enemies[i].health<0) enemies[i].health=0; }
                printf("%s used %s on all enemies (%d each)\n", champion_string[c->class], ls->name, ls->value);
            } else if (ls->type == LTE_DEBUFF) {
                for (int i=0;i<enemyCount;i++) if (enemies[i].health>0) { enemies[i].damage -= ls->value; if (enemies[i].damage<0) enemies[i].damage=0; }
                printf("%s used %s on all enemies (-%d dmg)\n", champion_string[c->class], ls->name, ls->value);
            }
            break;
        }
        case LST_SINGLE_ALLY: {
            int target = selectAlly(game);
            if (target < 0) return;
            if (ls->type == LTE_HEAL) {
                game->champion[target].health += ls->value;
                if (game->champion[target].health > game->champion[target].maxHealth) game->champion[target].health = game->champion[target].maxHealth;
                printf("%s used %s on Champion %d (+%d HP)\n", champion_string[c->class], ls->name, target+1, ls->value);
            } else if (ls->type == LTE_BUFF) {
                game->champion[target].damage += ls->value;
                printf("%s used %s on Champion %d (+%d dmg)\n", champion_string[c->class], ls->name, target+1, ls->value);
            }
            break;
        }
        case LST_AOE_ALLY: {
            if (ls->type == LTE_HEAL) {
                for (int i=0;i<3;i++) if (game->champion[i].health>0) { game->champion[i].health += ls->value; if (game->champion[i].health>game->champion[i].maxHealth) game->champion[i].health=game->champion[i].maxHealth; }
                printf("%s used %s on all allies (+%d HP)\n", champion_string[c->class], ls->name, ls->value);
            } else if (ls->type == LTE_BUFF) {
                for (int i=0;i<3;i++) if (game->champion[i].health>0) { game->champion[i].damage += ls->value; }
                printf("%s used %s on all allies (+%d dmg)\n", champion_string[c->class], ls->name, ls->value);
            }
            break;
        }
    }
}
int selectAlly(Game *game) {
    printf("\n--- Select Ally ---\n");
    int aliveMap[3];
    int counter = 0;
    int i;
    for (i = 0; i < 3; i++) {
        if (game->champion[i].health > 0) {
            aliveMap[i] = 1;
            printf("[%d] Champion %d - %s (HP: %d/%d)\n",
                   ++counter, i + 1, champion_string[game->champion[i].class],
                   game->champion[i].health,
                   game->champion[i].maxHealth);
        } else {
            aliveMap[i] = 0;
        }
    }
    printf("[0] Cancel\n");
    printf("Select: ");
    int choice;
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
        return -1;
    }
    if (choice == 0) return -1;
    int target = findBinaryMapping(aliveMap, choice - 1, 3);
    if (target == -1) {
        printf("Invalid target!\n");
        return -1;
    }
    return target;
}
void initCombat(Game *game) {
    printf("\n=== COMBAT START ===\n");
    sanitizeAllChampions(game);
    LinkedList *monsterList = &((LocationData*)game->locationData)[game->level].monsterList;
    int totalMonsters = monsterList->size;
                decrementLocalCooldowns();
    int fightCount = 3;
    if (fightCount > totalMonsters) fightCount = totalMonsters;
    Monster localEnemies[fightCount];
    int enemyCount = 0;
    int selected[totalMonsters];
    int i;
    for (i = 0; i < totalMonsters; i++) selected[i] = 0;
    srand(time(NULL));
    while (enemyCount < fightCount) {
        int randIndex = rand() % totalMonsters;
        if (selected[randIndex] == 0) {
            selected[randIndex] = 1;
            Node *current = monsterList->head;
            int k;
            for (k = 0; k < randIndex; k++) {
                current = current->next;
            }
            Monster *m = (Monster *)current->value;
            localEnemies[enemyCount].health = m->health;
            localEnemies[enemyCount].maxHealth = m->maxHealth;
            localEnemies[enemyCount].damage = m->damage;
            strcpy(localEnemies[enemyCount].name, m->name);
            enemyCount++;
        }
    }
    printf("%d monsters appear!\n", enemyCount);
    int championIndex = 0;
    int monsterIndex = 0;
    int downedMonster = 0;
    int woundedChampions = 0;
    int totalExp = 0;
    while (1) {
        if (championIndex >= 3) championIndex = 0;
        
        while (game->champion[championIndex].health <= 0) {
            championIndex++;
            if (championIndex >= 3) championIndex = 0;
        }
        printf("\n=== Champion %d (%s)'s Turn (HP: %d/%d) ===\n", 
               championIndex + 1, champion_string[game->champion[championIndex].class],
               game->champion[championIndex].health, game->champion[championIndex].maxHealth);
        printf("[1] Attack\n");
        printf("[2] Use Skill\n");
        printf("[3] Use Item\n");
        printf("[4] View Stats\n");
        printf("Choose action: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Invalid input.\n");
            continue;
        }
        switch (choice) {
            case 1: {
                int target = selectTarget(localEnemies, enemyCount);
                if (target < 0) continue;
                localEnemies[target].health -= game->champion[championIndex].damage;
                if (localEnemies[target].health <= 0) {
                    localEnemies[target].health = 0;
                    downedMonster++;
                    totalExp += 50;
                }
                printf("Champion %d attacks %s for %d damage! (HP: %d/%d)\n",
                       championIndex + 1, localEnemies[target].name, 
                       game->champion[championIndex].damage, localEnemies[target].health, 
                       localEnemies[target].maxHealth);
                if (downedMonster >= enemyCount) {
                    printf("\n=== VICTORY ===\n");
                    printf("All monsters defeated!\n");
                    printf("Experience gained: %d\n", totalExp);
                    addXp(game, totalExp);
                    return;
                }
                championIndex++;
                break;
            }
            case 2: {
                useSkill(&game->champion[championIndex], game, localEnemies, enemyCount);
                downedMonster = 0;
                int k;
                for (k = 0; k < enemyCount; k++) {
                    if (localEnemies[k].health <= 0) downedMonster++;
                }
                if (downedMonster >= enemyCount) {
                    printf("\n=== VICTORY ===\n");
                    printf("All monsters defeated!\n");
                    printf("Experience gained: %d\n", totalExp);
                    addXp(game, totalExp);
                    return;
                }
                championIndex++;
                break;
            }
            case 3:
                useItem(game);
                break;
            case 4:
                viewStatsMenu(game, localEnemies, enemyCount);
                break;
            default:
                printf("Invalid choice.\n");
                break;
        }
        while (monsterIndex < enemyCount && localEnemies[monsterIndex].health <= 0) {
            monsterIndex++;
        }
        if (monsterIndex >= enemyCount) {
            monsterIndex = 0;
            while (monsterIndex < enemyCount && localEnemies[monsterIndex].health <= 0) {
                monsterIndex++;
            }
        }
        if (monsterIndex >= enemyCount) continue;
        int aliveChampions[3];
        int aliveCount = 0;
        int i;
        for (i = 0; i < 3; i++) {
            if (game->champion[i].health > 0) {
                aliveChampions[aliveCount++] = i;
            }
        }
        if (aliveCount == 0) {
            printf("\n=== DEFEAT ===\n");
            printf("All champions defeated!\n");
            game->initialized = 0;
            return;
        }
        int targetIdx = aliveChampions[rand() % aliveCount];
        game->champion[targetIdx].health -= localEnemies[monsterIndex].damage;
        if (game->champion[targetIdx].health <= 0) {
            game->champion[targetIdx].health = 0;
            woundedChampions++;
        }
        printf("\n%s attacks Champion %d for %d damage! (HP: %d/%d)\n",
               localEnemies[monsterIndex].name, targetIdx + 1, 
               localEnemies[monsterIndex].damage,
               game->champion[targetIdx].health, 
               game->champion[targetIdx].maxHealth);
        if (woundedChampions >= 3) {
            printf("\n=== DEFEAT ===\n");
            printf("All champions defeated!\n");
            game->initialized = 0;
            return;
        }
        monsterIndex++;
        if (monsterIndex >= enemyCount) monsterIndex = 0;
        printCombatStatus(game, localEnemies, enemyCount);
    }
}
int selectTarget(Monster enemies[], int enemyCount) {
    while (1) {
        printf("\n--- Select Target ---\n");
        int aliveMap[enemyCount];
        int counter = 0;
        int i;
        for (i = 0; i < enemyCount; i++) {
            if (enemies[i].health > 0) {
                aliveMap[i] = 1;
                printf("[%d] %s (HP: %d/%d)\n", 
                       ++counter, enemies[i].name, 
                       enemies[i].health, enemies[i].maxHealth);
            } else {
                aliveMap[i] = 0;
            }
        }
        printf("[0] Cancel\n");
        printf("Select: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Invalid input.\n");
            continue;
        }
        if (choice == 0) return -1;
        int target = findBinaryMapping(aliveMap, choice - 1, enemyCount);
        if (target == -1) {
            printf("Invalid target!\n");
            continue;
        }
        
        return target;
    }
}
void viewStatsMenu(Game *game, Monster enemies[], int enemyCount) {
    while (1) {
        sanitizeAllChampions(game);
        printf("\n=== VIEW STATS ===\n");
        printf("[1] View All Champions\n");
        printf("[2] View Monster\n");
        printf("[0] Back\n");
        printf("Choice: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }
        if (choice == 0) break;
        if (choice == 1) {
            printf("\n--- All Champions ---\n");
            int i;
            for (i = 0; i < 3; i++) {
                Champion c = game->champion[i];
                printf("Champion %d - %s: HP %d/%d | Damage: %d%s\n",
                       i + 1, champion_string[c.class],
                       c.health, c.maxHealth, c.damage,
                       c.health <= 0 ? " [DEFEATED]" : "");
            }
            FILE *dbg = fopen("formation_debug.txt", "a");
            if (dbg) {
                fprintf(dbg, "--- All Champions ---\n");
                for (int j = 0; j < 3; j++) {
                    Champion c = game->champion[j];
                    fprintf(dbg, "Champion %d - %s: HP %d/%d | Damage: %d%s\n",
                            j + 1, champion_string[c.class], c.health, c.maxHealth, c.damage,
                            c.health <= 0 ? " [DEFEATED]" : "");
                }
                fclose(dbg);
            }
        } else if (choice == 2) {
            printf("\nSelect Monster:\n");
            int aliveMap[enemyCount];
            int counter = 0; int i;
            for (i = 0; i < enemyCount; i++) {
                if (enemies[i].health > 0) {
                    aliveMap[i] = 1;
                    printf("[%d] %s\n", ++counter, enemies[i].name);
                } else {
                    aliveMap[i] = 0;
                }
            }
            printf("Choice: ");
            int monChoice;
            if (scanf("%d", &monChoice) != 1) {
                while (getchar() != '\n');
                continue;
            }
            int target = findBinaryMapping(aliveMap, monChoice - 1, enemyCount);
            if (target != -1) {
                showMonsterStats(&enemies[target], target + 1);
            }
        }
    }
}
void showMonsterStats(Monster *m, int index) {
    printf("\n--- Monster %d ---\n", index);
    printf("Name: %s\n", m->name);
    printf("HP: %d/%d\n", m->health, m->maxHealth);
    printf("Damage: %d\n", m->damage);
    printf("------------------\n");
}
void printCombatStatus(Game *game, Monster enemies[], int enemyCount) {
    printf("\n=== COMBAT STATUS ===\n");
    printf("Champions:\n");
    int i;
    for (i = 0; i < 3; i++) {
        if (game->champion[i].health > 0) {
            printf("  [%d] %s: HP %d/%d\n", i + 1, 
                   champion_string[game->champion[i].class],
                   game->champion[i].health, game->champion[i].maxHealth);
        } else {
            printf("  [%d] DEFEATED\n", i + 1);
        }
    }
    printf("Monsters:\n");
    int j;
    for (j = 0; j < enemyCount; j++) {
        if (enemies[i].health > 0) {
            printf("  %s: %d/%d HP\n", enemies[j].name, 
                   enemies[j].health, enemies[j].maxHealth);
        } else {
            printf("  %s: DEFEATED\n", enemies[j].name);
        }
    }
    printf("=====================\n");
}
void createCombat(Game *game) {
    initCombat(game);
}