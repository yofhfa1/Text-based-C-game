// game.h
typedef struct {
    int level;
    Champion champion[3];
    int map[][];
} Game;

typedef struct {
    int allowSaveInCombat;
    int difficulty;

    int explorationTurn;
    int combatChance;
    int tradeChance;
    int minigameChance;
} Config;

typedef struct {
    int health;
    int maxHealth;
    int damage;

} Champion