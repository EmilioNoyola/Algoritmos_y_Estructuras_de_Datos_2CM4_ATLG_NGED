#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_LEN      50
#define MAX_PALABRAS 300000
#define REPETICIONES 1000   

typedef struct {
    char letra;
    int  inicio;
    int  fin;
} LetraInicial;

int construir_indice(char **palabras, int n, LetraInicial indice[], int *tam_indice)
{
    *tam_indice = 0;
    if (n == 0) return 0;

    char letra_actual = tolower(palabras[0][0]);
    indice[0].letra  = letra_actual;
    indice[0].inicio = 0;
    indice[0].fin    = 0;
    *tam_indice = 1;

    for (int i = 1; i < n; i++) {
        char letra_inicial = tolower(palabras[i][0]);
        if (letra_inicial != letra_actual) {
            indice[*tam_indice - 1].fin = i - 1;
            indice[*tam_indice].letra   = letra_inicial;
            indice[*tam_indice].inicio  = i;
            indice[*tam_indice].fin     = i;
            (*tam_indice)++;
            letra_actual = letra_inicial;
        } else {
            indice[*tam_indice - 1].fin = i;
        }
    }
    return 1;
}

LetraInicial *buscar_en_indice(LetraInicial indice[], int tam, char letra)
{
    letra = tolower(letra);
    for (int i = 0; i < tam; i++) {
        if (indice[i].letra == letra) {
            return &indice[i];
        }
    }    
    return NULL;
}

int busqueda_secuencial(char **palabras, int n, const char *clave)
{
    for (int i = 0; i < n; i++) {
        if (strcmp(palabras[i], clave) == 0) {
            return i;
        } 
    }  
    return -1;
}

int busqueda_binaria(char **palabras, int n, const char *clave)
{
    int izq = 0, der = n - 1;

    while (izq <= der) {
        int mid = izq + (der - izq) / 2;
        int cmp = strcmp(palabras[mid], clave);
        if (cmp == 0) {
           return mid; 
        } else if (cmp < 0) {
            izq = mid + 1;
        } else {
            der = mid - 1;
        }
    }
    return -1;
}

int busqueda_indexada_secuencial(char **palabras, LetraInicial indice[], int tam_indice, const char *clave)
{
    LetraInicial *entrada = buscar_en_indice(indice, tam_indice, clave[0]);
    if (!entrada) return -1;

    for (int i = entrada->inicio; i <= entrada->fin; i++) {
        if (strcmp(palabras[i], clave) == 0) {
            return i;
        }
    }
    return -1;
}

int busqueda_indexada_binaria(char **palabras, LetraInicial indice[], int tam_indice, const char *clave)
{
    LetraInicial *entrada = buscar_en_indice(indice, tam_indice, clave[0]);
    if (!entrada) return -1;

    int izq = entrada->inicio, der = entrada->fin;

    while (izq <= der) {
        int mid = izq + (der - izq) / 2;
        int cmp = strcmp(palabras[mid], clave);
        if (cmp == 0) {
            return mid;  
        } else if (cmp < 0) {
            izq = mid + 1; 
        } else {
            der = mid - 1; 
        }
    }
    return -1;
}

double medir_tiempo_secuencial(char **palabras, int n, const char *clave)
{
    clock_t ini = clock();
    for (int r = 0; r < REPETICIONES; r++) {
        busqueda_secuencial(palabras, n, clave);
    } 
    clock_t fin = clock();

    return ((double)(fin - ini) / CLOCKS_PER_SEC * 1e6) / REPETICIONES;
}

double medir_tiempo_binaria(char **palabras, int n, const char *clave)
{
    clock_t ini = clock();
    for (int r = 0; r < REPETICIONES; r++) {
        busqueda_binaria(palabras, n, clave);
    }
    clock_t fin = clock();

    return ((double)(fin - ini) / CLOCKS_PER_SEC * 1e6) / REPETICIONES;
}

double medir_tiempo_indexada_sec(char **palabras, LetraInicial indice[], int tam_indice, const char *clave)
{
    clock_t ini = clock();
    for (int r = 0; r < REPETICIONES; r++) {
        busqueda_indexada_secuencial(palabras, indice, tam_indice, clave);
    }  
    clock_t fin = clock();

    return ((double)(fin - ini) / CLOCKS_PER_SEC * 1e6) / REPETICIONES;
}

double medir_tiempo_indexada_bin(char **palabras, LetraInicial indice[], int tam_indice, const char *clave)
{
    clock_t ini = clock();
    for (int r = 0; r < REPETICIONES; r++) {
        busqueda_indexada_binaria(palabras, indice, tam_indice, clave); 
    }
    clock_t fin = clock();
    
    return ((double)(fin - ini) / CLOCKS_PER_SEC * 1e6) / REPETICIONES;
}

void imprimir_tabla(double t1, double t2, double t3, double t4)
{
    printf("\n+----------------------------------+--------------+\n");
    printf("| Tipo de Busqueda                 |  Tiempo (us) |\n");
    printf("+----------------------------------+--------------+\n");
    printf("| Secuencial directa               | %12.4f |\n", t1);
    printf("| Binaria directa                  | %12.4f |\n", t2);
    printf("| Indexada + secuencial            | %12.4f |\n", t3);
    printf("| Indexada + binaria               | %12.4f |\n", t4);
    printf("+----------------------------------+--------------+\n");
    printf("(promedio de %d repeticiones)\n", REPETICIONES);
}
    
int main(void)
{
    int n = 0;
    FILE *archivo = fopen("SALIDA.txt", "r");
    char linea[MAX_LEN];
    while (fgets(linea, MAX_LEN, archivo) != NULL) {
        if (strlen(linea) > 1) {
            n++;
        }
    }
    rewind(archivo); 

    char **palabras = malloc(n * sizeof(char *));
    if (palabras == NULL) {
        printf("Error: sin memoria\n");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        palabras[i] = malloc(MAX_LEN * sizeof(char));
        if (palabras[i] == NULL) {
            printf("Error: sin memoria\n");
            return 1;
        }
    }
        
    int leidas = 0;
    while (leidas < n && fgets(palabras[leidas], MAX_LEN, archivo) != NULL) {
        palabras[leidas][strcspn(palabras[leidas], "\n")] = '\0';
        if (strlen(palabras[leidas]) > 0)
            leidas++;
    }
    fclose(archivo);
    printf("Se leyeron %d palabras del archivo\n", n);

    LetraInicial *indice = malloc(26 * sizeof(LetraInicial));
    int tam_indice = 0;
    construir_indice(palabras, n, indice, &tam_indice);
    indice = realloc(indice, tam_indice * sizeof(LetraInicial));

    printf("=== Indice ===\n");
    for (int i = 0; i < tam_indice; i++) {
        printf("  '%c' -> posiciones [%d..%d]\n", indice[i].letra, indice[i].inicio, indice[i].fin);   
    }
    
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

    int posicion;
    double tiempo;

    switch (opcion) {
        case 1:
            tiempo   = medir_tiempo_secuencial(palabras, n, clave);
            posicion = busqueda_secuencial(palabras, n, clave);
            printf("--- Busqueda Secuencial directa ---\n");
            printf("  Resultado: %s\n", posicion >= 0 ? "Encontrado" : "No encontrado");

            if (posicion >= 0) {
                printf("  Posicion : %d\n", posicion); 
            }
             
            printf("  Tiempo   : %.4f us\n", tiempo);
            break;

        case 2:
            tiempo   = medir_tiempo_binaria(palabras, n, clave);
            posicion = busqueda_binaria(palabras, n, clave);
            printf("--- Busqueda Binaria directa ---\n");
            printf("  Resultado: %s\n", posicion >= 0 ? "Encontrado" : "No encontrado");

            if (posicion >= 0) {
                printf("  Posicion : %d\n", posicion);
            } 

            printf("  Tiempo   : %.4f us\n", tiempo);
            break;

        case 3:
            tiempo   = medir_tiempo_indexada_sec(palabras, indice, tam_indice, clave);
            posicion = busqueda_indexada_secuencial(palabras, indice, tam_indice, clave);
            printf("--- Busqueda Indexada + secuencial ---\n");
            printf("  Resultado: %s\n", posicion >= 0 ? "Encontrado" : "No encontrado");

            if (posicion >= 0) {
                printf("  Posicion : %d\n", posicion);
            } 

            printf("  Tiempo   : %.4f us\n", tiempo);
            break;

        case 4:
            tiempo   = medir_tiempo_indexada_bin(palabras, indice, tam_indice, clave);
            posicion = busqueda_indexada_binaria(palabras, indice, tam_indice, clave);
            printf("--- Busqueda Indexada + binaria ---\n");
            printf("  Resultado: %s\n", posicion >= 0 ? "Encontrado" : "No encontrado");

            if (posicion >= 0) {
                printf("  Posicion : %d\n", posicion);
            } 
            
            printf("  Tiempo   : %.4f us\n", tiempo);
            break;

        case 5: {
            double t1 = medir_tiempo_secuencial(palabras, n, clave);
            double t2 = medir_tiempo_binaria(palabras, n, clave);
            double t3 = medir_tiempo_indexada_sec(palabras, indice, tam_indice, clave);
            double t4 = medir_tiempo_indexada_bin(palabras, indice, tam_indice, clave);
            posicion = busqueda_indexada_binaria(palabras, indice, tam_indice, clave);
            printf("Resultado: %s", posicion >= 0 ? "Encontrado" : "No encontrado");

            if (posicion >= 0) {
                printf(" en posicion %d", posicion);
            }

            printf("\n");
            imprimir_tabla(t1, t2, t3, t4);
            break;
        }

        default:
            printf("Opcion invalida\n");
            break;
    }

    for (int i = 0; i < n; i++)
        free(palabras[i]);
    free(palabras);

    free(indice);

    return 0;
}