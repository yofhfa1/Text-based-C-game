#include "lib/cJson/cJSON.h"
#include <sys/stat.h>
#include "game_object.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define SAVE_DIRECTORY "./saves/"
#define MAP_FILE "./map.json"
int current_save;

void initFileIO() {
    current_save = 0;
}

// Make multithreading for this later
void saveGame(Game * game, char * filename) {
    char temp[50];
    sprintf(temp, "%s%s%n", SAVE_DIRECTORY, autosave?"autosave_":"", current_save);
    int fpointer = open(temp, O_WRONLY);
    if (fpointer == -1) {
        printf("error");
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "level", game->level);
    cJSON_AddNumberToObject(root, "day", game->day);
    cJSON_AddNumberToObject(root, "timeOfTheDay", game->timeOfTheDay);
    cJSON *champion = cJSON_AddArrayToObject(root, "champion");
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

void loadGame(Game * game, char * filename) {
    // Unsafe but handled in main
    int fd = open(filename, O_RDONLY);
    struct stat st;

    if (fstat(fd, &st) != 0) {
        //Handle
    } 

    char buffer[st.st_size];
    size_t size = read(fd, buffer, st.st_size);
    close(fd);

    cJSON *root = cJSON_Parse(buffer);
    if (!root) { free(buffer); return 0; }
    
    
}

void saveMap(Game * game) {
    
}

void loadMap(Game * game) {
   
}