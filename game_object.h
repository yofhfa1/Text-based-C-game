// game.h
typedef struct {
    int level;
    int day;
    int timeOfTheDay;
    int gold;
    LinkedList itemList;    
    Champion champion[3];
    // Implement map using adjacentMatrix;
    int initialized;
    Config config;
} Game;

typedef struct node {
    void *value;
    struct node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
} LinkedList;

void init(LinkedList list);
void insert(LinkedList list, void *pt);
*Node getElementAt(LinkedList list, int index);
void removeAt(LinkedList list, int index);
void freeList(LinkedList list);

typedef struct {
    int allowSaveInCombat;
    int difficulty;

    int explorationTurn;
    //first element is combat chance, second element is trade chance, 3rd is minigameChance
    int scenarioWeights[3];
    int maxTimeOfTheDay;
} Config;

enum ItemType {
    WEAPON,
    HEALTH_POTION,
    SPECIAL
};

typedef struct {
    enum ItemType type;
    char *name;
    int value;
    int effectValue;
} Item;

typedef struct {
    LinkedList itemList;
} Shop;

typedef struct location {
    LinkedList monsterList;
    LinkedList adjacentList;
} Location;

enum Class {
    WIZARD,
    KNIGHT,
    PALADIN,
    ROGUE,
    ELF
};

typedef struct {
    int health;
    int maxHealth;
    int damage;
    enum Class class;
} Champion;

typedef struct {
    int health;
    int maxHealth;
    int damage;
    char name[30];
} Monster;