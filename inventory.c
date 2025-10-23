#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inventory.h"
Item *createItemInstance(const Item *templateItem, int qty) {
Item *it = malloc(sizeof(Item));
    if (!it) return NULL;
    *it = *templateItem; // shallow copy
    it->qty = qty;
    it->equipped_by = -1;
    return it;
}
void addItemToInventory(Game *game, Item *item) {
    if (!game || !item) return;
    if (item->type == HEALTH_POTION) {
        Node *node = game->itemList.head;
        while (node) {
            Item *inv = (Item *) node->value;
            if (inv->type == HEALTH_POTION && strcmp(inv->name, item->name) == 0) {
                inv->qty += item->qty;
                free(item);
                return;
            }
            node = node->next;
        }
    }
    insert(&(game->itemList), item);
}
void openInventoryMenu(Game *game) {
    int choice;
    while (1) {
        printf("\n======== INVENTORY ========\n");
        if (game->itemList.size == 0) {
            printf("You have no items.\n");
        } else {
            Node *node = game->itemList.head;
            int i = 1;
            while (node) {
                Item *item = (Item *) node->value;
                if (item->type == HEALTH_POTION) {
                    printf("%d. %s x%d\n", i, item->name, item->qty);
                } else {
                    printf("%d. %s\n", i, item->name);
                }
                node = node->next;
                i++;
            }
        }
        printf("0. Exit\n");
        printf("Select item index to inspect/use/equip/drop (0 to exit): ");
        scanf("%d", &choice);
        if (choice == 0) break;
        if (choice < 0 || choice > game->itemList.size) {
            printf("Invalid choice\n");
            continue;
        }
        Node *selNode = getElementAt(game->itemList, choice-1);
        if (!selNode) {
            printf("Could not find item\n");
            continue;
        }
        Item *sel = (Item *) selNode->value;
        printf("Selected: %s\n", sel->name);
        printf("1. Use\n2. Equip/Unequip\n3. Drop\n4. Back\n");
        int action;
        printf("Action: ");
        scanf("%d", &action);
        if (action == 1) {
            if (sel->type == HEALTH_POTION) {
                for (int i=0;i<3;i++) {
                    if (game->champion[i].maxHealth > 0) {
                        game->champion[i].health += sel->effectValue;
                        if (game->champion[i].health > game->champion[i].maxHealth)
                            game->champion[i].health = game->champion[i].maxHealth;
                        printf("%s used on %s (champion %d). Healed %d.\n", sel->name, "Champion", i+1, sel->effectValue);
                        sel->qty -= 1;
                        if (sel->qty <= 0) removeAt(game->itemList, choice-1);
                        break;
                    }
                }
            } else {
                printf("Cannot use this item directly. Try equip.\n");
            }
        } else if (action == 2) {
            if (sel->equipped_by == -1) {
                for (int i=0;i<3;i++) {
                    if (game->champion[i].maxHealth > 0) {
                        sel->equipped_by = i;
                        game->champion[i].damage += sel->effectValue;
                        printf("Equipped %s to champion %d\n", sel->name, i+1);
                        break;
                    }
                }
            } else {
                int idx = sel->equipped_by;
                game->champion[idx].damage -= sel->effectValue;
                sel->equipped_by = -1;
                printf("Unequipped %s from champion %d\n", sel->name, idx+1);
            }
        } else if (action == 3) {
            printf("Dropping %s\n", sel->name);
            removeAt(game->itemList, choice-1);
        } else {
            continue;
        }
    }
}
void handleMonsterDrops(Game *game, LinkedList *drops, int autoPickup) {
    if (!drops) return;
    Node *node = drops->head;
    while (node) {
        Item *it = (Item *) node->value;
        if (autoPickup) {
            addItemToInventory(game, it);
        } else {
            addItemToInventory(game, it);
        }
        node = node->next;
    }
}
void serializeInventory(Game *game, FILE *fp) {
}
void deserializeInventory(Game *game, FILE *fp) {
}
void printInventory(Game *game) {
    printf("\n--- INVENTORY DUMP ---\n");
    if (!game) return;
    Node *n = game->itemList.head;
    int idx = 1;
    if (!n) { printf("(empty)\n"); return; }
    while (n) {
        Item *it = (Item *) n->value;
        if (it->type == HEALTH_POTION) printf("%d: %s x%d (heal %d)\n", idx, it->name, it->qty, it->effectValue);
        else printf("%d: %s (val %d eff %d)\n", idx, it->name, it->value, it->effectValue);
        n = n->next; idx++;
    }
    printf("----------------------\n");
}
