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
    Shop shop;
} Game;

typedef struct node_t {
    void *value;
    struct node_t *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    int size;
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
    //first element is combat chance, second element is trade chance, third is minigameChance
    int scenarioWeights[3];
    int maxTimeOfTheDay;

    double sellValue;
} Config;

enum ItemType {
    WEAPON,
    HEALTH_POTION,
    SPECIAL
};

typedef struct {
    enum ItemType type;
    char name[50];
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