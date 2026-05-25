/*
    Práctica 8: Pilas - Tratamiento de expresiones aritméticas
    Alumnos:
     - Álvarez Tahuilán Luis Gustavo
     - Noyola Gómez Emilio Damian
    Fecha: 25 de mayo de 2026
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define WHITE   "\033[37m"

/* ============================================================
   NODOS
   ============================================================ */
typedef struct NodoChar
{
    char valor;
    struct NodoChar *sig;
} NodoChar;

typedef struct NodoDouble
{
    double valor;
    struct NodoDouble *sig;
} NodoDouble;

/* ============================================================
   UTILIDADES DE INTERFAZ
   ============================================================ */
void limpiarPantalla()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void mostrarExito(const char *msg)
{
    printf("  %s%s✔  %s%s\n", BOLD, GREEN, msg, RESET);
}

void mostrarError(const char *msg)
{
    printf("  %s%s✘  %s%s\n", BOLD, RED, msg, RESET);
}

void mostrarAdvertencia(const char *msg)
{
    printf("  %s%s⚠  %s%s\n", BOLD, YELLOW, msg, RESET);
}

/* ============================================================
   PILA DE CHARS
   ============================================================ */
void pushChar(NodoChar **p, char v)
{
    NodoChar *nuevo = (NodoChar *) malloc(sizeof(NodoChar));
    nuevo->valor = v;
    nuevo->sig   = *p;
    *p = nuevo;
}

char popChar(NodoChar **p)
{
    NodoChar *aux = *p;
    char ret      = aux->valor;
    *p  = aux->sig;
    free(aux);
    return ret;
}

char topeChar(NodoChar *p)
{
    return p->valor;
}

int pilaCharVacia(NodoChar *p)
{
    return p == NULL;
}

void liberarChar(NodoChar **p)
{
    while (*p != NULL)
    {
        NodoChar *prox = (*p)->sig;
        free(*p);
        *p = prox;
    }
}

/* Imprime el contenido de la pila de chars (tope → fondo) */
void imprimirPilaChar(NodoChar *p)
{
    if (p == NULL)
    {
        printf("%s[vacía]%s", DIM, RESET);
        return;
    }
    printf("%s[%s", CYAN, RESET);
    NodoChar *aux = p;
    while (aux != NULL)
    {
        printf("%s%c%s", YELLOW, aux->valor, RESET);
        if (aux->sig != NULL) printf(" | ");
        aux = aux->sig;
    }
    printf("%s]%s", CYAN, RESET);
}

/* ============================================================
   PILA DE DOUBLES
   ============================================================ */
void pushDouble(NodoDouble **p, double v)
{
    NodoDouble *nuevo = (NodoDouble *) malloc(sizeof(NodoDouble));
    nuevo->valor = v;
    nuevo->sig   = *p;
    *p = nuevo;
}

double popDouble(NodoDouble **p)
{
    NodoDouble *aux = *p;
    double ret      = aux->valor;
    *p  = aux->sig;
    free(aux);
    return ret;
}

int pilaDoubleVacia(NodoDouble *p)
{
    return p == NULL;
}

int contarDouble(NodoDouble *p)
{
    int n = 0;
    while (p != NULL) { n++; p = p->sig; }
    return n;
}

void liberarDouble(NodoDouble **p)
{
    while (*p != NULL)
    {
        NodoDouble *prox = (*p)->sig;
        free(*p);
        *p = prox;
    }
}

/* Imprime el contenido de la pila de doubles (tope → fondo) */
void imprimirPilaDouble(NodoDouble *p)
{
    if (p == NULL)
    {
        printf("%s[vacía]%s", DIM, RESET);
        return;
    }
    printf("%s[%s", CYAN, RESET);
    NodoDouble *aux = p;
    while (aux != NULL)
    {
        printf("%s%.4g%s", YELLOW, aux->valor, RESET);
        if (aux->sig != NULL) printf(" | ");
        aux = aux->sig;
    }
    printf("%s]%s", CYAN, RESET);
}

/* ============================================================
   UTILIDADES DE OPERADORES
   ============================================================ */
int precedencia(char op)
{
    switch (op)
    {
        case '^': return 3;
        case '*':
        case '/':
        case '%': return 2;
        case '+':
        case '-': return 1;
        default:  return 0;
    }
}

int esOperador(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' ||
           c == '%' || c == '^';
}

int esAperturaParentesis(char c)
{
    return c == '(' || c == '[' || c == '{';
}

int esCierreParentesis(char c)
{
    return c == ')' || c == ']' || c == '}';
}

int sonPareja(char apertura, char cierre)
{
    return (apertura == '(' && cierre == ')') ||
           (apertura == '[' && cierre == ']') ||
           (apertura == '{' && cierre == '}');
}

/* ============================================================
   BALANCEO DE PARÉNTESIS  (con traza paso a paso)
   ============================================================ */
int balancearParentesis(const char *expr)
{
    NodoChar *pila = NULL;
    int paso = 0;

    printf("\n  %s%sPASO A PASO — Balanceo de paréntesis%s\n\n", BOLD, BLUE, RESET);
    printf("  %s%-6s  %-10s  %-8s  %s%s\n", DIM, "Paso", "Símbolo", "Acción", "Pila", RESET);

    for (int i = 0; expr[i] != '\0'; i++)
    {
        char c = expr[i];

        /* Solo mostramos pasos cuando el símbolo es relevante */
        if (esAperturaParentesis(c) || esCierreParentesis(c))
        {
            paso++;
            const char *accion = "";

            if (esAperturaParentesis(c))
            {
                pushChar(&pila, c);
                accion = "PUSH";
            }
            else
            {
                if (pilaCharVacia(pila))
                {
                    printf("  %-6d  %s%-10c%s  %s%-8s%s  ",
                           paso, YELLOW, c, RESET, RED, "ERROR", RESET);
                    imprimirPilaChar(pila);
                    printf("\n");
                    liberarChar(&pila);
                    printf("\n");
                    mostrarError("Cierre sin apertura correspondiente.");
                    return 0;
                }

                char tope = popChar(&pila);

                if (!sonPareja(tope, c))
                {
                    printf("  %-6d  %s%-10c%s  %s%-8s%s  ",
                           paso, YELLOW, c, RESET, RED, "ERROR", RESET);
                    imprimirPilaChar(pila);
                    printf("\n");
                    liberarChar(&pila);
                    printf("\n");
                    mostrarError("Tipo de paréntesis incompatible.");
                    return 0;
                }
                accion = "POP";
            }

            printf("  %-6d  %s%-10c%s  %s%-8s%s  ",
                   paso, YELLOW, c, RESET, GREEN, accion, RESET);
            imprimirPilaChar(pila);
            printf("\n");
        }
    }

    int resultado = pilaCharVacia(pila);
    liberarChar(&pila);

    if (resultado)
        mostrarExito("Paréntesis BALANCEADOS — pila vacía al final.");
    else
        mostrarError("Paréntesis NO balanceados — quedan aperturas sin cerrar.");

    return resultado;
}

/* ============================================================
   CONVERSIÓN INFIJA → POSTFIJA  (con traza paso a paso)
   ============================================================ */
void infijaAPostfija(const char *expr, char *postfija)
{
    NodoChar *pila = NULL;
    int j    = 0;
    int paso = 0;

    printf("\n  %s%sPASO A PASO — Conversión Infija a Postfija%s\n\n", BOLD, BLUE, RESET);
    printf("  %s%-6s  %-12s  %-28s  %s%s\n",
           DIM, "Paso", "Símbolo", "Postfija acumulada", "Pila", RESET);

    for (int i = 0; expr[i] != '\0'; i++)
    {
        char c = expr[i];
        if (c == ' ') continue;

        paso++;

        /* Número (posiblemente multidigito) */
        if (isdigit(c))
        {
            char numStr[32];
            int  k = 0;
            while (isdigit(expr[i]))
                numStr[k++] = expr[i++];
            numStr[k] = '\0';
            i--;

            for (int m = 0; m < k; m++)
                postfija[j++] = numStr[m];
            postfija[j++] = ' ';

            /* imprimir fila */
            postfija[j] = '\0';
            printf("  %-6d  %s%-12s%s  %s%-28s%s  ",
                   paso, GREEN, numStr, RESET, YELLOW, postfija, RESET);
            imprimirPilaChar(pila);
            printf("\n");
        }
        /* Paréntesis de apertura */
        else if (esAperturaParentesis(c))
        {
            pushChar(&pila, c);

            postfija[j] = '\0';
            printf("  %-6d  %s%-12c%s  %s%-28s%s  ",
                   paso, MAGENTA, c, RESET, YELLOW, postfija, RESET);
            imprimirPilaChar(pila);
            printf("\n");
        }
        /* Paréntesis de cierre */
        else if (esCierreParentesis(c))
        {
            while (!pilaCharVacia(pila) && !esAperturaParentesis(topeChar(pila)))
            {
                postfija[j++] = popChar(&pila);
                postfija[j++] = ' ';
            }
            if (!pilaCharVacia(pila))
                popChar(&pila);   /* descartar apertura */

            postfija[j] = '\0';
            printf("  %-6d  %s%-12c%s  %s%-28s%s  ",
                   paso, MAGENTA, c, RESET, YELLOW, postfija, RESET);
            imprimirPilaChar(pila);
            printf("\n");
        }
        /* Operador */
        else if (esOperador(c))
        {
            while (!pilaCharVacia(pila) &&
                   !esAperturaParentesis(topeChar(pila)) &&
                   precedencia(topeChar(pila)) >= precedencia(c))
            {
                postfija[j++] = popChar(&pila);
                postfija[j++] = ' ';
            }
            pushChar(&pila, c);

            postfija[j] = '\0';
            printf("  %-6d  %s%-12c%s  %s%-28s%s  ",
                   paso, CYAN, c, RESET, YELLOW, postfija, RESET);
            imprimirPilaChar(pila);
            printf("\n");
        }
    }

    /* Vaciar pila */
    while (!pilaCharVacia(pila))
    {
        paso++;
        char op = popChar(&pila);
        postfija[j++] = op;
        postfija[j++] = ' ';
        postfija[j]   = '\0';

        printf("  %-6d  %s%-12s%s  %s%-28s%s  ",
               paso, CYAN, "(vaciar pila)", RESET, YELLOW, postfija, RESET);
        imprimirPilaChar(pila);
        printf("\n");
    }

    if (j > 0 && postfija[j-1] == ' ') j--;
    postfija[j] = '\0';

    liberarChar(&pila);
    printf("  %sResultado:%s %s%s%s\n", BOLD, RESET, GREEN, postfija, RESET);
}

/* ============================================================
   EVALUACIÓN RPN  (con traza paso a paso)
   ============================================================ */
double evaluarRPN(const char *postfija, int *errorFlag)
{
    NodoDouble *pila = NULL;
    *errorFlag = 0;

    printf("\n  %s%sPASO A PASO — Evaluación RPN%s\n\n",
           BOLD, BLUE, RESET);
    printf("  %s%-6s  %-12s  %-14s  %s%s\n",
           DIM, "Paso", "Token", "Operación", "Pila", RESET);

    char token[64];
    int  i = 0, t = 0, paso = 0;

    while (postfija[i] != '\0')
    {
        char c = postfija[i];

        if (c == ' ' || postfija[i+1] == '\0')
        {
            if (c != ' ') token[t++] = c;
            token[t] = '\0';

            if (t > 0)
            {
                paso++;
                char *endptr;
                double val = strtod(token, &endptr);

                if (endptr != token)   /* es número */
                {
                    pushDouble(&pila, val);

                    printf("  %-6d  %s%-12s%s  %s%-14s%s  ",
                           paso, GREEN, token, RESET, DIM, "PUSH", RESET);
                    imprimirPilaDouble(pila);
                    printf("\n");
                }
                else if (strlen(token) == 1 && esOperador(token[0]))
                {
                    if (contarDouble(pila) < 2)
                    {
                        printf("\n");
                        mostrarError("Error RPN: operandos insuficientes.");
                        liberarDouble(&pila);
                        *errorFlag = 1;
                        return 0;
                    }

                    double b = popDouble(&pila);
                    double a = popDouble(&pila);
                    double resultado = 0;
                    char   opStr[32];

                    switch (token[0])
                    {
                        case '+':
                            resultado = a + b;
                            snprintf(opStr, sizeof(opStr), "%.4g + %.4g", a, b);
                            break;
                        case '-':
                            resultado = a - b;
                            snprintf(opStr, sizeof(opStr), "%.4g - %.4g", a, b);
                            break;
                        case '*':
                            resultado = a * b;
                            snprintf(opStr, sizeof(opStr), "%.4g * %.4g", a, b);
                            break;
                        case '/':
                            if (b == 0)
                            {
                                printf("\n");
                                mostrarError("Error RPN: division por cero.");
                                liberarDouble(&pila);
                                *errorFlag = 1;
                                return 0;
                            }
                            resultado = a / b;
                            snprintf(opStr, sizeof(opStr), "%.4g / %.4g", a, b);
                            break;
                        case '%':
                            resultado = fmod(a, b);
                            snprintf(opStr, sizeof(opStr), "%.4g %% %.4g", a, b);
                            break;
                        case '^':
                            resultado = pow(a, b);
                            snprintf(opStr, sizeof(opStr), "%.4g ^ %.4g", a, b);
                            break;
                    }

                    pushDouble(&pila, resultado);

                    printf("  %-6d  %s%-12c%s  %s%-14s%s  ",
                           paso, CYAN, token[0], RESET, YELLOW, opStr, RESET);
                    imprimirPilaDouble(pila);
                    printf("\n");
                }
            }

            t = 0;
        }
        else
        {
            token[t++] = c;
        }

        i++;
    }


    if (contarDouble(pila) != 1)
    {
        mostrarError("Error RPN: demasiados operandos.");
        liberarDouble(&pila);
        *errorFlag = 1;
        return 0;
    }

    double resultado = popDouble(&pila);
    liberarDouble(&pila);
    return resultado;
}

/* ============================================================
   MAIN
   ============================================================ */
int main()
{
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    char infija[512];
    char postfija[1024];

    limpiarPantalla();

    printf("\n %s%s PRACTICA 8 — PILAS: TRATAMIENTO DE EXPRESIONES ARITMÉTICAS %s\n", BOLD, CYAN, RESET);
    printf("  %sAlgoritmos y Estructuras de Datos%s\n\n", DIM, RESET);

    printf("  %s> Ingrese la expresion infija:%s ", BOLD, RESET);
    fgets(infija, sizeof(infija), stdin);
    infija[strcspn(infija, "\n")] = '\0';

    if (strlen(infija) == 0)
    {
        mostrarAdvertencia("No se ingreso ninguna expresion.");
        return 1;
    }

    /* ── 1. BALANCEO ─────────────────────────────────────────── */
    printf("\n  %s%s[1] BALANCEO DE PARENTESIS%s\n", BOLD, BLUE, RESET);
    printf("  %sExpresion:%s %s%s%s\n", DIM, RESET, CYAN, infija, RESET);

    int balanceado = balancearParentesis(infija);

    if (!balanceado)
    {
        printf("\n  %sNo es posible continuar con la expresion ingresada.%s\n\n", RED, RESET);
        return 1;
    }

    /* ── 2. CONVERSIÓN INFIJA → POSTFIJA ────────────────────── */
    printf("\n  %s%s[2] CONVERSION INFIJA a POSTFIJA%s\n", BOLD, BLUE, RESET);
    printf("  %sExpresion infija:%s %s%s%s\n\n", DIM, RESET, CYAN, infija, RESET);

    infijaAPostfija(infija, postfija);

    /* ── 3. EVALUACIÓN RPN ───────────────────────────────────── */
    printf("\n  %s%s[3] EVALUACION RPN%s\n", BOLD, BLUE, RESET);
    printf("  %sExpresion postfija:%s %s%s%s\n\n", DIM, RESET, YELLOW, postfija, RESET);

    int    errorFlag = 0;
    double resultado = evaluarRPN(postfija, &errorFlag);

    /* ── RESUMEN FINAL ───────────────────────────────────────── */
    printf("\n  %sInfija  :%s  %s%s%s\n", BOLD, RESET, CYAN,   infija,   RESET);
    printf("  %sPostfija:%s  %s%s%s\n", BOLD, RESET, YELLOW, postfija, RESET);

    if (!errorFlag)
    {
        printf("  %sResultado:%s %s%.4g%s\n", BOLD, RESET, GREEN, resultado, RESET);
    }
    else
    {
        mostrarError("El proceso termino con errores.");
    }

    printf("\n");

    return 0;
}