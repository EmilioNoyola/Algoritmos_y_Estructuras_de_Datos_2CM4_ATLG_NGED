#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 50
#define MAX_PALABRAS 300002

/* Estructura que representa una entrada en el índice.
   Guarda el rango [inicio, fin] del arreglo donde
   viven las palabras que empiezan con esa letra. */
typedef struct {
    char letra;
    int  inicio;
    int  fin;
} EntradaIndice;

/* Función que construye el índice recorriendo el arreglo UNA sola vez.
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

int busqueda_indexada(char palabras[][MAX_LEN], int n, EntradaIndice indice[], int tam_indice, const char *clave)
{
    EntradaIndice *entrada = buscar_en_indice(indice, tam_indice, clave[0]);
    
    if (!entrada) {
        printf("  No hay palabras con la letra '%c'\n", clave[0]);
        return -1;
    }

    printf("  Indice: letra='%c', rango [%d..%d]\n", entrada->letra, entrada->inicio, entrada->fin);

    /* Búsqueda lineal solo dentro del rango */
    for (int i = entrada->inicio; i <= entrada->fin; i++) {
        if (strcmp(palabras[i], clave) == 0) {
            printf("  Encontrado en posicion %d\n", i);
            return i;
        }
    }

    printf("  '%s' no encontrado en el rango\n", clave);
    return -1;
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
        // Quitar el '\n' que deja fgets al final de cada línea
        palabras[n][strcspn(palabras[n], "\n")] = '\0';
        
        // Solo contar la línea si no está vacía
        if (strlen(palabras[n]) > 0)
            n++;
    }

    fclose(archivo);

    printf("Se leyeron %d palabras del archivo\n", n);

    /* Construir el índice */
    EntradaIndice indice[26];
    int tam_indice = 0;
    construir_indice(palabras, n, indice, &tam_indice);

    printf("=== Indice construido ===\n");
    for (int i = 0; i < tam_indice; i++) {
        printf("  '%c' -> posiciones [%d..%d]\n",
               indice[i].letra, indice[i].inicio, indice[i].fin);
    }

    printf("\n=== Busquedas ===\n");

    char clave[MAX_LEN];

    printf("Que clave deseas buscar?: ");
    fgets(clave, MAX_LEN, stdin);

    clave[strcspn(clave, "\n")] = '\0';

    printf("\nBuscando: '%s'\n", clave);
    busqueda_indexada(palabras, n, indice, tam_indice, clave);

    return 0;
}
