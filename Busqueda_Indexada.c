#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PALABRAS 100
#define MAX_LEN      50

/* Estructura que representa una entrada en el índice.
   Guarda el rango [inicio, fin] del arreglo donde
   viven las palabras que empiezan con esa letra. */
typedef struct {
    char letra;
    int  inicio;
    int  fin;
} EntradaIndice;

/* Construye el índice recorriendo el arreglo UNA sola vez.
   Asume que las palabras ya están ordenadas alfabéticamente. */
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
            /* Nueva letra: cierra la entrada anterior y abre una nueva */
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

/* Función que busca la entrada del índice que corresponde a una letra.
   Retorna el puntero a la entrada o NULL si no existe. */
EntradaIndice *buscar_en_indice(EntradaIndice indice[], int tam, char letra)
{
    letra = tolower(letra);
    for (int i = 0; i < tam; i++) {
        if (indice[i].letra == letra)
            return &indice[i];
    }
    return NULL;
}

0 -> a, 0, 9 -> 2 3456ert
1 -> b, 10, 12 -> 3

/* Búsqueda indexada completa.
   1. Consulta el índice con la inicial de la clave.
   2. Recorre SOLO el sub-rango correspondiente. */
int busqueda_indexada(char palabras[][MAX_LEN], int n, EntradaIndice indice[], int tam_indice, const char *clave)
{
    EntradaIndice *entrada = buscar_en_indice(indice, tam_indice, clave[0]);
    
    if (!entrada) {
        printf("  No hay palabras con la letra '%c'\n", clave[0]);
        return -1;
    }

    printf("  Índice: letra='%c', rango [%d..%d]\n",
           entrada->letra, entrada->inicio, entrada->fin);

    /* Búsqueda lineal solo dentro del rango */
    for (int i = entrada->inicio; i <= entrada->fin; i++) {
        if (strcmp(palabras[i], clave) == 0) {
            printf("  Encontrado en posición %d\n", i);
            return i;
        }
    }

    printf("  '%s' no encontrado en el rango\n", clave);
    return -1;
}

int main(void)
{
    /* Arreglo ordenado alfabéticamente */
    char palabras[][MAX_LEN] = {
        "abeja", "águila",
        "ballena", "búho",
        "caballo", "ciervo",
        "delfín",
        "elefante"
    };
    int n = 8;

    /* Construir el índice */
    EntradaIndice indice[26];
    int tam_indice = 0;
    construir_indice(palabras, n, indice, &tam_indice);

    printf("=== Índice construido ===\n");
    for (int i = 0; i < tam_indice; i++) {
        printf("  '%c' -> posiciones [%d..%d]\n",
               indice[i].letra, indice[i].inicio, indice[i].fin);
    }

    printf("\n=== Búsquedas ===\n");
    const char *claves[] = {"búho", "ciervo", "zorro"};
    for (int i = 0; i < 3; i++) {
        printf("\nBuscando: '%s'\n", claves[i]);
        busqueda_indexada(palabras, n, indice, tam_indice, claves[i]);
    }

    return 0;
}