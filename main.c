#include "fileio.c"
#include "game.c"
#include "explore.c"
#include "combat.c"

typedef struct {
    
} Game;

Game game;

void showMainMenu();
void doGameTick();

int main() {
    showMainMenu(0);
}