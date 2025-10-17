#include <stdio.h>
#include <stdlib.h>
#include "game_object.h"

void doTraining(Game * game) {
	int choice;
	while (1) {
		printf("Training menu: \n");
		printf("1. Mock battle\n");
		printf("2. Change formation\n");
		printf("3. Exit\n");
		printf("Your choice: ");
		scanf("%d", &choice);
		getchar();

		switch (choice)
		{
		case 1:
			createBattle(game);
			break;
		case 2:
			editFormation(game);
			break;
		case 3:
			return;
		default:
			printf("Please input a number in range 1-3");
			break;
		}
	}	
}

void doGameTick(Game * game){
	int choice;
	while(1){
		if (game->level == 0) {
			printf("\n====MAIN MENU (Day %d) ===\n", game->day);
			printf("1. Shop\n");
			printf("2. Di chuyen\n");
			printf("3. Huan luyen\n");
			printf("4. Nghi ngoi\n");
			printf("5. Thoat\n");
			printf("Chon: ");
			scanf("%d", &choice);
			
			switch(choice){
				case 1:
					openShop(game);
					break;
			    case 2:
			    	move(game);
			    	break;
			    case 3:
			    	doTraining(game);
			    	break;
			    case 4:
			    	rest();
			    	break;
			    case 5:
			    	return;
				default:
					printf("Lua chon khong hop le.\n");
			}
		} else {
			printf("\n=== LUA CHON HANH DONG ===\n");
			printf("1. Kham pha\n");
			printf("2. Di chuyen\n");
			printf("3. Bien ve\n");
			printf("4. Thoat\n");
			printf("Nhap lua chon: ");
			scanf("%d", &choice);

			switch (choice) {
				case 1:
					explore(game);
					break;
				case 2:
					move(game);
					break;
				case 3:
					if (game->timeOfTheDay < game->config.maxTimeOfTheDay) {
						game->timeOfTheDay++;
						teleport(game, 0);	
					} else {
						printf("Can teleport home when it's night time");
					}
					break;
				case 4:
					return;
				default:
					printf("Lua chon khong hop le, vui long nhap lai.\n");
			}
		}
	}
	return 0;
}
