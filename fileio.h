#include "game_object.h"

void initFileIO();
void saveGame(Game * game, int autosave);
void loadGame(Game * game);
void loadMap(Game * game);
void loadMapAndLocationData(Game * game);
void loadConfig(Game * game);
void loadShop(Game * game);