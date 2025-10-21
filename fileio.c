#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "game_object.h"
#include "fileio.h"
#include <cJSON.h>

#define SAVE_DIRECTORY "./saves/"
#define JSON_DIR "./json"
#define MAP_FILE "./json/map.json"
#define CONFIG_FILE "./json/config.json"
#define SHOP_FILE "./json/shop.json"
#define MAX_AUTOSAVE 8
int current_save;

void initFileIO() {
    current_save = 0;
    mkdir("./json", S_IRWXU);
}

// Make multithreading for this later
void saveGame(Game * game, int autosave) {
    char temp[50];
    if (!autosave) {
        printf("Enter the name of the file you want to save (leave blank for default name)");
        fgets(temp, 50, stdin);
    } else {
        sprintf(temp, "%s%s%d\n", SAVE_DIRECTORY, /*autosave*/1?"autosave_":"", current_save);
        current_save++;
        if (current_save > MAX_AUTOSAVE) {
            current_save = 0;
        }
    }
    int fpointer = open(temp, O_WRONLY);
    if (fpointer == -1) {
        printf("error");
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "level", game->level);
    cJSON_AddNumberToObject(root, "day", game->day);
    cJSON_AddNumberToObject(root, "timeOfTheDay", game->timeOfTheDay);
    cJSON *champion = cJSON_AddArrayToObject(root, "champions");
    int i;
    for (i = 0; i < 3;i++) {
        cJSON *temp = cJSON_CreateObject();
        cJSON_AddNumberToObject(temp, "health", game->champion[i].health);
        cJSON_AddNumberToObject(temp, "maxHealth", game->champion[i].maxHealth);
        cJSON_AddNumberToObject(temp, "damage", game->champion[i].damage);
        cJSON_AddItemToArray(champion, temp);       
    }

    char *json_str = cJSON_Print(root);

    // Write JSON string to file
    write(fpointer, json_str, strlen(json_str));

    // Cleanup
    close(fpointer);
    cJSON_free(json_str);
    cJSON_Delete(root);   
}

void loadGame(Game * game) {
    char filename[20] = "buhbuh\0";
    // Unsafe but handled in main
    int fd = open(filename, O_RDONLY);
    struct stat st;

    if (fstat(fd, &st) != 0) {
        //Handle
    } 

    char buffer[st.st_size];
    size_t size = read(fd, buffer, st.st_size);
    if (size < st.st_size) {
        printf("Read fail! Please try another file");
    }
    close(fd);

    cJSON *root = cJSON_Parse(buffer);
    
    cJSON *temp = cJSON_GetObjectItem(root, "level");
    if (cJSON_IsNumber(temp)) {
        game->level = temp->valueint;
    }
    temp = cJSON_GetObjectItem(root, "day");
    if (cJSON_IsNumber(temp)) {
        game->day = temp->valueint;
    }
    temp = cJSON_GetObjectItem(root, "timeOfTheDay");
    if (cJSON_IsNumber(temp)) {
        game->timeOfTheDay = temp->valueint;
    }
    cJSON *champions = cJSON_GetObjectItem(root, "champions");
    cJSON *item = NULL;
    for (int i = 0;i < 3;i ++) {
        item = cJSON_GetArrayItem(champions, i);
        temp = cJSON_GetObjectItem(item, "health");
        if (cJSON_IsNumber(temp)) {
            game->champion[i].health = temp->valueint;
        }
        temp = cJSON_GetObjectItem(item, "maxHealth");
        if (cJSON_IsNumber(temp)) {
            game->champion[i].maxHealth = temp->valueint;
        }
        temp = cJSON_GetObjectItem(item, "damage");
        if (cJSON_IsNumber(temp)) {
            game->champion[i].damage = temp->valueint;
        }
    }

    cJSON_Delete(root);
    // temp = cJSON_GetObjectItem(root, "timeOfTheDay");
    // if (cJSON_IsNumber(temp)) {
    //     game->timeOfTheDay = temp->valueint;
    // }
}

void saveMap(Game * game) {
    int fd = open(MAP_FILE, O_WRONLY);
    if (fd == -1) {}

    cJSON *root = cJSON_CreateArray();
    int *pt = game->map;
    for (int i = 0;i < game->mapSize;i++) {
        // char temp[7] = "level";
        // temp[5] = i;
        // temp[6] = '\0';
        cJSON *adjMap = cJSON_CreateIntArray(pt, game->mapSize);
        cJSON_AddItemToArray(root, adjMap);
    }

    char *json_str = cJSON_Print(root);
    write(fd, json_str, strlen(json_str));

    close(fd);
    cJSON_free(json_str);
    cJSON_Delete(root);
}

void initMap(Game * game) {
    int fd = open(MAP_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {}

    cJSON *root = cJSON_CreateArray();
    for (int i = 0;i < 8;i++) {
        int a[8] = {1, 1, 1, 1, 1, 1, 1, 1};
        cJSON *adjMap = cJSON_CreateIntArray(a, 8);
        cJSON_AddItemToArray(root, adjMap);
    }

    char *json_str = cJSON_Print(root);
    write(fd, json_str, strlen(json_str));

    close(fd);
    cJSON_free(json_str);
    cJSON_Delete(root);
}

void loadMap(Game * game) {
    int fd = open(MAP_FILE, O_RDONLY);
    if (fd == -1) {
        initMap(game);
        fd = open(MAP_FILE, O_RDONLY);
    }
    
    struct stat st;
    if (fstat(fd, &st) != 0) {
        printf("Map reading failed! Exiting!");
        game->initialized = 0;
        return;
    } 
    if (st.st_size == 0) {
        initMap(game);
        fd = open(MAP_FILE, O_RDONLY);
    }
    char buffer[st.st_size];
    size_t size = read(fd, buffer, st.st_size);
    if (size < st.st_size) {
        printf("Map reading failed! Exiting!");
        game->initialized = 0;
        return;
    }
    close(fd);

    cJSON *root = cJSON_Parse(buffer);
    if (!cJSON_IsArray(root)) {
        printf("Map reading failed! Exiting!");
        game->initialized = 0;
        return;
    }
    game->mapSize = cJSON_GetArraySize(root);
    game->map = malloc(game->mapSize * game->mapSize * sizeof(int));
    int *pt = game->map;
    
    for (int i = 0;i < game->mapSize;i++) {
        cJSON *adjMap = cJSON_GetArrayItem(root, i);
        for (int j = 0;j < game->mapSize;j++) {
            *pt = cJSON_GetArrayItem(adjMap, j)->valueint;
        }
    }
    cJSON_Delete(root);
}

// implement location data 

void loadMapAndLocationData(Game * game) {
    loadMap(game);
    loadLocationData(game);
}

void saveConfig(Game * game) {
    int fd = open(CONFIG_FILE, O_WRONLY);
    if (fd == -1) {}

    cJSON *root = cJSON_CreateObject();
    Config config = game->config;
    cJSON_AddNumberToObject(root, "allowSaveInCombat", config.allowSaveInCombat);
    cJSON_AddNumberToObject(root, "difficulty", config.difficulty);
    cJSON_AddNumberToObject(root, "baseLvUpXp", config.baseLvUpXp);
    cJSON_AddNumberToObject(root, "lvUpXpMul", config.lvUpXpMul);
    cJSON_AddNumberToObject(root, "chestGoldMin", config.chestGoldMin);
    cJSON_AddNumberToObject(root, "chestGoldMax", config.chestGoldMax);
    cJSON_AddNumberToObject(root, "chestItemChance", config.chestItemChance);
    cJSON_AddNumberToObject(root, "lockpickChance", config.lockpickChance);
    cJSON_AddNumberToObject(root, "trapDisarmChance", config.trapDisarmChance);
    cJSON_AddNumberToObject(root, "trapDisarmExp", config.trapDisarmExp);
    cJSON_AddNumberToObject(root, "trapDamage", config.trapDamage);

    cJSON_AddNumberToObject(root, "explorationTurn", config.explorationTurn);
    cJSON *scenarioWeights = cJSON_CreateIntArray(config.scenarioWeights, 3);
    cJSON_AddItemToObject(root, "scenarioWeights", scenarioWeights);
    cJSON_AddNumberToObject(root, "maxTimeOfTheDay", config.maxTimeOfTheDay);
    cJSON_AddNumberToObject(root, "teleportPenalty", config.teleportPenalty);
    cJSON_AddNumberToObject(root, "sellValue", config.sellValue);

    char *json_str = cJSON_Print(root);
    write(fd, json_str, strlen(json_str));

    close(fd);
    cJSON_free(json_str);
    cJSON_Delete(root);

}

void initConfig(Game * game) {
    int fd = open(CONFIG_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {}

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "allowSaveInCombat", 0);
    cJSON_AddNumberToObject(root, "difficulty", 1);
    cJSON_AddNumberToObject(root, "baseLvUpXp", 100);
    cJSON_AddNumberToObject(root, "lvUpXpMul", 1.2);
    cJSON_AddNumberToObject(root, "chestGoldMin", 50);
    cJSON_AddNumberToObject(root, "chestGoldMax", 100);
    cJSON_AddNumberToObject(root, "chestItemChance", 50);
    cJSON_AddNumberToObject(root, "lockpickChance", 80);
    cJSON_AddNumberToObject(root, "trapDisarmChance", 80);
    cJSON_AddNumberToObject(root, "trapDisarmExp", 100);
    cJSON_AddNumberToObject(root, "trapDamage", 10);

    cJSON_AddNumberToObject(root, "explorationTurn", 3);
    int a[3] = {30, 30, 40};
    cJSON *scenarioWeights = cJSON_CreateIntArray(a, 3);
    cJSON_AddItemToObject(root, "scenarioWeights", scenarioWeights);
    cJSON_AddNumberToObject(root, "maxTimeOfTheDay", 4);
    cJSON_AddNumberToObject(root, "teleportPenalty", 1);
    cJSON_AddNumberToObject(root, "sellValue", 0.5);
    char *json_str = cJSON_Print(root);
    write(fd, json_str, strlen(json_str));
   
    close(fd);
    // cJSON_free(json_str);
    cJSON_Delete(root);
    printf("initConfig complete");
}

void loadConfig(Game * game) {
    // If config file not found init 
    int fd = open(CONFIG_FILE, O_RDONLY);
    if (fd == -1) {
        printf("%d\n", errno);
        initConfig(game);
        fd = open(CONFIG_FILE, O_RDONLY);
    }
    
    struct stat st;
    if (fstat(fd, &st) != 0) {
        printf("Config reading failed! Exiting!\n");
        printf("Stat failed errno\n");
        game->initialized = 0;
        return;
    } 
    if (st.st_size == 0) {
        initConfig(game); 
    }
    char buffer[st.st_size];
    size_t size = read(fd, buffer, st.st_size);
    if (size < st.st_size) {
        printf("Config reading failed! Exiting!");
        game->initialized = 0;
        return;
    }
    close(fd);

    cJSON *root = cJSON_Parse(buffer);
    game->config.allowSaveInCombat = cJSON_GetObjectItem(root, "allowSaveInCombat")->valueint;
    game->config.difficulty = cJSON_GetObjectItem(root, "difficulty")->valueint;
    game->config.baseLvUpXp = cJSON_GetObjectItem(root, "baseLvUpXp")->valueint;
    game->config.lvUpXpMul = cJSON_GetObjectItem(root, "lvUpXpMul")->valueint;
    game->config.chestGoldMin = cJSON_GetObjectItem(root, "chestGoldMin")->valueint;
    game->config.chestGoldMax = cJSON_GetObjectItem(root, "chestGoldMax")->valueint;
    game->config.chestItemChance = cJSON_GetObjectItem(root, "chestItemChance")->valueint;
    game->config.lockpickChance = cJSON_GetObjectItem(root, "lockpickChance")->valueint;
    game->config.trapDisarmChance = cJSON_GetObjectItem(root, "trapDisarmChance")->valueint;
    game->config.trapDisarmExp = cJSON_GetObjectItem(root, "trapDisarmExp")->valueint;
    game->config.trapDamage = cJSON_GetObjectItem(root, "trapDamage")->valueint;

    game->config.explorationTurn = cJSON_GetObjectItem(root, "explorationTurn")->valueint;
    cJSON *scenarioWeights = cJSON_GetObjectItem(root, "scenarioWeights");
    game->config.scenarioWeights[0] = cJSON_GetArrayItem(scenarioWeights, 0)->valueint;
    game->config.scenarioWeights[1] = cJSON_GetArrayItem(scenarioWeights, 1)->valueint;
    game->config.scenarioWeights[2] = cJSON_GetArrayItem(scenarioWeights, 2)->valueint;
    game->config.maxTimeOfTheDay = cJSON_GetObjectItem(root, "maxTimeOfTheDay")->valueint;
    game->config.teleportPenalty = cJSON_GetObjectItem(root, "teleportPenalty")->valueint;
    game->config.sellValue = cJSON_GetObjectItem(root, "sellValue")->valueint;

    cJSON_Delete(root);
}

void saveShop(Game * game) {
    int fd = open(SHOP_FILE, O_WRONLY);
    if (fd == -1) {}

    cJSON *root = cJSON_CreateObject();
    cJSON *itemList = cJSON_AddArrayToObject(root, "itemList");
    Node * current = game->shop.itemList.head;
    while (current != NULL) {
        cJSON *item_object = cJSON_CreateObject();
        Item *item = (Item *)current->value;
        cJSON_AddNumberToObject(item_object, "type", item->type);
        cJSON_AddStringToObject(item_object, "name", item->name);
        cJSON_AddNumberToObject(item_object, "value", item->value);
        cJSON_AddNumberToObject(item_object, "effectValue", item->effectValue);
        cJSON_AddItemToArray(itemList, item_object);
    }

    char *json_str = cJSON_Print(root);
    write(fd, json_str, strlen(json_str));

    close(fd);
    cJSON_free(json_str);
    cJSON_Delete(root);
}

void initShop(Game * game) {
    int fd = open(SHOP_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        printf("error when open file %d", errno);
    }

    cJSON *root = cJSON_CreateObject();
    cJSON *itemList = cJSON_AddArrayToObject(root, "itemList");
    Item item;
    item.effectValue = HEALTH_POTION;
    item.name[1] = 'a';
    item.name[2] = '\0';
    item.value = 50;
    cJSON *item_object = cJSON_CreateObject();
    cJSON_AddNumberToObject(item_object, "type", item.type);
    cJSON_AddStringToObject(item_object, "name", item.name);
    cJSON_AddNumberToObject(item_object, "value", item.value);
    cJSON_AddNumberToObject(item_object, "effectValue", item.effectValue);
    cJSON_AddItemToArray(itemList, item_object);

    char *json_str = cJSON_Print(root);
    write(fd, json_str, strlen(json_str));

    close(fd);
    cJSON_Delete(root);
}

void loadShop(Game * game) {
    int fd = open(SHOP_FILE, O_RDONLY);
    if (fd == -1) {
        initShop(game);
        fd = open(SHOP_FILE, O_RDONLY);
    }
    
    struct stat st;
    if (fstat(fd, &st) != 0) {
        printf("Shop reading failed! Exiting!");
        game->initialized = 0;
        return;
    }
    if (st.st_size) {
        initShop(game);
        fd = open(SHOP_FILE, O_RDONLY);
    }
    char buffer[st.st_size];
    size_t size = read(fd, buffer, st.st_size);
    if (size < st.st_size) {
        printf("Shop reading failed! Exiting!");
        game->initialized = 0;
        return;
    }
    close(fd);

    cJSON *root = cJSON_Parse(buffer);
    cJSON *itemList = cJSON_GetObjectItem(root, "itemList");
    cJSON *temp;
    
    for (int i = 0;i < cJSON_GetArraySize(itemList);i++) {
        temp = cJSON_GetArrayItem(itemList, i);
        Item * item = (Item *) malloc(sizeof(Item));
        item->type = cJSON_GetObjectItem(temp, "type")->valueint;
        strcpy(item->name, cJSON_GetObjectItem(temp, "name")->valuestring);
        item->value = cJSON_GetObjectItem(temp, "value")->valueint;
        item->effectValue = cJSON_GetObjectItem(temp, "effectValue")->valueint;
        insert(&(game->shop.itemList), item);
    }
    cJSON_Delete(root);
}