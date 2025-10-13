// game.h
typedef struct {
    int level;
    int day;
    int timeOfTheDay;
    int gold;
    LinkedList itemList;    
    Champion champion[3];
    // Implement map using adjacentMatrix;
    int *map;
    LocationData *locationData;
    int mapSize;
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
Node* getElementAt(LinkedList list, int index);
Node* removeAt(LinkedList list, int index);
void freeList(LinkedList list);
// Method to find the index'th true value in a binary map
// Return -1 if an error was found
Node* findBinaryMapping(int binaryMap[], int index, int length);
void addTimeOfTheDay(Game * game, int value);

typedef struct {
    int allowSaveInCombat;
    int difficulty;

    int explorationTurn;
    //first element is combat chance, second element is trade chance, third is minigameChance
    int scenarioWeights[3];
    int maxTimeOfTheDay;
    int teleportPenalty;

    double sellValue;
} Config;

enum ItemType {
    WEAPON,
    HEALTH_POTION,
    SPECIAL
};

typedef struct {
    char name[50];
    LinkedList monsterList;
} LocationData;

typedef struct {
    enum ItemType type;
    char name[50];
    int value;
    int effectValue;
} Item;

typedef struct {
    LinkedList itemList;
} Shop;

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