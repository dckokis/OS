#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node {
    char *string;
    struct node *next;
} node;

node *initList() {
    node *list = (node *) malloc(sizeof(node));
    if (list) {
        list->string = NULL;
        list->next = NULL;
    }
    return list;
}

void dispose(node *list) {
    if (!list) {
        return;
    }
    node *current_list = list;
    while (current_list) {
        if (current_list->string) {
            free(current_list->string);
        }
        node *to_delete = current_list;
        current_list = current_list->next;
        free(to_delete);
    }
}

void printStrings(node *list) {
    if (!list) {
        return;
    }
    node *current_list = list;
    while (current_list) {
        printf("%s\n", current_list->string);
        current_list = current_list->next;
    }
}

int append(node **list, char *string) {
    if (!*list) {
        *list = initList();
        if (!*list) {
            perror("Node has not been inited\n");
            return -1;
        }
        (*list)->string = string;
        return 1;
    } else {
        node *last_list = *list;
        while (last_list->next) {
            last_list = last_list->next;
        }
        node *new_item = initList();
        if (!new_item) {
            perror("Node cannot add more strings\n");
            return -1;
        }
        new_item->string = string;
        last_list->next = new_item;
        return 1;
    }
}

int main() {
    char string[255];
    node *string_list = NULL;
    printf("Enter your strings. To end entering data type \".\" :\n");
    while (*(fgets(string, 255, stdin)) != '.') {
        size_t string_lenght = strlen(string);
        char *temp = (char *) malloc(string_lenght);
        if (temp) {
            memcpy(temp, string, string_lenght);
            if (temp[string_lenght - 1] == '\n') {
                temp[string_lenght - 1] = '\0';
            }
            if (append(&string_list, temp) == -1) {
                printStrings(string_list);
                dispose(string_list);
            }
        }
    }
    printStrings(string_list);
    dispose(string_list);
    return 0;
}