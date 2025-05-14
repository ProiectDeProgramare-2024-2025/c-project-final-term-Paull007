#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#define CLEAR_COMMAND "cls"
#include <windows.h>
#else
#define CLEAR_COMMAND "clear"
#endif

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define RESET   "\033[0m"

#define FILENAME "bikes.txt"
#define HEADER "#GeneratedByBikeApp\n"

void clear_screen() {
    system(CLEAR_COMMAND);
}

void flush_input() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void wait_for_enter() {
    printf("\nApasa Enter pentru a te intoarce la meniul principal...");
    flush_input();
    getchar();
}

typedef struct {
    char name[50];
    int total;
    int available;
} Bike;

Bike bikes[] = {
    {"Mountain", 5, 5},
    {"Road", 3, 3},
    {"Electric", 4, 4}
};

int bikeCount = sizeof(bikes) / sizeof(bikes[0]);

void clean_line(const char *input, char *output) {
    int j = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (isalnum(input[i]) || isspace(input[i])) {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
}

void load_bikes() {
    FILE *fp = fopen(FILENAME, "r");
    if (fp != NULL) {
        char line[256];
        char cleaned[256];
        if (fgets(line, sizeof(line), fp) != NULL) {
            clean_line(line, cleaned);
            char expectedHeader[256];
            clean_line(HEADER, expectedHeader);
            if (strcmp(cleaned, expectedHeader) != 0) {
                fclose(fp);
                return;
            }
        }
        int index = 0;
        while (index < bikeCount && fgets(line, sizeof(line), fp) != NULL) {
            clean_line(line, cleaned);
            char name[50];
            int total, available;
            if (sscanf(cleaned, "%49s %d %d", name, &total, &available) == 3) {
                strcpy(bikes[index].name, name);
                bikes[index].total = total;
                bikes[index].available = available;
            }
            index++;
        }
        fclose(fp);
    } else {
        FILE *fpw = fopen(FILENAME, "w");
        if (fpw != NULL) {
            fprintf(fpw, HEADER);
            for (int i = 0; i < bikeCount; i++) {
                fprintf(fpw, "%s %d %d\n", bikes[i].name, bikes[i].total, bikes[i].available);
            }
            fclose(fpw);
        }
    }
}

void save_bikes() {
    FILE *fp = fopen(FILENAME, "w");
    if (fp != NULL) {
        fprintf(fp, HEADER);
        for (int i = 0; i < bikeCount; i++) {
            fprintf(fp, "%s %d %d\n", bikes[i].name, bikes[i].total, bikes[i].available);
        }
        fclose(fp);
    }
}

void afiseaza_meniu() {
    printf("\n--- Aplicatie de Rezervare Biciclete ---\n");
    printf("1. Rezerva o bicicleta\n");
    printf("2. Vezi starea bicicletelor\n");
    printf("3. Anuleaza o rezervare\n");
    printf("4. Iesire\n");
}

void rezerva_bicicleta() {
    int opt;
    printf("\nSelecteaza tipul de bicicleta:\n");
    for (int i = 0; i < bikeCount; i++) {
        printf("%d. %s%s%s (Disponibile: %s%d%s)\n", i + 1, BLUE, bikes[i].name, RESET, GREEN, bikes[i].available, RESET);
    }
    printf("[INPUT] Alege o optiune (1-%d): ", bikeCount);
    if (scanf("%d", &opt) != 1 || opt < 1 || opt > bikeCount) {
        flush_input();
        printf(RED "[EROARE] Optiune invalida.\n" RESET);
        return;
    }
    flush_input();
    if (bikes[opt - 1].available > 0) {
        bikes[opt - 1].available--;
        char nume[50];
        printf("[INPUT] Introdu numele tau (fara spatii): ");
        scanf("%49s", nume);
        flush_input();
        printf(GREEN "[SUCCES] Rezervarea pentru bicicleta '%s%s%s' a fost inregistrata pentru %s.\n" RESET,
               BLUE, bikes[opt - 1].name, RESET, nume);
        save_bikes();
    } else {
        printf(RED "[EROARE] Nu sunt disponibile biciclete de tipul %s%s%s.\n" RESET, BLUE, bikes[opt - 1].name, RESET);
    }
}

void vezi_rezervari() {
    printf("\nStarea curenta a bicicletelor:\n");
    for (int i = 0; i < bikeCount; i++) {
        int rezervate = bikes[i].total - bikes[i].available;
        printf("%s%s%s: Disponibile: %s%d%s, Rezervate: %s%d%s\n",
               BLUE, bikes[i].name, RESET,
               GREEN, bikes[i].available, RESET,
               RED, rezervate, RESET);
    }
    printf("\nDate citite din fisierul \"%s\":\n", FILENAME);
    FILE *fp = fopen(FILENAME, "r");
    if (fp != NULL) {
        char line[256];
        char cleaned[256];
        while (fgets(line, sizeof(line), fp) != NULL) {
            clean_line(line, cleaned);
            printf("%s\n", cleaned);
        }
        fclose(fp);
    } else {
        printf(RED "[EROARE] Nu se poate citi fisierul cu date.\n" RESET);
    }
}

void anuleaza_rezervare() {
    int opt;
    printf("\nSelecteaza tipul de bicicleta pentru anularea rezervarii:\n");
    for (int i = 0; i < bikeCount; i++) {
        int rezervate = bikes[i].total - bikes[i].available;
        printf("%d. %s%s%s (Rezervate: %s%d%s)\n", i + 1, BLUE, bikes[i].name, RESET, RED, rezervate, RESET);
    }
    printf("[INPUT] Alege o optiune (1-%d): ", bikeCount);
    if (scanf("%d", &opt) != 1 || opt < 1 || opt > bikeCount) {
        flush_input();
        printf(RED "[EROARE] Optiune invalida.\n" RESET);
        return;
    }
    flush_input();
    if (bikes[opt - 1].available < bikes[opt - 1].total) {
        bikes[opt - 1].available++;
        printf(GREEN "[SUCCES] Rezervarea pentru bicicleta '%s%s%s' a fost anulata.\n" RESET, BLUE, bikes[opt - 1].name, RESET);
        save_bikes();
    } else {
        printf(RED "[EROARE] Nu exista rezervari pentru bicicleta %s%s%s de anulat.\n" RESET, BLUE, bikes[opt - 1].name, RESET);
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // optional, doar daca ai caractere Unicode
#endif

    load_bikes();
    int optiune;
    while (1) {
        clear_screen();
        afiseaza_meniu();
        printf("[INPUT] Alege o optiune: ");
        if (scanf("%d", &optiune) != 1) {
            flush_input();
            printf(RED "[EROARE] Optiune invalida. Te rog sa incerci din nou.\n" RESET);
            wait_for_enter();
            continue;
        }
        flush_input();
        switch (optiune) {
            case 1:
                clear_screen();
                printf("--- %sRezervare Biciclete%s ---\n", YELLOW, RESET);
                rezerva_bicicleta();
                wait_for_enter();
                break;
            case 2:
                clear_screen();
                printf("--- %sStare Biciclete%s ---\n", YELLOW, RESET);
                vezi_rezervari();
                wait_for_enter();
                break;
            case 3:
                clear_screen();
                printf("--- %sAnulare Rezervare%s ---\n", YELLOW, RESET);
                anuleaza_rezervare();
                wait_for_enter();
                break;
            case 4:
                printf("La revedere!\n");
                exit(0);
            default:
                printf(RED "[EROARE] Optiune invalida. Te rog sa incerci din nou.\n" RESET);
                wait_for_enter();
        }
    }
    return 0;
}
