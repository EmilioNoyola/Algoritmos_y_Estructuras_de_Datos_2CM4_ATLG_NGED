/*
    Práctica 9: Colas - Suma de Enteros Largos
    Alumnos:
     - Álvarez Tahuilán Luis Gustavo
     - Noyola Gómez Emilio Damian
    Fecha: 01 de junio de 2026
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"

typedef struct Nodo
{
    int          valor;
    struct Nodo *sig;
} Nodo;


void limpiarPantalla()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void mostrarError(const char *msg)
{
    printf("  %s%s✘  %s%s\n", BOLD, RED, msg, RESET);
}


void encolar(Nodo **p, int v)
{
    Nodo *nuevo = malloc(sizeof(Nodo));
    if (!nuevo) { mostrarError("Sin memoria."); exit(1); }
    nuevo->valor = v;
    if (*p == NULL)
        nuevo->sig = nuevo;
    else
    {
        nuevo->sig = (*p)->sig;
        (*p)->sig  = nuevo;
    }
    *p = nuevo;
}

int desencolar(Nodo **p)
{
    int ret = (*p)->sig->valor;
    if (*p == (*p)->sig)
    {
        free(*p);
        *p = NULL;
    }
    else
    {
        Nodo *aux = (*p)->sig;
        (*p)->sig = aux->sig;
        free(aux);
    }
    return ret;
}

int  colaVacia(Nodo *p) { return p == NULL; }

void liberarCola(Nodo **p)
{
    while (!colaVacia(*p)) desencolar(p);
}

void imprimirCola(Nodo *p)
{
    if (!p) { printf("%s[vacia]%s", DIM, RESET); return; }
    printf("%s[%s", CYAN, RESET);
    Nodo *aux = p->sig;
    while (1)
    {
        printf("%s%d%s", YELLOW, aux->valor, RESET);
        if (aux == p) break;
        printf(" -> ");
        aux = aux->sig;
    }
    printf("%s]%s", CYAN, RESET);
}


typedef struct NodoChar
{
    char             valor;
    struct NodoChar *sig;
} NodoChar;

void pushChar(NodoChar **p, char v)
{
    NodoChar *nuevo = malloc(sizeof(NodoChar));
    if (!nuevo) { mostrarError("Sin memoria."); exit(1); }
    nuevo->valor = v;
    nuevo->sig   = *p;
    *p = nuevo;
}

char popChar(NodoChar **p)
{
    NodoChar *aux = *p;
    char ret      = aux->valor;
    *p = aux->sig;
    free(aux);
    return ret;
}

int pilaCharVacia(NodoChar *p) { return p == NULL; }


void cargarEnCola(Nodo **cola, const char *numStr)
{
    int len = (int)strlen(numStr);
    for (int i = len - 1; i >= 0; i--)
        encolar(cola, numStr[i] - '0');
}

void sumarEnterosLargos(const char *strA, const char *strB)
{
    int lenA   = (int)strlen(strA);
    int lenB   = (int)strlen(strB);
    int maxLen = lenA > lenB ? lenA : lenB;

    Nodo *colaA = NULL, *colaB = NULL;
    cargarEnCola(&colaA, strA);
    cargarEnCola(&colaB, strB);

    printf("\n  %s%s[1] COLAS%s\n\n", BOLD, BLUE, RESET);

    printf("  %sCola A%s (frente = unidades): ", BOLD, RESET);
    imprimirCola(colaA); printf("\n");

    printf("  %sCola B%s (frente = unidades): ", BOLD, RESET);
    imprimirCola(colaB); printf("\n");

    Nodo     *colaAcarreos  = NULL;
    NodoChar *pilaResultado = NULL;

    encolar(&colaAcarreos, 0);

    printf("\n  %s%s[2] SUMA COLUMNA A COLUMNA%s\n\n", BOLD, BLUE, RESET);
    printf("  %s%-6s  %-8s  %-8s  %-12s  %-8s  %s%s\n", DIM, "Col", "Dig A", "Dig B", "Ac. entrada", "Digito", "Ac. salida", RESET);

    int col = 1;

    while (!colaVacia(colaA) || !colaVacia(colaB))
    {
        int digA      = colaVacia(colaA) ? 0 : desencolar(&colaA);
        int digB      = colaVacia(colaB) ? 0 : desencolar(&colaB);
        int acEntrada = desencolar(&colaAcarreos);   

        int suma        = digA + digB + acEntrada;
        int digitoFinal = suma % 10;
        int acSalida    = suma / 10;

        encolar(&colaAcarreos, acSalida);

        pushChar(&pilaResultado, '0' + digitoFinal);

        printf("  %-6d  %s%-8d%s  %s%-8d%s  %s%-12d%s  %s%-8d%s  %s%d%s\n",
               col++,
               GREEN,   digA,        RESET,
               CYAN,    digB,        RESET,
               YELLOW,  acEntrada,   RESET,
               MAGENTA, digitoFinal, RESET,
               RED,     acSalida,    RESET);
    }

    int acFinal = desencolar(&colaAcarreos);
    if (acFinal)
    {
        pushChar(&pilaResultado, '0' + acFinal);
        printf("  %-6d  %s%-8s%s  %s%-8s%s  %s%-12d%s  %s%-8d%s  %s%d%s\n",
               col,
               GREEN,   "-", RESET,
               CYAN,    "-", RESET,
               YELLOW,  acFinal, RESET,
               MAGENTA, acFinal, RESET,
               RED,     0,       RESET);
    }

    int   resultadoSize = maxLen + 3;
    char *resultado     = malloc(resultadoSize);
    if (!resultado) { mostrarError("Sin memoria."); exit(1); }

    int idx = 0;
    while (!pilaCharVacia(pilaResultado))
        resultado[idx++] = popChar(&pilaResultado);
    resultado[idx] = '\0';

    int lenRes = (int)strlen(resultado);
    int ancho  = lenRes > maxLen ? lenRes : maxLen;

    printf("\n  %s%s[3] RESULTADO FINAL%s\n\n", BOLD, GREEN, RESET);
    printf("  %s  %*s%s\n", CYAN,  ancho, strA,      RESET);
    printf("  %s+ %*s%s\n", CYAN,  ancho, strB,      RESET);
    printf("  %s  ", DIM);
    for (int i = 0; i <= ancho; i++) printf("-");
    printf("%s\n", RESET);
    printf("  %s  %*s%s\n\n", GREEN, ancho, resultado, RESET);

    free(resultado);
    liberarCola(&colaA);
    liberarCola(&colaB);
    liberarCola(&colaAcarreos);
}


int esNumeroValido(const char *s)
{
    if (!s || !*s) return 0;
    for (int i = 0; s[i]; i++)
        if (s[i] < '0' || s[i] > '9') return 0;
    return 1;
}

const char *quitarCerosIzquierda(const char *s)
{
    while (*s == '0' && *(s+1)) s++;
    return s;
}


int main()
{
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    char bufA[512], bufB[512];

    limpiarPantalla();
    printf("\n  %s%sPRÁCTICA 9 - COLAS: SUMA DE ENTEROS LARGOS%s\n", BOLD, CYAN, RESET);
    printf("  %sAlgoritmos y Estructuras de Datos%s\n\n", DIM, RESET);

    printf("  %s> Primer numero:%s  ", BOLD, RESET);
    if (scanf("%511s", bufA) != 1 || !esNumeroValido(bufA)) { mostrarError("Solo digitos positivos."); return 1; }

    printf("  %s> Segundo numero:%s ", BOLD, RESET);
    if (scanf("%511s", bufB) != 1 || !esNumeroValido(bufB)) { mostrarError("Solo digitos positivos."); return 1; }

    const char *numA = quitarCerosIzquierda(bufA);
    const char *numB = quitarCerosIzquierda(bufB);

    printf("\n  %sNumeros a sumar:%s\n", BOLD, RESET);
    printf("    A = %s%s%s\n", GREEN,  numA, RESET);
    printf("    B = %s%s%s\n", YELLOW, numB, RESET);

    sumarEnterosLargos(numA, numB);

    printf("\n");
    return 0;
}