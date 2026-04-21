/*
    Práctica 4: Exploración Exhaustiva
    Alumnos: 
     - Álvarez Tahuilán Luis Gustavo
     - Noyola Gómez Emilio Damian
    Fecha: 20 de abril de 2026
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct { 
    int peso; 
    int valor; 
} Objeto;

typedef struct
{
    int id; 
    int peso_total; 
    int valor_total; 
    int cantidad;
    int *elementos;
} Combinacion;

int main()
{
    int n, W;

    printf("El problema de la mochila entera\n");
    printf("- Nos dan n objetos y una mochila.\n");
    printf("- Para i=1, 2, ..., n el objeto i tiene un peso positivo wi y un valor positivo vi.\n");
    printf("- La mochila tiene que llevar un peso que no sobrepase W.\n");
    printf("- Se debe definir el numero de objetos, el peso y valor de cada objeto y el peso maximo de la mochila.\n");
    printf("\nNuestro objetivo es llenar la mochila de tal manera que se maximice el valor de los objetos transportados, respetando la limitacion de capacidad impuesta.\n\n");

    printf("Numero de objetos: ");
    scanf("%d", &n);

    Objeto *objetos = malloc(n * sizeof(Objeto));
    if (!objetos) { 
        printf("No hay memoria.\n"); 
        return 1; 
    }

    printf("Peso maximo de la mochila: ");
    scanf("%d", &W);

    for (int i = 0; i < n; i++)
    {
        printf("\nObjeto %d\nPeso: ", i + 1);
        scanf("%d", &objetos[i].peso);
        printf("Valor: ");
        scanf("%d", &objetos[i].valor);
    }

    int total = (int)pow(2, n);

    Combinacion *tabla = malloc(total * sizeof(Combinacion));
    if (!tabla) { 
        printf("No hay memoria para la tabla."); 
        free(objetos); 
        return 1; 
    }

    int valor_optimo = 0, combinacion_optima = 0, validas = 0, invalidas = 0;

    for (int i = 0; i < total; i++)
    {
        tabla[i].elementos = malloc(n * sizeof(int));
        tabla[i].cantidad  = 0;
        int peso = 0, valor = 0;

        for (int j = 0; j < n; j++)
        {
            if (i & (1 << j))
            {
                peso  += objetos[j].peso;
                valor += objetos[j].valor;
                tabla[i].elementos[tabla[i].cantidad++] = j;
            }
        }

        tabla[i].id = i; 
        tabla[i].peso_total = peso; 
        tabla[i].valor_total = valor;

        if (peso <= W)
        {
            validas++;
            if (valor > valor_optimo) { 
                valor_optimo = valor; 
                combinacion_optima = i; 
            }
        } else invalidas++;
    }

    int ancho_max = 2;
    for (int i = 0; i < total; i++)
    {
        int ancho = 2;
        for (int k = 0; k < tabla[i].cantidad; k++)
        {
            int obj = tabla[i].elementos[k] + 1;
            ancho += (obj < 10) ? 1 : (obj < 100) ? 2 : 3;
            if (k < tabla[i].cantidad - 1) ancho += 2;
        }
        if (ancho > ancho_max) ancho_max = ancho;
    }

    int ancho_id  = snprintf(NULL, 0, "%d", total - 1);
    int ancho_bin = n > 7 ? n : 7;
    int linea     = ancho_id + ancho_bin + ancho_max + 42;

    printf("\n%-*s | %-*s | %-*s | %-5s | %-5s | %-6s | %s\n",
        ancho_id,  "#",
        ancho_bin, "Binario",
        ancho_max, "Elementos",
        "Peso", "Valor", "Valida", "Optima");
    for (int i = 0; i < linea; i++) printf("-");

    for (int i = 0; i < total; i++)
    {
        char bin[64];
        for (int b = n - 1; b >= 0; b--)
            bin[n - 1 - b] = (i & (1 << b)) ? '1' : '0';
        bin[n] = '\0';

        char buf[1024]; int pos = 0;
        buf[pos++] = '{';
        for (int k = 0; k < tabla[i].cantidad; k++)
        {
            pos += sprintf(buf + pos, "%d", tabla[i].elementos[k] + 1);
            if (k < tabla[i].cantidad - 1) { buf[pos++] = ','; buf[pos++] = ' '; }
        }
        buf[pos++] = '}'; buf[pos] = '\0';

        printf("\n%-*d | %-*s | %-*s | %-5d | %-5d | %-6s | %s",
            ancho_id,  i,
            ancho_bin, bin,
            ancho_max, buf,
            tabla[i].peso_total, tabla[i].valor_total,
            tabla[i].peso_total <= W ? "Si" : "No",
            i == combinacion_optima  ? "Si" : "No");
    }

    printf("\n"); 
    for (int i = 0; i < linea; i++) printf("-");
    printf("\nCombinaciones validas:   %d", validas);
    printf("\nCombinaciones invalidas: %d\n", invalidas);

    printf("\nLa solucion optima es agregar los objetos ");
    for (int k = 0; k < tabla[combinacion_optima].cantidad; k++)
    {
        printf("O%d", tabla[combinacion_optima].elementos[k] + 1);
        if (k < tabla[combinacion_optima].cantidad - 2) printf(", ");
        else if (k == tabla[combinacion_optima].cantidad - 2) printf(" y ");
    }
    printf(" con un peso total de %d y un valor de %d.\n", tabla[combinacion_optima].peso_total, valor_optimo);

    for (int i = 0; i < total; i++) 
        free(tabla[i].elementos);
    free(objetos);
    free(tabla);

    return 0;
}