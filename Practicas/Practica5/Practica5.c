#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESET    "\033[0m"
#define BOLD     "\033[1m"
#define RED      "\033[31m"
#define GREEN    "\033[32m"
#define YELLOW   "\033[33m"
#define CYAN     "\033[36m"
#define WHITE    "\033[37m"
#define BG_GREEN "\033[42m"
#define BG_RED   "\033[41m"
#define BG_BLUE  "\033[44m"

#define PARED     1
#define LIBRE     0
#define CAMINO    2
#define IMPOSIBLE 3

#define MAX_FILAS 100
#define MAX_COLS  100

int laberinto[MAX_FILAS][MAX_COLS];
int filas, cols;

int leer_laberinto(const char *archivo)
{
    FILE *f = fopen(archivo, "r");
    if (!f) {
        printf("%s  Error: no se pudo abrir '%s'%s\n", RED, archivo, RESET);
        return 0;
    }

    filas = 0;
    char linea[4096];

    while (fgets(linea, sizeof(linea), f) && filas < MAX_FILAS) {
        cols = 0;
        char *token = strtok(linea, ",\n\r");
        while (token && cols < MAX_COLS) {
            laberinto[filas][cols++] = atoi(token);
            token = strtok(NULL, ",\n\r");
        }
        filas++;
    }

    fclose(f);
    return 1;
}

/* Busca la entrada y salida en los bordes */
int buscar_entrada_salida(int *ef, int *ec, int *sf, int *sc)
{
    int encontrados = 0;

    for (int j = 0; j < cols; j++) {
        if (laberinto[0][j] == LIBRE) {
            if (encontrados == 0) { *ef = 0; *ec = j; }
            else                  { *sf = 0; *sc = j; }
            encontrados++;
        }
    }

    for (int j = 0; j < cols; j++) {
        if (laberinto[filas - 1][j] == LIBRE) {
            if (encontrados == 0) { *ef = filas - 1; *ec = j; }
            else                  { *sf = filas - 1; *sc = j; }
            encontrados++;
        }
    }

    for (int i = 1; i < filas - 1; i++) {
        if (laberinto[i][0] == LIBRE) {
            if (encontrados == 0) { *ef = i; *ec = 0; }
            else                  { *sf = i; *sc = 0; }
            encontrados++;
        }
    }

    for (int i = 1; i < filas - 1; i++) {
        if (laberinto[i][cols - 1] == LIBRE) {
            if (encontrados == 0) { *ef = i; *ec = cols - 1; }
            else                  { *sf = i; *sc = cols - 1; }
            encontrados++;
        }
    }

    return encontrados;
}

/* Imprime el laberinto con colores según el estado de cada celda */
void imprimir_laberinto(int entrada_f, int entrada_c, int salida_f,  int salida_c)
{
    for (int i = 0; i < filas; i++) {
        printf("  ");
        for (int j = 0; j < cols; j++) {

            if (i == entrada_f && j == entrada_c) {
                printf("%s%s E %s", BOLD, BG_BLUE,  RESET);
            } else if (i == salida_f && j == salida_c) {
                printf("%s%s S %s", BOLD, BG_GREEN, RESET);
            } else {
                switch (laberinto[i][j]) {
                    case PARED:
                        printf("%s%s   %s", BOLD, BG_RED,   RESET);
                        break;
                    case LIBRE:
                        printf("%s . %s", WHITE, RESET);
                        break;
                    case CAMINO:
                        printf("%s%s * %s", BOLD, BG_GREEN, RESET);
                        break;
                    case IMPOSIBLE:
                        printf("%s x %s", RED, RESET);
                        break;
                }
            }
        }
        printf("\n");
    }
}

/* Backtracking */
int backtracking(int f, int c, int salida_f, int salida_c)
{
    if (f == salida_f && c == salida_c) {
        laberinto[f][c] = CAMINO;
        return 1;
    }

    if (f < 0 || f >= filas || c < 0 || c >= cols) return 0;
    if (laberinto[f][c] != LIBRE)                  return 0;

    laberinto[f][c] = CAMINO;

    int df[] = {  1,  0, -1,  0 };
    int dc[] = {  0,  1,  0, -1 };

    for (int d = 0; d < 4; d++) {
        if (backtracking(f + df[d], c + dc[d], salida_f, salida_c))
            return 1;
    }

    laberinto[f][c] = IMPOSIBLE;
    return 0;
}

int main(void)
{
    char archivo[256];

    printf("\n%s%s%s LABERINTO - BACKTRACKING %s\n", BOLD, BG_GREEN, WHITE, RESET);

    printf("%s  Archivos disponibles:%s\n\n", BOLD, RESET);
    printf("  %s[1]%s laberinto1.txt  (41 x 41)\n", CYAN, RESET);
    printf("  %s[2]%s laberinto2.txt  (29 x 29)\n", CYAN, RESET);
    printf("  %s[3]%s laberinto3.txt  (41 x 61)\n", CYAN, RESET);
    printf("  %s[4]%s Ingresar nombre manualmente\n\n", CYAN, RESET);

    int opcion;
    printf("%s  Seleccione:%s ", BOLD, RESET);
    scanf("%d", &opcion);

    switch (opcion) {
        case 1: strcpy(archivo, "laberinto1.txt"); break;
        case 2: strcpy(archivo, "laberinto2.txt"); break;
        case 3: strcpy(archivo, "laberinto3.txt"); break;
        default:
            printf("%s  Nombre del archivo:%s ", BOLD, RESET);
            scanf("%s", archivo);
            break;
    }

    if (!leer_laberinto(archivo)) return 1;

    printf("\n%s  Laberinto %s%s %scargado: %s %d filas x %d columnas\n", GREEN, RESET, archivo, GREEN, RESET, filas, cols);

    int entrada_f, entrada_c, salida_f, salida_c;
    int encontrados = buscar_entrada_salida(&entrada_f, &entrada_c, &salida_f,  &salida_c);

    if (encontrados < 2) {
        printf("%s\n  Error: no se encontraron entrada y salida en los bordes.%s\n", RED, RESET);
        return 1;
    }

    printf("\n%s  Entrada:%s fila %d, columna %d\n", GREEN, RESET, entrada_f, entrada_c);
    printf("%s  Salida:%s fila %d, columna %d\n", GREEN, RESET, salida_f, salida_c);

    printf("\n%s%s%s LABERINTO INICIAL %s\n\n", BOLD, BG_GREEN, WHITE, RESET);

    printf("  %s E %s Entrada    ", BG_BLUE,  RESET);
    printf("  %s S %s Salida\n",   BG_GREEN, RESET);
    printf("  %s   %s Pared      ", BG_RED,   RESET);
    printf("  %s . %s Libre\n\n",  WHITE,    RESET);

    imprimir_laberinto(entrada_f, entrada_c, salida_f, salida_c);

    int resuelto = backtracking(entrada_f, entrada_c, salida_f, salida_c);

    if (resuelto) {
        printf("\n%s%s%s LABERINTO RESUELTO %s\n\n", BOLD, BG_GREEN, WHITE, RESET);

        printf("  %s E %s Entrada    ", BG_BLUE,  RESET);
        printf("  %s S %s Salida / Camino\n", BG_GREEN, RESET);
        printf("  %s   %s Pared      ", BG_RED,   RESET);
        printf("  %s . %s Libre\n\n",  WHITE,    RESET);

        imprimir_laberinto(entrada_f, entrada_c, salida_f, salida_c);

        int pasos = 0;
        for (int i = 0; i < filas; i++)
            for (int j = 0; j < cols; j++)
                if (laberinto[i][j] == CAMINO) pasos++;

        printf("\n%s  Solucion encontrada%s\n", GREEN, RESET);
        printf("  Entrada : fila %d, columna %d\n", entrada_f, entrada_c);
        printf("  Salida  : fila %d, columna %d\n", salida_f,  salida_c);
        printf("  Pasos   : %s%d%s celdas en el camino\n\n", GREEN, pasos, RESET);

    } else {
        printf("\n%s%s%s SIN SOLUCION %s\n", BOLD, BG_RED, WHITE, RESET);
        printf("%s  El laberinto no tiene solucion%s\n", RED, RESET);
        printf("  desde (%d, %d) hasta (%d, %d)\n\n", entrada_f, entrada_c, salida_f, salida_c);
    }

    return 0;
}