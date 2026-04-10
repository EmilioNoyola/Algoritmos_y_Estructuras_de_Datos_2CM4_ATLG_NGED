#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_LEN      50
#define MAX_PALABRAS 300002
#define REPETICIONES 1000   

typedef struct {
    char letra;
    int  inicio;
    int  fin;
} EntradaIndice;

int construir_indice(char palabras[][MAX_LEN], int n, EntradaIndice indice[], int *tam_indice)
{
    *tam_indice = 0;
    if (n == 0) return 0;

    char letra_actual = tolower(palabras[0][0]);
    indice[0].letra  = letra_actual;
    indice[0].inicio = 0;
    indice[0].fin    = 0;
    *tam_indice = 1;

    for (int i = 1; i < n; i++) {
        char c = tolower(palabras[i][0]);
        if (c != letra_actual) {
            indice[*tam_indice - 1].fin = i - 1;
            indice[*tam_indice].letra   = c;
            indice[*tam_indice].inicio  = i;
            indice[*tam_indice].fin     = i;
            (*tam_indice)++;
            letra_actual = c;
        } else {
            indice[*tam_indice - 1].fin = i;
        }
    }
    return 1;
}

EntradaIndice *buscar_en_indice(EntradaIndice indice[], int tam, char letra)
{
    letra = tolower(letra);
    for (int i = 0; i < tam; i++)
        if (indice[i].letra == letra)
            return &indice[i];
    return NULL;
}

int busqueda_secuencial(char palabras[][MAX_LEN], int n, const char *clave)
{
    for (int i = 0; i < n; i++)
        if (strcmp(palabras[i], clave) == 0)
            return i;
    return -1;
}

// Utilizar variables incluidas en clase.
int busqueda_binaria(char palabras[][MAX_LEN], int n, const char *clave)
{
    int izq = 0, der = n - 1;
    while (izq <= der) {
        int mid = izq + (der - izq) / 2;
        int cmp = strcmp(palabras[mid], clave);
        if (cmp == 0) return mid;
        if (cmp < 0)  izq = mid + 1;
        else          der = mid - 1;
    }
    return -1;
}

int busqueda_indexada_secuencial(char palabras[][MAX_LEN], EntradaIndice indice[], int tam_indice, const char *clave)
{
    EntradaIndice *e = buscar_en_indice(indice, tam_indice, clave[0]);
    if (!e) return -1;
    for (int i = e->inicio; i <= e->fin; i++)
        if (strcmp(palabras[i], clave) == 0)
            return i;
    return -1;
}

// Utilizar variables incluidas en clase.
int busqueda_indexada_binaria(char palabras[][MAX_LEN], EntradaIndice indice[], int tam_indice, const char *clave)
{
    EntradaIndice *e = buscar_en_indice(indice, tam_indice, clave[0]);
    if (!e) return -1;
    int izq = e->inicio, der = e->fin;
    while (izq <= der) {
        int mid = izq + (der - izq) / 2;
        int cmp = strcmp(palabras[mid], clave);
        if (cmp == 0) return mid;
        if (cmp < 0)  izq = mid + 1;
        else          der = mid - 1;
    }
    return -1;
}

double medir_secuencial(char palabras[][MAX_LEN], int n, const char *clave)
{
    clock_t ini = clock();
    for (int r = 0; r < REPETICIONES; r++)
        busqueda_secuencial(palabras, n, clave);
    clock_t fin = clock();
    return ((double)(fin - ini) / CLOCKS_PER_SEC * 1e6) / REPETICIONES;
}

double medir_binaria(char palabras[][MAX_LEN], int n, const char *clave)
{
    clock_t ini = clock();
    for (int r = 0; r < REPETICIONES; r++)
        busqueda_binaria(palabras, n, clave);
    clock_t fin = clock();
    return ((double)(fin - ini) / CLOCKS_PER_SEC * 1e6) / REPETICIONES;
}

double medir_indexada_sec(char palabras[][MAX_LEN], int n, EntradaIndice indice[], int tam_indice, const char *clave)
{
    clock_t ini = clock();
    for (int r = 0; r < REPETICIONES; r++)
        busqueda_indexada_secuencial(palabras, indice, tam_indice, clave);
    clock_t fin = clock();
    return ((double)(fin - ini) / CLOCKS_PER_SEC * 1e6) / REPETICIONES;
}

double medir_indexada_bin(char palabras[][MAX_LEN], int n, EntradaIndice indice[], int tam_indice, const char *clave)
{
    clock_t ini = clock();
    for (int r = 0; r < REPETICIONES; r++)
        busqueda_indexada_binaria(palabras, indice, tam_indice, clave);
    clock_t fin = clock();
    return ((double)(fin - ini) / CLOCKS_PER_SEC * 1e6) / REPETICIONES;
}

void imprimir_tabla(double t1, double t2, double t3, double t4)
{
    printf("\n+----------------------------------+--------------+\n");
    printf("| Metodo                           | Tiempo (us)  |\n");
    printf("+----------------------------------+--------------+\n");
    printf("| Secuencial directa               | %12.4f |\n", t1);
    printf("| Binaria directa                  | %12.4f |\n", t2);
    printf("| Indexada + secuencial            | %12.4f |\n", t3);
    printf("| Indexada + binaria               | %12.4f |\n", t4);
    printf("+----------------------------------+--------------+\n");
    printf("(promedio de %d repeticiones)\n", REPETICIONES);
}

char palabras[MAX_PALABRAS][MAX_LEN];

int main(void)
{
    int n = 0;

    FILE *archivo = fopen("SALIDA.txt", "r");
    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo\n");
        return 1;
    }

    while (n < MAX_PALABRAS && fgets(palabras[n], MAX_LEN, archivo) != NULL) {
        palabras[n][strcspn(palabras[n], "\n")] = '\0';
        if (strlen(palabras[n]) > 0)
            n++;
    }

    fclose(archivo);
    printf("Se leyeron %d palabras del archivo\n", n);

    EntradaIndice indice[26];
    int tam_indice = 0;
    construir_indice(palabras, n, indice, &tam_indice);

    printf("=== Indice construido ===\n");
    for (int i = 0; i < tam_indice; i++)
    printf("  '%c' -> posiciones [%d..%d]\n", indice[i].letra, indice[i].inicio, indice[i].fin);

    char clave[MAX_LEN];
    printf("\nQue clave deseas buscar?: ");
    fgets(clave, MAX_LEN, stdin);
    clave[strcspn(clave, "\n")] = '\0';

    int opcion;
    printf("\nTipo de busqueda:\n");
    printf("  1. Busqueda secuencial directa\n");
    printf("  2. Busqueda binaria directa\n");
    printf("  3. Busqueda indexada con secuencial\n");
    printf("  4. Busqueda indexada con binaria\n");
    printf("  5. Comparacion de todas\n");
    printf("Opcion: ");
    scanf("%d", &opcion);

    printf("\nBuscando: '%s'\n\n", clave);

    int pos;
    double t;

    switch (opcion) {
        case 1:
            t   = medir_secuencial(palabras, n, clave);
            pos = busqueda_secuencial(palabras, n, clave);
            printf("--- Secuencial directa ---\n");
            printf("  Resultado: %s\n", pos >= 0 ? "encontrado" : "no encontrado");
            if (pos >= 0) printf("  Posicion : %d\n", pos);
            printf("  Tiempo   : %.4f us\n", t);
            break;

        case 2:
            t   = medir_binaria(palabras, n, clave);
            pos = busqueda_binaria(palabras, n, clave);
            printf("--- Binaria directa ---\n");
            printf("  Resultado: %s\n", pos >= 0 ? "encontrado" : "no encontrado");
            if (pos >= 0) printf("  Posicion : %d\n", pos);
            printf("  Tiempo   : %.4f us\n", t);
            break;

        case 3:
            t   = medir_indexada_sec(palabras, n, indice, tam_indice, clave);
            pos = busqueda_indexada_secuencial(palabras, indice, tam_indice, clave);
            printf("--- Indexada + secuencial ---\n");
            printf("  Resultado: %s\n", pos >= 0 ? "encontrado" : "no encontrado");
            if (pos >= 0) printf("  Posicion : %d\n", pos);
            printf("  Tiempo   : %.4f us\n", t);
            break;

        case 4:
            t   = medir_indexada_bin(palabras, n, indice, tam_indice, clave);
            pos = busqueda_indexada_binaria(palabras, indice, tam_indice, clave);
            printf("--- Indexada + binaria ---\n");
            printf("  Resultado: %s\n", pos >= 0 ? "encontrado" : "no encontrado");
            if (pos >= 0) printf("  Posicion : %d\n", pos);
            printf("  Tiempo   : %.4f us\n", t);
            break;

        case 5: {
            double t1 = medir_secuencial(palabras, n, clave);
            double t2 = medir_binaria(palabras, n, clave);
            double t3 = medir_indexada_sec(palabras, n, indice, tam_indice, clave);
            double t4 = medir_indexada_bin(palabras, n, indice, tam_indice, clave);
            pos = busqueda_secuencial(palabras, n, clave);
            printf("Resultado: %s", pos >= 0 ? "encontrado" : "no encontrado");
            if (pos >= 0) printf(" en posicion %d", pos);
            printf("\n");
            imprimir_tabla(t1, t2, t3, t4);
            break;
        }

        default:
            printf("Opcion invalida\n");
            break;
    }

    return 0;
}