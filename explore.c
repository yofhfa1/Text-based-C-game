#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "explore.h"
#define MAX_LEVELS 10
#define MAX_AREAS_PER_LEVEL 3
#define MAX_NAME_LENGTH 50
#define MAX_DESC_LENGTH 200
#define MAX_MERCHANT_ITEMS 8

typedef struct {
    char name[MAX_NAME_LENGTH];
    char description[MAX_DESC_LENGTH];
    int level;
    int difficulty;
    int hasShrine;
    int shrineDiscovered;
    char shrineName[MAX_NAME_LENGTH];
} Area;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int price;
    int healAmount;
    int manaAmount;
    int stock;
    int itemType;
} MerchantItem;

typedef struct {
    int active;
    char description[MAX_DESC_LENGTH];
    char questZoneName[MAX_NAME_LENGTH];
    int completed;
    int rewardGold;
    char rewardItem[MAX_NAME_LENGTH];
} MerchantQuest;

typedef struct {
    Area areas[MAX_LEVELS][MAX_AREAS_PER_LEVEL];
    int currentLevel;
    int currentAreaIndex;
    int playerGold;
    int teleportationStones;
    int currentDay;
    int areasInitialized;
    MerchantQuest activeQuest;

    int shrineCount;
    int discoveredShrines[MAX_LEVELS * MAX_AREAS_PER_LEVEL][2];
} ExplorationState;

static ExplorationState exploreState;

static void initializeAreas();
static void displayCurrentArea();
static int exploreArea(GameState* gameState);
static int generateEncounter();
static int handleCombatEncounter(GameState* gameState);
static void handleTrapEncounter(GameState* gameState);
static void handleChestEncounter();
static void handleMerchantEncounter();
static void handleShrineEncounter();
static int handleMerchantQuest(GameState* gameState);
static int attemptLockpicking(int difficulty, GameState* gameState);
static int attemptTrapDisarm(int difficulty, GameState* gameState);
static void showTeleportOptions();
static int handleVillageMenu(GameState* gameState);
static int handleAreaTransition(GameState* gameState);

// Main exploration functions
void initExploration(GameState* gameState) {
    if (!exploreState.areasInitialized) {
        initializeAreas();
        exploreState.areasInitialized = 1;
    }
    
    exploreState.currentLevel = 1;
    exploreState.currentAreaIndex = -1;
    exploreState.playerGold = 500;
    exploreState.teleportationStones = 1;
    exploreState.currentDay = 1;
    exploreState.shrineCount = 0;
    exploreState.activeQuest.active = 0;
    
    printf("\n=== EXPLORATION BEGINS ===\n");
    printf("You stand at the village gates, ready to venture forth...\n");
    printf("Gold: %d | Teleportation Stones: %d | Day: %d\n", 
           exploreState.playerGold, exploreState.teleportationStones, exploreState.currentDay);
}

int runExploration(GameState* gameState) {
    if (exploreState.activeQuest.active) {
        return handleMerchantQuest(gameState);
    }
    
    if (exploreState.currentLevel == 1 && exploreState.currentAreaIndex == -1) {
        return handleVillageMenu(gameState);
    }
    
    return exploreArea(gameState);
}

static int handleVillageMenu(GameState* gameState) {
    clearScreen();
    printf("\n=== VILLAGE START ===\n");
    printf("Day: %d | Gold: %d | Teleportation Stones: %d\n",
           exploreState.currentDay, exploreState.playerGold, exploreState.teleportationStones);
    
    if (exploreState.currentLevel <= 4) {
        printf("\nChoose your path:\n");
        for (int i = 0; i < MAX_AREAS_PER_LEVEL; i++) {
            Area* area = &exploreState.areas[exploreState.currentLevel-1][i];
            printf("%d. %s [%s - Danger Level %d]\n", 
                   i+1, area->name, 
                   area->difficulty == 1 ? "Easy" :
                   area->difficulty == 2 ? "Medium" : "Hard",
                   area->difficulty);
        }
        printf("4. Visit fountain/shop\n");
        printf("5. View discovered shrines\n");
        printf("0. Return to main menu\n");
        
        int choice = getIntInput(0, 5);
        if (choice == 0) return 0;
        if (choice <= 3) {
            exploreState.currentAreaIndex = choice - 1;
            return runExploration(gameState);
        }
        if (choice == 4) {
            printf("Village services not yet implemented.\n");
            waitForEnter();
            return runExploration(gameState);
        }
        if (choice == 5) {
            showTeleportOptions();
            return runExploration(gameState);
        }
    } else if (exploreState.currentLevel == 5) {
        printf("\nAll paths lead to the Wizard Tower...\n");
        printf("1. Enter Wizard Tower [Semi-Boss - Danger Level 5]\n");
        printf("2. Visit fountain/shop\n");
        printf("3. View discovered shrines\n");
        printf("0. Return to main menu\n");
        
        int choice = getIntInput(0, 3);
        if (choice == 0) return 0;
        if (choice == 1) {
            exploreState.currentAreaIndex = 0;
            return runExploration(gameState);
        }
        if (choice == 2) {
            printf("Village services not yet implemented.\n");
            waitForEnter();
            return runExploration(gameState);
        }
        if (choice == 3) {
            showTeleportOptions();
            return runExploration(gameState);
        }
    } else {
        Area* nextArea = &exploreState.areas[exploreState.currentLevel-1][0];
        printf("\nNext destination: %s [Danger Level %d]\n", 
               nextArea->name, exploreState.currentLevel);
        printf("1. Continue journey\n");
        printf("2. Visit fountain/shop\n");
        printf("3. View discovered shrines\n");
        printf("0. Return to main menu\n");
        
        int choice = getIntInput(0, 3);
        if (choice == 0) return 0;
        if (choice == 1) {
            exploreState.currentAreaIndex = 0;
            return runExploration(gameState);
        }
        if (choice == 2) {
            printf("Village services not yet implemented.\n");
            waitForEnter();
            return runExploration(gameState);
        }
        if (choice == 3) {
            showTeleportOptions();
            return runExploration(gameState);
        }
    }
    
    return runExploration(gameState);
}

static int exploreArea(GameState* gameState) {
    displayCurrentArea();
    
    printf("\n--- EXPLORATION OPTIONS ---\n");
    printf("1. Continue exploring\n");
    printf("2. Use teleportation stone\n");
    printf("3. Return to village (ends day)\n");
    
    int choice = getIntInput(1, 3);
    
    switch (choice) {
        case 1: {
            int encounterType = generateEncounter();
            switch (encounterType) {
                case 0:
                    return handleCombatEncounter(gameState);
              case 1:
                    handleTrapEncounter(gameState);
                    break;
                case 2:
                    handleChestEncounter();
                    break;
                case 3:
                    handleMerchantEncounter();
                    break;
                case 4:
                    handleShrineEncounter();
                    break;
                default:
                    printf("You continue exploring without incident...\n");
                    printf("The path ahead remains quiet.\n");
                    waitForEnter();
                    break;
            }
            return runExploration(gameState);
        }
        
        case 2:
            showTeleportOptions();
            return runExploration(gameState);
            
        case 3:
            printf("You journey back to the village as night falls...\n");
            exploreState.currentLevel = 1;
            exploreState.currentAreaIndex = -1;
            exploreState.currentDay++;
            printf("Day %d begins...\n", exploreState.currentDay);
            waitForEnter();
            return runExploration(gameState);
    }
    
    return runExploration(gameState);
}

static void displayCurrentArea() {
    clearScreen();
    if (exploreState.currentLevel <= 4) {
        Area* area = &exploreState.areas[exploreState.currentLevel-1][exploreState.currentAreaIndex];
        printf("You teleport to %s!\n", area->shrineName);
    }
    
    waitForEnter();
}

static void initializeAreas() {
    // Level 2: Three starting paths (Easy/Medium/Hard)
    strcpy(exploreState.areas[1][0].name, "Peaceful Meadows");
    strcpy(exploreState.areas[1][0].description, "Rolling hills dotted with wildflowers and gentle streams.");
    exploreState.areas[1][0].level = 2;
    exploreState.areas[1][0].difficulty = 1; // Easy
    exploreState.areas[1][0].hasShrine = 1;
    strcpy(exploreState.areas[1][0].shrineName, "Flower Grove Shrine");
    
    strcpy(exploreState.areas[1][1].name, "Dark Woodlands");
    strcpy(exploreState.areas[1][1].description, "Dense forest with twisted trees and mysterious shadows.");
    exploreState.areas[1][1].level = 2;
    exploreState.areas[1][1].difficulty = 2; // Medium
    exploreState.areas[1][1].hasShrine = 1;
    strcpy(exploreState.areas[1][1].shrineName, "Ancient Oak Shrine");
    
    strcpy(exploreState.areas[1][2].name, "Jagged Cliffs");
    strcpy(exploreState.areas[1][2].description, "Treacherous rocky terrain with steep drops and howling winds.");
    exploreState.areas[1][2].level = 2;
    exploreState.areas[1][2].difficulty = 3; // Hard
    exploreState.areas[1][2].hasShrine = 1;
    strcpy(exploreState.areas[1][2].shrineName, "Eagle's Rest Shrine");
    
    // Level 3: Continuation paths
    strcpy(exploreState.areas[2][0].name, "Sunny Farmlands");
    strcpy(exploreState.areas[2][0].description, "Abandoned farms with overgrown fields and empty barns.");
    exploreState.areas[2][0].level = 3;
    exploreState.areas[2][0].difficulty = 1;
    exploreState.areas[2][0].hasShrine = 0;
    
    strcpy(exploreState.areas[2][1].name, "Misty Marshlands");
    strcpy(exploreState.areas[2][1].description, "Swampy terrain filled with fog and strange sounds.");
    exploreState.areas[2][1].level = 3;
    exploreState.areas[2][1].difficulty = 2;
    exploreState.areas[2][1].hasShrine = 1;
    strcpy(exploreState.areas[2][1].shrineName, "Will-o'-Wisp Shrine");
    
    strcpy(exploreState.areas[2][2].name, "Volcanic Slopes");
    strcpy(exploreState.areas[2][2].description, "Rocky slopes with lava flows and sulfurous air.");
    exploreState.areas[2][2].level = 3;
    exploreState.areas[2][2].difficulty = 3;
    exploreState.areas[2][2].hasShrine = 0;
    
    // Level 4: Final branching paths
    strcpy(exploreState.areas[3][0].name, "Crystal Gardens");
    strcpy(exploreState.areas[3][0].description, "Beautiful crystalline formations that sing in the wind.");
    exploreState.areas[3][0].level = 4;
    exploreState.areas[3][0].difficulty = 1;
    exploreState.areas[3][0].hasShrine = 1;
    strcpy(exploreState.areas[3][0].shrineName, "Resonance Crystal Shrine");
    
    strcpy(exploreState.areas[3][1].name, "Haunted Ruins");
    strcpy(exploreState.areas[3][1].description, "Ancient stone structures filled with ghostly whispers.");
    exploreState.areas[3][1].level = 4;
    exploreState.areas[3][1].difficulty = 2;
    exploreState.areas[3][1].hasShrine = 1;
    strcpy(exploreState.areas[3][1].shrineName, "Spectral Shrine");
    
    strcpy(exploreState.areas[3][2].name, "Demon Gorge");
    strcpy(exploreState.areas[3][2].description, "A hellish canyon where demons are said to dwell.");
    exploreState.areas[3][2].level = 4;
    exploreState.areas[3][2].difficulty = 3;
    exploreState.areas[3][2].hasShrine = 0;
    
    // Level 5: Wizard Tower (convergence point)
    strcpy(exploreState.areas[4][0].name, "Wizard Tower");
    strcpy(exploreState.areas[4][0].description, "A towering spire crackling with magical energy. The Wizard awaits within.");
    exploreState.areas[4][0].level = 5;
    exploreState.areas[4][0].difficulty = 5;
    exploreState.areas[4][0].hasShrine = 1;
    strcpy(exploreState.areas[4][0].shrineName, "Arcane Sanctum Shrine");
    
    // Level 6: Post-Wizard areas
    strcpy(exploreState.areas[5][0].name, "Blighted Wasteland");
    strcpy(exploreState.areas[5][0].description, "A desolate land corrupted by dark magic from the defeated wizard.");
    exploreState.areas[5][0].level = 6;
    exploreState.areas[5][0].difficulty = 6;
    exploreState.areas[5][0].hasShrine = 1;
    strcpy(exploreState.areas[5][0].shrineName, "Purification Shrine");
    
    // Level 7
    strcpy(exploreState.areas[6][0].name, "Shadow Valley");
    strcpy(exploreState.areas[6][0].description, "A valley perpetually shrouded in darkness and despair.");
    exploreState.areas[6][0].level = 7;
    exploreState.areas[6][0].difficulty = 7;
    exploreState.areas[6][0].hasShrine = 0;
    
    // Level 8
    strcpy(exploreState.areas[7][0].name, "Bone Fields");
    strcpy(exploreState.areas[7][0].description, "Ancient battlefields littered with the bones of fallen warriors.");
    exploreState.areas[7][0].level = 8;
    exploreState.areas[7][0].difficulty = 8;
    exploreState.areas[7][0].hasShrine = 1;
    strcpy(exploreState.areas[7][0].shrineName, "Memorial Shrine");
    
    // Level 9
    strcpy(exploreState.areas[8][0].name, "Dragon's Approach");
    strcpy(exploreState.areas[8][0].description, "The final path to the dragon's lair, marked by scorched earth and treasure.");
    exploreState.areas[8][0].level = 9;
    exploreState.areas[8][0].difficulty = 9;
    exploreState.areas[8][0].hasShrine = 0;
    
    // Level 10: Dragon's Lair (final boss)
    strcpy(exploreState.areas[9][0].name, "Dragon's Lair");
    strcpy(exploreState.areas[9][0].description, "The ancient dragon's domain, filled with treasure and unimaginable danger.");
    exploreState.areas[9][0].level = 10;
    exploreState.areas[9][0].difficulty = 10;
    exploreState.areas[9][0].hasShrine = 0; // No escape from final boss
}

// Interface functions for integration with main game

void updateExplorationAfterCombat(int victory, int goldEarned, int expEarned) {
    if (victory) {
        printf("\n=== VICTORY REWARDS ===\n");
        printf("Gold earned: %d\n", goldEarned);
        printf("Experience earned: %d\n", expEarned);
        
        exploreState.playerGold += goldEarned;
        
        // If in quest zone and combat won, complete quest
        if (exploreState.activeQuest.active && !exploreState.activeQuest.completed) {
            printf("Quest objective completed!\n");
            exploreState.activeQuest.completed = 1;
            
            printf("Returning to merchant for reward...\n");
            exploreState.playerGold += exploreState.activeQuest.rewardGold;
            printf("Merchant rewards you with %d gold and a %s!\n",
                   exploreState.activeQuest.rewardGold, exploreState.activeQuest.rewardItem);
            
            exploreState.activeQuest.active = 0;
        }
        
        // Small chance to find items after combat
        if (rand() % 100 < 15) {
            printf("You find something valuable among the remains!\n");
            if (rand() % 100 < 30) {
                printf("A teleportation stone!\n");
                exploreState.teleportationStones++;
            } else {
                int bonusGold = 10 + rand() % 20;
                printf("Extra gold: %d pieces!\n", bonusGold);
                exploreState.playerGold += bonusGold;
            }
        }
    } else {
        printf("\nDefeated! You retreat to safety...\n");
        exploreState.playerGold = (exploreState.playerGold * 3) / 4; // Lose 25% gold
        
        // If in quest zone and lost, return to merchant empty-handed
        if (exploreState.activeQuest.active) {
            printf("Quest failed. Returning to merchant...\n");
            exploreState.activeQuest.active = 0;
        }
    }
    
    waitForEnter();
}

int getCurrentLocationDanger() {
    if (exploreState.currentLevel <= 4) {
        return exploreState.areas[exploreState.currentLevel-1][exploreState.currentAreaIndex].difficulty;
    } else {
        return exploreState.currentLevel;
    }
}

int getPlayerGold() {
    return exploreState.playerGold;
}

void addPlayerGold(int amount) {
    exploreState.playerGold += amount;
    if (exploreState.playerGold < 0) exploreState.playerGold = 0;
}

int getTeleportationStones() {
    return exploreState.teleportationStones;
}

void addTeleportationStones(int amount) {
    exploreState.teleportationStones += amount;
    if (exploreState.teleportationStones < 0) exploreState.teleportationStones = 0;
}

int getCurrentDay() {
    return exploreState.currentDay;
}

// Debug function
void debugExplorationState() {
    printf("\n=== EXPLORATION DEBUG ===\n");
    printf("Current Level: %d\n", exploreState.currentLevel);
    printf("Current Area Index: %d\n", exploreState.currentAreaIndex);
    printf("Gold: %d\n", exploreState.playerGold);
    printf("Teleportation Stones: %d\n", exploreState.teleportationStones);
    printf("Day: %d\n", exploreState.currentDay);
    printf("Discovered Shrines: %d\n", exploreState.shrineCount);
    printf("Active Quest: %s\n", exploreState.activeQuest.active ? "Yes" : "No");
    printf("========================\n");
}];
        printf("\n=== %s ===\n", area->name);
        printf("%s\n", area->description);
        printf("Danger Level: %d | ", area->difficulty);
    } else {
        Area* area = &exploreState.areas[exploreState.currentLevel-1][0];
        printf("\n=== %s ===\n", area->name);
        printf("%s\n", area->description);
        printf("Danger Level: %d | ", exploreState.currentLevel);
    }
    
    printf("Gold: %d | Stones: %d | Day: %d\n",
           exploreState.playerGold, exploreState.teleportationStones, exploreState.currentDay);
}

static int generateEncounter() {
    int roll = rand() % 100;
    int dangerLevel = exploreState.currentLevel <= 4 ? 
                     exploreState.areas[exploreState.currentLevel-1][exploreState.currentAreaIndex].difficulty :
                     exploreState.currentLevel;
    
    // Adjust encounter chances based on danger level
    if (roll < (35 + dangerLevel * 5)) {
        return 0; // Combat
    } else if (roll < (50 + dangerLevel * 3)) {
        return 1; // Trap
    } else if (roll < (65 + dangerLevel * 2)) {
        return 2; // Chest
    } else if (roll < 80) {
        return 3; // Merchant
    } else if (roll < 90) {
        return 4; // Shrine
    } else {
        return 5; // Nothing
    }
}

static int handleCombatEncounter(GameState* gameState) {
    printf("\n*** ENEMIES APPROACH! ***\n");
    int dangerLevel = exploreState.currentLevel <= 4 ? 
                     exploreState.areas[exploreState.currentLevel-1][exploreState.currentAreaIndex].difficulty :
                     exploreState.currentLevel;
    
    if (dangerLevel <= 2) {
        printf("A group of weak monsters blocks your path!\n");
    } else if (dangerLevel <= 4) {
        printf("Dangerous creatures emerge from the shadows!\n");
    } else {
        printf("Fearsome beasts surround you - this will be a tough fight!\n");
    }
    
    printf("Prepare for combat!\n");
    waitForEnter();
    return 1; // Return to main game for combat
}

static void handleTrapEncounter(GameState* gameState) {
    int dangerLevel = exploreState.currentLevel <= 4 ? 
                     exploreState.areas[exploreState.currentLevel-1][exploreState.currentAreaIndex].difficulty :
                     exploreState.currentLevel;
    int trapDifficulty = dangerLevel + (exploreState.currentLevel / 3);
    
    printf("\n*** TRAP DETECTED! ***\n");
    printf("You notice a suspicious mechanism ahead...\n");
    printf("Trap Difficulty: %s (Requires skill ≥ %d)\n", 
           trapDifficulty <= 3 ? "Simple" :
           trapDifficulty <= 6 ? "Moderate" : "Complex",
           trapDifficulty * 3);
    
    printf("1. Attempt to disarm (Rogue skill)\n");
    printf("2. Try to avoid the trap\n");
    printf("3. Trigger it deliberately and take damage\n");
    
    int choice = getIntInput(1, 3);
    
    switch (choice) {
        case 1:
            if (attemptTrapDisarm(trapDifficulty, gameState)) {
                printf("Success! You carefully disarm the trap mechanism.\n");
                printf("Your party gains 15 EXP for skillful work!\n");
            } else {
                printf("Failed! The trap springs as you fumble with the mechanism!\n");
                printf("Your party takes damage from the triggered trap!\n");
                // TODO: Deal damage to party based on trap difficulty
            }
            break;
            
        case 2:
            if (rand() % 100 < 60) {
                printf("You successfully navigate around the trap!\n");
            } else {
                printf("Despite your caution, you trigger the trap anyway!\n");
                printf("Your party takes partial damage!\n");
                // TODO: Deal reduced damage
            }
            break;
            
        case 3:
            printf("You brace yourself and spring the trap intentionally!\n");
            printf("Your party takes full damage but pushes through!\n");
            // TODO: Deal full trap damage
            break;
    }
    
    waitForEnter();
}

static void handleChestEncounter() {
    int dangerLevel = exploreState.currentLevel <= 4 ? 
                     exploreState.areas[exploreState.currentLevel-1][exploreState.currentAreaIndex].difficulty :
                     exploreState.currentLevel;
    int chestDifficulty = dangerLevel + (exploreState.currentLevel / 4);
    
    printf("\n*** TREASURE CHEST DISCOVERED! ***\n");
    printf("A locked chest sits before you, gleaming with potential riches.\n");
    printf("Lock Difficulty: %s (Requires skill ≥ %d)\n",
           chestDifficulty <= 3 ? "Simple" :
           chestDifficulty <= 6 ? "Moderate" : "Master",
           chestDifficulty * 3);
    
    printf("1. Attempt lockpicking (Rogue skill)\n");
    printf("2. Force the chest open (damages contents)\n");
    printf("3. Leave the chest alone\n");
    
    int choice = getIntInput(1, 3);
    
    switch (choice) {
        case 1:
            // TODO: Check if party has Rogue and get actual skill level
            if (attemptLockpicking(chestDifficulty, NULL)) {
                int goldFound = 30 + (chestDifficulty * 20) + rand() % 40;
                printf("Success! The lock clicks open smoothly.\n");
                printf("Inside you find %d gold pieces!\n", goldFound);
                exploreState.playerGold += goldFound;
                
                // Small chance for teleportation stone
                if (rand() % 100 < 15) {
                    printf("You also discover a teleportation stone!\n");
                    exploreState.teleportationStones++;
                }
            } else {
                printf("The lockpicking attempt fails!\n");
                printf("The mechanism jams - you may try forcing it or leave.\n");
                
                printf("Force the chest? (Y/N): ");
                char response;
                scanf(" %c", &response);
                
                if (response == 'Y' || response == 'y') {
                    int goldFound = (20 + chestDifficulty * 10) / 2;
                    printf("You smash the lock! Some contents spill out damaged.\n");
                    printf("You recover %d gold pieces.\n", goldFound);
                    exploreState.playerGold += goldFound;
                }
            }
            break;
            
        case 2: {
            int goldFound = (25 + chestDifficulty * 15) / 2;
            printf("You force the chest open with brute strength!\n");
            printf("The violent opening damages some contents.\n");
            printf("You recover %d gold pieces.\n", goldFound);
            exploreState.playerGold += goldFound;
            break;
        }
        
        case 3:
            printf("You decide to leave the chest untouched.\n");
            printf("Perhaps another adventurer will have better luck...\n");
            break;
    }
    
    waitForEnter();
}

static void handleMerchantEncounter() {
    printf("\n*** TRAVELING MERCHANT ENCOUNTERED! ***\n");
    printf("A weathered trader sits beside a campfire, his pack full of goods.\n");
    printf("\"Greetings, adventurer! Care to see my wares?\"\n");
    
    printf("1. Browse merchant's goods\n");
    printf("2. Continue exploring\n");
    printf("3. Attack the merchant (hostile)\n");
    
    int choice = getIntInput(1, 3);
    
    switch (choice) {
        case 1: {
            // Generate merchant inventory
            MerchantItem items[6];
            int itemCount = 0;
            
            // Always available items
            strcpy(items[itemCount].name, "Small Health Potion");
            items[itemCount].price = 75;
            items[itemCount].healAmount = 100;
            items[itemCount].stock = 2 + rand() % 2; // 2-3 stock
            items[itemCount].itemType = 0;
            itemCount++;
            
            strcpy(items[itemCount].name, "Medium Health Potion");
            items[itemCount].price = 125;
            items[itemCount].healAmount = 150;
            items[itemCount].stock = 1 + rand() % 2; // 1-2 stock
            items[itemCount].itemType = 0;
            itemCount++;
            
            strcpy(items[itemCount].name, "Large Health Potion");
            items[itemCount].price = 200;
            items[itemCount].healAmount = 250;
            items[itemCount].stock = rand() % 2; // 0-1 stock (rare)
            items[itemCount].itemType = 0;
            itemCount++;
            
            strcpy(items[itemCount].name, "Mana Elixir");
            items[itemCount].price = 100;
            items[itemCount].manaAmount = 80;
            items[itemCount].stock = 2 + rand() % 2;
            items[itemCount].itemType = 0;
            itemCount++;
            
            strcpy(items[itemCount].name, "Lockpicking Tools");
            items[itemCount].price = 200;
            items[itemCount].stock = 1;
            items[itemCount].itemType = 1;
            itemCount++;
            
            // Rare teleportation stone (20% chance)
            if (rand() % 100 < 20) {
                strcpy(items[itemCount].name, "Teleportation Stone");
                items[itemCount].price = 1000;
                items[itemCount].stock = 1;
                items[itemCount].itemType = 2;
                itemCount++;
            }
            
            // Show merchant menu
            while (1) {
                printf("\n=== TRAVELING MERCHANT ===\n");
                printf("Your Gold: %d\n\n", exploreState.playerGold);
                
                printf("Available Items:\n");
                for (int i = 0; i < itemCount; i++) {
                    if (items[i].stock > 0) {
                        printf("%d. %s - %d gold (Stock: %d)\n", 
                               i+1, items[i].name, items[i].price, items[i].stock);
                        if (items[i].healAmount > 0) {
                            printf("   Restores %d HP\n", items[i].healAmount);
                        }
                        if (items[i].manaAmount > 0) {
                            printf("   Restores %d Mana\n", items[i].manaAmount);
                        }
                    }
                }
                
                // Rare merchant quest (5% chance)
                if (rand() % 100 < 5) {
                    printf("\n[RARE] The merchant looks distressed...\n");
                    printf("%d. Accept special quest\n", itemCount + 1);
                }
                
                printf("0. Leave merchant\n");
                
                int merchantChoice = getIntInput(0, itemCount + 1);
                if (merchantChoice == 0) break;
                
                if (merchantChoice <= itemCount) {
                    int itemIndex = merchantChoice - 1;
                    if (items[itemIndex].stock <= 0) {
                        printf("That item is out of stock!\n");
                        continue;
                    }
                    
                    if (exploreState.playerGold < items[itemIndex].price) {
                        printf("You don't have enough gold!\n");
                        continue;
                    }
                    
                    printf("Purchase %s for %d gold? (Y/N): ", 
                           items[itemIndex].name, items[itemIndex].price);
                    char confirm;
                    scanf(" %c", &confirm);
                    
                    if (confirm == 'Y' || confirm == 'y') {
                        exploreState.playerGold -= items[itemIndex].price;
                        items[itemIndex].stock--;
                        
                        if (strcmp(items[itemIndex].name, "Teleportation Stone") == 0) {
                            exploreState.teleportationStones++;
                        }
                        
                        printf("Purchased %s!\n", items[itemIndex].name);
                        // TODO: Add item to player inventory
                    }
                } else {
                    // Merchant quest
                    printf("\"Bandits attacked my caravan! I dropped a valuable chest during escape.\"\n");
                    printf("\"It's hidden near the old dead tree, north of here.\"\n");
                    printf("\"Retrieve it and I'll reward you handsomely...\"\n");
                    
                    printf("Accept quest? (Y/N): ");
                    char accept;
                    scanf(" %c", &accept);
                    
                    if (accept == 'Y' || accept == 'y') {
                        exploreState.activeQuest.active = 1;
                        strcpy(exploreState.activeQuest.description, "Retrieve merchant's stolen chest");
                        strcpy(exploreState.activeQuest.questZoneName, "Bandit Ambush Site");
                        exploreState.activeQuest.rewardGold = 150;
                        strcpy(exploreState.activeQuest.rewardItem, "Large Health Potion");
                        printf("Quest accepted! You'll be transported to the quest area.\n");
                        waitForEnter();
                        return;
                    }
                }
                
                waitForEnter();
            }
            break;
        }
        
        case 2:
            printf("You politely decline and continue on your way.\n");
            break;
            
        case 3:
            printf("You attack the merchant! This will have consequences...\n");
            // TODO: Trigger combat with merchant (hostile)
            break;
    }
    
    waitForEnter();
}

static void handleShrineEncounter() {
    Area* currentArea;
    if (exploreState.currentLevel <= 4) {
        currentArea = &exploreState.areas[exploreState.currentLevel-1][exploreState.currentAreaIndex];
    } else {
        currentArea = &exploreState.areas[exploreState.currentLevel-1][0];
    }
    
    if (!currentArea->hasShrine) {
        printf("You continue exploring...\n");
        waitForEnter();
        return;
    }
    
    if (currentArea->shrineDiscovered) {
        printf("You return to the familiar %s.\n", currentArea->shrineName);
        printf("The shrine provides a moment of peace and rest.\n");
        printf("All party members recover some HP and mana.\n");
        // TODO: Restore some HP/mana to party
    } else {
        printf("\n*** ANCIENT SHRINE DISCOVERED! ***\n");
        printf("You discover %s!\n", currentArea->shrineName);
        printf("The shrine radiates with mystical energy...\n");
        printf("This location is now available for teleportation!\n");
        
        currentArea->shrineDiscovered = 1;
        exploreState.discoveredShrines[exploreState.shrineCount][0] = exploreState.currentLevel;
        exploreState.discoveredShrines[exploreState.shrineCount][1] = exploreState.currentAreaIndex;
        exploreState.shrineCount++;
    }
    
    waitForEnter();
}

static int handleMerchantQuest(GameState* gameState) {
    printf("\n=== QUEST ZONE: %s ===\n", exploreState.activeQuest.questZoneName);
    printf("%s\n", exploreState.activeQuest.description);
    
    printf("1. Search near the dead tree\n");
    printf("2. Look for bandit tracks\n");
    printf("3. Return to merchant (abandon quest)\n");
    
    int choice = getIntInput(1, 3);
    
    switch (choice) {
        case 1:
            printf("You find the merchant's chest buried under leaves!\n");
            if (rand() % 100 < 30) {
                printf("But bandits are still here guarding it!\n");
                return 1; // Trigger combat
            } else {
                printf("The area is clear. You retrieve the chest safely.\n");
                exploreState.activeQuest.completed = 1;
                printf("Quest completed! Returning to merchant...\n");
                
                // Give rewards
                exploreState.playerGold += exploreState.activeQuest.rewardGold;
                printf("Merchant rewards you with %d gold and a %s!\n",
                       exploreState.activeQuest.rewardGold, exploreState.activeQuest.rewardItem);
                
                exploreState.activeQuest.active = 0;
                waitForEnter();
                return runExploration(gameState);
            }
            break;
            
        case 2:
            printf("You follow bandit tracks but they lead to an ambush!\n");
            return 1; // Trigger combat
            
        case 3:
            printf("You abandon the quest and return to the merchant.\n");
            printf("The merchant looks disappointed...\n");
            exploreState.activeQuest.active = 0;
            waitForEnter();
            return runExploration(gameState);
    }
    
    return runExploration(gameState);
}

static int attemptLockpicking(int difficulty, GameState* gameState) {
    // TODO: Check if party has Rogue and get actual skill
    int rogueSkill = 8; // Placeholder
    int successChance = 70 - (difficulty * 10) + (rogueSkill * 5);
    
    if (successChance < 10) successChance = 10;
    if (successChance > 95) successChance = 95;
    
    printf("Lockpicking attempt... Success chance: %d%%\n", successChance);
    return (rand() % 100) < successChance;
}

static int attemptTrapDisarm(int difficulty, GameState* gameState) {
    // TODO: Check if party has Rogue and get actual skill
    int rogueSkill = 8; // Placeholder
    int successChance = 60 - (difficulty * 12) + (rogueSkill * 6);
    
    if (successChance < 15) successChance = 15;
    if (successChance > 90) successChance = 90;
    
    printf("Disarming attempt... Success chance: %d%%\n", successChance);
    return (rand() % 100) < successChance;
}

static void showTeleportOptions() {
    printf("\n=== DISCOVERED SHRINES ===\n");
    printf("Available destinations:\n");
    printf("1. Village Fountain (Free)\n");
    
    for (int i = 0; i < exploreState.shrineCount; i++) {
        int level = exploreState.discoveredShrines[i][0];
        int areaIndex = exploreState.discoveredShrines[i][1];
        Area* area = &exploreState.areas[level-1][areaIndex];
        printf("%d. %s (1 stone)\n", i + 2, area->shrineName);
    }
    
    printf("0. Cancel\n");
    printf("Teleportation stones: %d\n", exploreState.teleportationStones);
    
    int choice = getIntInput(0, exploreState.shrineCount + 1);
    
    if (choice == 0) return;
    
    if (choice == 1) {
        // Free return to village
        printf("You teleport back to the village...\n");
        exploreState.currentLevel = 1;
        exploreState.currentAreaIndex = -1;
        exploreState.currentDay++;
        printf("Day %d begins...\n", exploreState.currentDay);
    } else if (choice <= exploreState.shrineCount + 1) {
        if (exploreState.teleportationStones <= 0) {
            printf("You don't have any teleportation stones!\n");
            waitForEnter();
            return;
        }
        
        int shrineIndex = choice - 2;
        exploreState.teleportationStones--;
        exploreState.currentLevel = exploreState.discoveredShrines[shrineIndex][0];
        exploreState.currentAreaIndex = exploreState.discoveredShrines[shrineIndex][1];
        
        Area* area = &exploreState.areas[exploreState.currentLevel-1][exploreState.currentAreaIndex// Write some thing like explore(Game *game) here
