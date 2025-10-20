#include <stdio.h>
#include "fileio.h"
#include "game.h"
#include "explore.h"
#include "combat.h"
#include "game_object.h"
#include "character.h"
#include "movement.h"
#include "cJSON.h"

void showMainMenu(Game *game);
void showRoleMenu(Game *game);
void adminMenu();

int main() {
    Game game;
    game.initialized = 0;
    game.isTester = 0;
    initFileIO();

    showRoleMenu(&game);
    showMainMenu(&game);
    
    return 0;
}

void showRoleMenu(Game *game) {
    int modeChoice;
    while (1) {
        printf("\n====================================\n");
        printf("     WELCOME TO OSG ADVENTURE GAME  \n");
        printf("====================================\n");
        printf("Select mode:\n");
        printf("1. Admin mode\n");
        printf("2. Tester mode\n");
        printf("3. Main game\n");
        printf("0. Exit\n");
        printf("------------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &modeChoice);

	    switch(modeChoice) {
	    	case 1:
	            adminMenu();
	            continue;
	        case 2:
	            game->isTester = 1;
	            return;
            case 3: 
                game->isTester = 0;
                return;
	        default:
	            printf("Invalid choice! Please choose 1-3.\n");
	            continue;
		}
    }
}

void showMainMenu(Game * game) {
    int choice;
    while (1) {
        printf("Main menu: \n");
        // Game is already running, add new option
        if (game->initialized) {
            printf("1. Continue\n");
            printf("2. Save game\n");
            printf("3. View stats\n");
            printf("4. New game\n");
            printf("5. Load game\n");
            printf("6. Exit\n");
            printf("Your choice: ");
            scanf("%d", &choice);
            getchar();

            switch (choice)
            {
                case 1:
                    doGameTick(game);
                    break;
                case 2:
                    //This is manual save so autosave = 0
                    saveGame(game, 0);
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
                    printf("Please input a number in range of 1-6\n");
            }
        } else {
            printf("1. New game\n");
            printf("2. Load game\n");
            printf("3. Exit\n");
            printf("Your choice: ");
            scanf("%d", &choice);
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
                    printf("Please input a number in range of 1-3\n");
            }
        }
    }
}

void adminMenu() {
    int choice;
    while (1) {
        printf("\n========== ADMIN MENU ==========\n");
        printf("1. View all saved games\n");
        printf("2. Delete save file\n");
        printf("3. Back to mode selection\n");
        printf("--------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            printf("[Save files list]\n- save1.dat\n- save2.dat\n");
        } 
        else if (choice == 2) {
            char filename[50];
            printf("Enter file name to delete: ");
            scanf("%s", filename);
            if (remove(filename) == 0)
                printf("File '%s' deleted successfully!\n", filename);
            else
                printf("Failed to delete '%s'.\n", filename);
        } 
        else if (choice == 3) {
            printf("Returning to mode selection...\n");
            return;
        } 
        else {
            printf("Invalid choice! Please choose 1-3.\n");
        }
    }
}
