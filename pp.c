#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#define CLEAR_CMD "cls"
#else
#define CLEAR_CMD "clear"
#endif

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

#define MAX_RESTAURANTS 5
#define MAX_MENU 10
#define MAX_INGREDIENTS 10
#define MAX_HISTORY 100
#define ORDER_FILE "orders.txt"

typedef struct {
    char name[50];
    char menu[MAX_MENU][50];
    int menuCount;
} Restaurant;

typedef struct {
    char restaurant[50];
    char orderDesc[200];
    char location[100];
} Order;

Restaurant restaurants[MAX_RESTAURANTS] = {
    {"Pizza Palace", {"Margherita", "Pepperoni", "Vegetarian", "BBQ Chicken"}, 4},
    {"Burger House", {"Classic Burger", "Cheeseburger", "Bacon Burger", "Veggie Burger"}, 4},
    {"Sushi World", {"California Roll", "Tuna Nigiri", "Salmon Sashimi", "Veggie Roll"}, 4}
};
int restaurantCount = 3;

void clearScreen() {
    system(CLEAR_CMD);
}

void pause() {
    printf("\nApasa Enter pentru a continua...");
    getchar();
}

void toLowerStr(char *s) {
    for (int i = 0; s[i]; i++) s[i] = tolower(s[i]);
}

void initHistory() {
    FILE *f = fopen(ORDER_FILE, "a");
    if (f) fclose(f);
}

void saveOrder(const Order *o) {
    FILE *f = fopen(ORDER_FILE, "a");
    if (!f) return;
    fprintf(f, "%s|%s|%s\n", o->restaurant, o->orderDesc, o->location);
    fclose(f);
}

void viewHistory() {
    clearScreen();
    printf("--- Istoric comenzi ---\n\n");
    FILE *f = fopen(ORDER_FILE, "r");
    if (!f) {
        printf("%sNu exista comenzi.%s\n", RED, RESET);
        return;
    }
    char line[256];
    int idx = 1;
    while (fgets(line, sizeof(line), f)) {
        char *r = strtok(line, "|");
        char *d = strtok(NULL, "|");
        char *l = strtok(NULL, "|\n");
        printf("%d. Restaurant: %s%s%s; Comanda: %s%s%s; Locatie: %s%s%s\n",
            idx++, CYAN, r, RESET, GREEN, d, RESET, YELLOW, l, RESET);
    }
    fclose(f);
}

void customizePizza(char *desc) {
    char size[20];
    char ingredient[50];
    char list[MAX_INGREDIENTS][50];
    int count = 0;
    printf("%sDimensiune pizza (Small/Medium/Large):%s ", YELLOW, RESET);
    fgets(size, sizeof(size), stdin);
    size[strcspn(size, "\n")] = '\0';
    while (count < MAX_INGREDIENTS) {
        printf("%sAdauga ingredient (blank pt. terminare):%s ", MAGENTA, RESET);
        fgets(ingredient, sizeof(ingredient), stdin);
        ingredient[strcspn(ingredient, "\n")] = '\0';
        if (strlen(ingredient) == 0) break;
        strcpy(list[count++], ingredient);
    }
    strcpy(desc, size);
    strcat(desc, " pizza cu ");
    for (int i = 0; i < count; i++) {
        strcat(desc, list[i]);
        if (i < count - 1) strcat(desc, ", ");
    }
    if (count == 0) strcat(desc, "fara ingrediente");
}

void placeOrder() {
    clearScreen();
    printf("--- Plasare comanda ---\n\n");
    for (int i = 0; i < restaurantCount; i++) {
        printf("%d. %s%s%s\n", i + 1, CYAN, restaurants[i].name, RESET);
    }
    printf("%sSelecteaza restaurantul (1-%d):%s ", YELLOW, restaurantCount, RESET);
    int choice;
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > restaurantCount) {
        getchar();
        printf("%sOptiune invalida.%s\n", RED, RESET);
        return;
    }
    getchar();
    Restaurant *r = &restaurants[choice - 1];
    printf("\nMeniu %s%s%s:\n", CYAN, r->name, RESET);
    for (int i = 0; i < r->menuCount; i++) {
        printf("%d. %s%s%s\n", i + 1, GREEN, r->menu[i], RESET);
    }
    printf("%d. %sPersonalizeaza pizza%s\n", r->menuCount + 1, MAGENTA, RESET);
    printf("%sAlege optiunea (1-%d):%s ", YELLOW, r->menuCount + 1, RESET);
    int item;
    if (scanf("%d", &item) != 1 || item < 1 || item > r->menuCount + 1) {
        getchar();
        printf("%sOptiune invalida.%s\n", RED, RESET);
        return;
    }
    getchar();
    Order o;
    strcpy(o.restaurant, r->name);
    if (item == r->menuCount + 1) {
        customizePizza(o.orderDesc);
    } else {
        strcpy(o.orderDesc, r->menu[item - 1]);
    }
    printf("%s\nLocatia livrarii (ex: Strada Exemplu 12):%s ", BLUE, RESET);
    fgets(o.location, sizeof(o.location), stdin);
    o.location[strcspn(o.location, "\n")] = '\0';
    if (strlen(o.location) == 0) {
        printf("%sLocatia nu poate fi goala!%s\n", RED, RESET);
        return;
    }
    saveOrder(&o);
    printf("\n%sComanda plasata cu succes!%s\n", GREEN, RESET);
}

int main() {
    initHistory();
    while (1) {
        clearScreen();
        printf("--- %sSistem Food Delivery%s ---\n", BLUE, RESET);
        printf("1. Plasare comanda\n");
        printf("2. Vizualizare istoric\n");
        printf("3. Iesire\n");
        printf("%sAlege optiunea:%s ", YELLOW, RESET);
        int opt;
        if (scanf("%d", &opt) != 1) {
            getchar();
            continue;
        }
        getchar();
        switch (opt) {
            case 1: placeOrder(); pause(); break;
            case 2: viewHistory(); pause(); break;
            case 3: exit(0);
            default: printf("%sOptiune invalida.%s\n", RED, RESET); pause(); break;
        }
    }
    return 0;
}
