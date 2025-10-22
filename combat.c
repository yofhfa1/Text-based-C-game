#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game_object.h"

extern void useItem(Game *game);
extern void addXp(Game *game, int xp);

void printCombatStatus(Game *game, Monster enemies[], int enemyCount);
int selectTarget(Monster enemies[], int enemyCount);
int selectAlly(Game *game);
void showMonsterStats(Monster *m, int index);
void viewStatsMenu(Game *game, Monster enemies[], int enemyCount);
void useSkill(Champion *c, Game *game, Monster enemies[], int enemyCount);

typedef struct {
    char name[50];
    enum EffectType effectType;
    enum EffectTarget effectTarget;
    int cooldown;
} SkillTemplate;

static const SkillTemplate skillTemplates[5][4] = {
    // WIZARD
    {
        {"Fireball", DAMAGE, AOE_ENEMY, 3},
        {"Lightning Bolt", DAMAGE, SINGLE_TARGET_ENEMY, 2},
        {"Heal", HEAL, SINGLE_TARGET_ALLY, 3},
        {"Arcane Shield", BUFF, AOE_ALLY, 5}
    },
    // KNIGHT
    {
        {"Power Strike", DAMAGE, SINGLE_TARGET_ENEMY, 2},
        {"Shield Wall", BUFF, AOE_ALLY, 4},
        {"Taunt", DEBUFF, AOE_ENEMY, 3},
        {"Shield Bash", DAMAGE, SINGLE_TARGET_ENEMY, 2}
    },
    // PALADIN
    {
        {"Holy Strike", DAMAGE, SINGLE_TARGET_ENEMY, 2},
        {"Divine Protection", HEAL, SINGLE_TARGET_ALLY, 3},
        {"Blessing", BUFF, AOE_ALLY, 4},
        {"Smite", DAMAGE, AOE_ENEMY, 3}
    },
    // ROGUE
    {
        {"Backstab", DAMAGE, SINGLE_TARGET_ENEMY, 3},
        {"Poison Dart", DAMAGE, SINGLE_TARGET_ENEMY, 2},
        {"Smoke Bomb", DEBUFF, AOE_ENEMY, 4},
        {"Agility Boost", BUFF, SINGLE_TARGET_ALLY, 3}
    },
    // ELF
    {
        {"Arrow Volley", DAMAGE, AOE_ENEMY, 2},
        {"Piercing Shot", DAMAGE, SINGLE_TARGET_ENEMY, 2},
        {"Nature's Blessing", HEAL, SINGLE_TARGET_ALLY, 3},
        {"Wind Rush", BUFF, AOE_ALLY, 4}
    }
};

void useSkill(Champion *c, Game *game, Monster enemies[], int enemyCount) {
    printf("\n=== Skills ===\n");
    for (int i = 0; i < 4; i++) {
        printf("[%d] %s - %s (%s) Value:%d CD:%d", 
               i + 1, 
               c->skills[i].name,
               effect_target_string[c->skills[i].effectTarget],
               effect_type_string[c->skills[i].effectType],
               c->skills[i].effectValue,
               c->skillCooldown[i]);
        if (c->skillCooldown[i] > 0) {
            printf(" *ON COOLDOWN*");
        }
        printf("\n");
    }
    printf("[0] Cancel\n");
    printf("Select skill: ");
    
    int index;
    if (scanf("%d", &index) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }
    
    if (index == 0) return;
    index--;
    
    if (index < 0 || index >= 4) {
        printf("Invalid skill!\n");
        return;
    }
    
    if (c->skillCooldown[index] > 0) {
        printf("Skill is on cooldown! (%d turns remaining)\n", c->skillCooldown[index]);
        return;
    }
    
    Skill skill = c->skills[index];
    const SkillTemplate *tmpl = &skillTemplates[c->class][index];
    c->skillCooldown[index] = tmpl->cooldown;
    
    switch (skill.effectTarget) {
        case SINGLE_TARGET_ENEMY: {
            int target = selectTarget(enemies, enemyCount);
            if (target < 0) return;
            
            if (skill.effectType == DAMAGE) {
                enemies[target].health -= skill.effectValue;
                if (enemies[target].health < 0) enemies[target].health = 0;
                printf("%s used %s on %s. (%d damage)\n", 
                       champion_string[c->class], skill.name,
                       enemies[target].name, skill.effectValue);
            } else if (skill.effectType == DEBUFF) {
                enemies[target].damage -= skill.effectValue;
                if (enemies[target].damage < 0) enemies[target].damage = 0;
                printf("%s used %s on %s. (-%d damage debuff)\n", 
                       champion_string[c->class], skill.name,
                       enemies[target].name, skill.effectValue);
            }
            break;
        }
        case AOE_ENEMY: {
            if (skill.effectType == DAMAGE) {
                for (int i = 0; i < enemyCount; i++) {
                    if (enemies[i].health > 0) {
                        enemies[i].health -= skill.effectValue;
                        if (enemies[i].health < 0) enemies[i].health = 0;
                    }
                }
                printf("%s used %s on all enemies. (%d damage each)\n", 
                       champion_string[c->class], skill.name, skill.effectValue);
            } else if (skill.effectType == DEBUFF) {
                for (int i = 0; i < enemyCount; i++) {
                    if (enemies[i].health > 0) {
                        enemies[i].damage -= skill.effectValue;
                        if (enemies[i].damage < 0) enemies[i].damage = 0;
                    }
                }
                printf("%s used %s on all enemies. (-%d damage debuff each)\n", 
                       champion_string[c->class], skill.name, skill.effectValue);
            }
            break;
        }
        case SINGLE_TARGET_ALLY: {
            int target = selectAlly(game);
            if (target < 0) return;
            
            if (skill.effectType == HEAL) {
                game->champion[target].health += skill.effectValue;
                if (game->champion[target].health > game->champion[target].maxHealth) {
                    game->champion[target].health = game->champion[target].maxHealth;
                }
                printf("%s used %s on Champion %d. (+%d HP)\n", 
                       champion_string[c->class], skill.name, 
                       target + 1, skill.effectValue);
            } else if (skill.effectType == BUFF) {
                game->champion[target].damage += skill.effectValue;
                printf("%s used %s on Champion %d. (+%d damage buff)\n", 
                       champion_string[c->class], skill.name, 
                       target + 1, skill.effectValue);
            }
            break;
        }
        case AOE_ALLY: {
            if (skill.effectType == HEAL) {
                for (int i = 0; i < 3; i++) {
                    if (game->champion[i].health > 0) {
                        game->champion[i].health += skill.effectValue;
                        if (game->champion[i].health > game->champion[i].maxHealth) {
                            game->champion[i].health = game->champion[i].maxHealth;
                        }
                    }
                }
                printf("%s used %s on all allies. (+%d HP each)\n", 
                       champion_string[c->class], skill.name, skill.effectValue);
            } else if (skill.effectType == BUFF) {
                for (int i = 0; i < 3; i++) {
                    if (game->champion[i].health > 0) {
                        game->champion[i].damage += skill.effectValue;
                    }
                }
                printf("%s used %s on all allies. (+%d damage buff each)\n", 
                       champion_string[c->class], skill.name, skill.effectValue);
            }
            break;
        }
    }
}

int selectAlly(Game *game) {
    printf("\n--- Select Ally ---\n");
    int aliveMap[3];
    int counter = 0;
    for (int i = 0; i < 3; i++) {
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
    
    LinkedList *monsterList = (LinkedList *)(game->mapMonsterList + game->level * sizeof(LinkedList));
    int totalMonsters = monsterList->size;
    
    if (totalMonsters == 0) {
        printf("No monsters to fight!\n");
        return;
    }
    
    int fightCount = game->config.maxCombatEnemies;
    if (fightCount > totalMonsters) fightCount = totalMonsters;
    
    Monster localEnemies[fightCount];
    int enemyCount = 0;
    int selected[totalMonsters];
    for (int i = 0; i < totalMonsters; i++) selected[i] = 0;
    
    srand(time(NULL));
    
    while (enemyCount < fightCount) {
        int randIndex = rand() % totalMonsters;
        
        if (selected[randIndex] == 0) {
            selected[randIndex] = 1;
            
            Node *current = monsterList->head;
            for (int i = 0; i < randIndex; i++) {
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
                
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 4; j++) {
                        if (game->champion[i].skillCooldown[j] > 0) {
                            game->champion[i].skillCooldown[j]--;
                        }
                    }
                }
                break;
            }
            case 2: {
                useSkill(&game->champion[championIndex], game, localEnemies, enemyCount);
                
                downedMonster = 0;
                for (int i = 0; i < enemyCount; i++) {
                    if (localEnemies[i].health <= 0) downedMonster++;
                }
                
                if (downedMonster >= enemyCount) {
                    printf("\n=== VICTORY ===\n");
                    printf("All monsters defeated!\n");
                    printf("Experience gained: %d\n", totalExp);
                    addXp(game, totalExp);
                    return;
                }
                
                championIndex++;
                
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 4; j++) {
                        if (game->champion[i].skillCooldown[j] > 0) {
                            game->champion[i].skillCooldown[j]--;
                        }
                    }
                }
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
        for (int i = 0; i < 3; i++) {
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
        for (int i = 0; i < enemyCount; i++) {
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
            for (int i = 0; i < 3; i++) {
                Champion c = game->champion[i];
                printf("Champion %d - %s: HP %d/%d | Damage: %d%s\n",
                       i + 1, champion_string[c.class],
                       c.health, c.maxHealth, c.damage,
                       c.health <= 0 ? " [DEFEATED]" : "");
            }
        } else if (choice == 2) {
            printf("\nSelect Monster:\n");
            int aliveMap[enemyCount];
            int counter = 0;
            for (int i = 0; i < enemyCount; i++) {
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
    for (int i = 0; i < 3; i++) {
        if (game->champion[i].health > 0) {
            printf("  [%d] %s: HP %d/%d\n", i + 1, 
                   champion_string[game->champion[i].class],
                   game->champion[i].health, game->champion[i].maxHealth);
        } else {
            printf("  [%d] DEFEATED\n", i + 1);
        }
    }
    
    printf("Monsters:\n");
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i].health > 0) {
            printf("  %s: %d/%d HP\n", enemies[i].name, 
                   enemies[i].health, enemies[i].maxHealth);
        } else {
            printf("  %s: DEFEATED\n", enemies[i].name);
        }
    }
    printf("=====================\n");
}