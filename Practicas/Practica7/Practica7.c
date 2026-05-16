/*
    Práctica 7: Listas dinámicas
    Alumnos: 
     - Álvarez Tahuilán Luis Gustavo
     - Noyola Gómez Emilio Damian
    Fecha: 18 de mayo de 2026
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

typedef struct Nodo
{
    int valor;
    struct Nodo* sig;
} Nodo;

void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pausar() {
    printf("\n%s%s  Presione ENTER para continuar...%s", DIM, CYAN, RESET);
    limpiarBuffer();
    getchar();
}

void mostrarEncabezado(const char *titulo) {
    limpiarPantalla();
    int ancho = 60;
    int len = (int)strlen(titulo);
    int pad = (ancho - len - 2) / 2;

    printf("\n%s", CYAN);
    printf("  ╔");
    for (int i = 0; i < ancho - 2; i++) printf("═");
    printf("╗\n");

    printf("  ║%s%s%*s%s%*s%s ║\n",
           BOLD, BLUE,
           pad + len, titulo,
           CYAN,
           ancho - 2 - pad - len, "",
           CYAN);

    printf("  ╚");
    for (int i = 0; i < ancho - 2; i++) printf("═");
    printf("╝%s\n", RESET);
}

void mostrarExito(const char *msg) {
    printf("\n  %s%s✔  %s%s\n", BOLD, GREEN, msg, RESET);
}

void mostrarError(const char *msg) {
    printf("\n  %s%s✘  %s%s\n", BOLD, RED, msg, RESET);
}

void mostrarAdvertencia(const char *msg) {
    printf("\n  %s%s⚠  %s%s\n", BOLD, YELLOW, msg, RESET);
}

// Agrega un nodo al final de la lista
void agregar(Nodo **p, int v)
{
    Nodo *nuevo = (Nodo *) malloc(sizeof(Nodo));
    nuevo->valor = v;
    nuevo->sig   = NULL;

    if (*p == NULL)
    {
        *p = nuevo;
    }
    else
    {
        Nodo *aux = *p;
        while (aux->sig != NULL)
            aux = aux->sig;
        aux->sig = nuevo;
    }
}

// Muestra todos los nodos de la lista
void mostrar(Nodo *p)
{
    if (p == NULL)
    {
        mostrarAdvertencia("La lista está vacía.");
        return;
    }

    printf("\n  %s", CYAN);
    printf("╔══════════════╗\n");
    printf("  ║  %s%-12s%s%s║\n", BOLD, " Valores", RESET, CYAN);
    printf("  ╠══════════════╣%s\n", RESET);

    Nodo *aux = p;
    int pos = 1;
    while (aux != NULL)
    {
        printf("  %s║%s  %s%-3d%s  → %s%-4d%s %s║%s\n",
               CYAN, RESET,
               DIM, pos, RESET,
               GREEN, aux->valor, RESET,
               CYAN, RESET);
        aux = aux->sig;
        pos++;
    }

    printf("  %s╚══════════════╝%s\n", CYAN, RESET);
}

// Libera toda la memoria de la lista
void liberar(Nodo **p)
{
    while (*p != NULL)
    {
        Nodo *prox = (*p)->sig;
        free(*p);
        *p = prox;
    }
}

// Busca un valor, regresa puntero al nodo o NULL
Nodo* buscar(Nodo *p, int v)
{
    Nodo *aux = p;
    while ((aux != NULL) && (aux->valor != v))
        aux = aux->sig;
    return aux;
}

// Inserta un valor en orden ascendente
Nodo* insertarOrdenado(Nodo **p, int v)
{
    Nodo *nuevo = (Nodo *) malloc(sizeof(Nodo));
    nuevo->valor = v;
    nuevo->sig   = NULL;

    Nodo *aux = *p;
    Nodo *ant = NULL;

    while ((aux != NULL) && (aux->valor <= v))
    {
        ant = aux;
        aux = aux->sig;
    }

    if (ant == NULL)
        *p = nuevo;       
    else
        ant->sig = nuevo; 

    nuevo->sig = aux;
    return nuevo;
}

// 1. Copiar lista en otra lista de forma ordenada
void copiarOrdenada(Nodo *origen, Nodo **destino)
{
    liberar(destino);

    Nodo *aux = origen;
    while (aux != NULL)
    {
        insertarOrdenado(destino, aux->valor);
        aux = aux->sig;
    }
}

// 2. Invertir la lista
void invertir(Nodo *origen, Nodo **invertida)
{
    liberar(invertida);

    Nodo *aux = origen;
    while (aux != NULL)
    {
        Nodo *nuevo = (Nodo *) malloc(sizeof(Nodo));
        nuevo->valor = aux->valor;
        nuevo->sig   = *invertida;
        *invertida   = nuevo;

        aux = aux->sig;
    }
}

// 3. Eliminar duplicados (modifica la lista original)
void eliminarDuplicados(Nodo **p)
{
    Nodo *actual = *p;

    while (actual != NULL)
    {
        Nodo *aux = actual;

        while (aux->sig != NULL)
        {
            if (aux->sig->valor == actual->valor)
            {
                Nodo *dup = aux->sig;
                aux->sig  = dup->sig;
                free(dup);
            }
            else
            {
                aux = aux->sig;
            }
        }
        actual = actual->sig;
    }
}

int contarNodos(Nodo *p)
{
    int n = 0;
    Nodo *aux = p;
    while (aux != NULL) { n++; aux = aux->sig; }
    return n;
}

double calcularMedia(Nodo *p, int n)
{
    double suma = 0;
    Nodo *aux = p;
    while (aux != NULL) { suma += aux->valor; aux = aux->sig; }
    return suma / n;
}

double calcularMediana(Nodo *ordenada, int n)
{
    Nodo *aux = ordenada;
    int medio = n / 2;

    if (n % 2 == 1)
    {
        for (int i = 0; i < medio; i++) aux = aux->sig;
        return (double) aux->valor;
    }
    else
    {
        for (int i = 0; i < medio - 1; i++) aux = aux->sig;
        double v1 = aux->valor;
        double v2 = aux->sig->valor;
        return (v1 + v2) / 2.0;
    }
}

int calcularModa(Nodo *p)
{
    int moda      = p->valor;
    int maxFreq   = 0;

    Nodo *aux = p;
    while (aux != NULL)
    {
        int freq = 0;
        Nodo *aux2 = p;
        while (aux2 != NULL)
        {
            if (aux2->valor == aux->valor) freq++;
            aux2 = aux2->sig;
        }
        if (freq > maxFreq)
        {
            maxFreq = freq;
            moda    = aux->valor;
        }
        aux = aux->sig;
    }
    return moda;
}

double calcularVarianza(Nodo *p, int n, double media)
{
    double suma = 0;
    Nodo *aux = p;
    while (aux != NULL)
    {
        double diff = aux->valor - media;
        suma += diff * diff;
        aux = aux->sig;
    }
    return suma / n;
}

void menuMostrarEstadisticas(Nodo *lista, Nodo *ordenada)
{
    mostrarEncabezado("ESTADÍSTICAS DE LA LISTA");

    int n = contarNodos(lista);
    if (n == 0) { mostrarAdvertencia("La lista está vacía."); pausar(); return; }

    double media    = calcularMedia(lista, n);
    double mediana  = calcularMediana(ordenada, n);
    int    moda     = calcularModa(lista);
    double varianza = calcularVarianza(lista, n, media);
    double desv     = sqrt(varianza);

    printf("  %s╔══════════════════════════════════════╗%s\n", CYAN, RESET);
    printf("  %s║%s %s%-36s%s %s ║%s\n", CYAN, RESET, BOLD, "        MEDIDAS ESTADÍSTICAS      ", RESET, CYAN, RESET);
    printf("  %s╠══════════════════════════════════════╣%s\n", CYAN, RESET);
    printf("  %s║%s  Total de valores:    %s%-14d%s %s║%s\n", CYAN, RESET, GREEN, n,       RESET, CYAN, RESET);
    printf("  %s║%s  Media:               %s%-14.2f%s %s║%s\n", CYAN, RESET, GREEN, media,   RESET, CYAN, RESET);
    printf("  %s║%s  Mediana:             %s%-14.2f%s %s║%s\n", CYAN, RESET, GREEN, mediana, RESET, CYAN, RESET);
    printf("  %s║%s  Moda:                %s%-14d%s %s║%s\n", CYAN, RESET, GREEN, moda,    RESET, CYAN, RESET);
    printf("  %s║%s  Varianza:            %s%-14.2f%s %s║%s\n", CYAN, RESET, GREEN, varianza,RESET, CYAN, RESET);
    printf("  %s║%s  Desviación estándar: %s%-14.2f%s %s║%s\n", CYAN, RESET, GREEN, desv,    RESET, CYAN, RESET);
    printf("  %s╚══════════════════════════════════════╝%s\n", CYAN, RESET);

    pausar();
}

void menuPrincipal()
{
    Nodo *lista    = NULL;  
    Nodo *ordenada = NULL;  
    Nodo *invertida = NULL; 

    int op = 0;

    while (op != 6)
    {
        mostrarEncabezado("LISTAS DINÁMICAS SIMPLEMENTE ENLAZADAS");
        printf("  %sPráctica 7 - Algoritmos y Estructuras de Datos%s\n\n", DIM, RESET);

        printf("  %sNodos en lista:%s %s%d%s\n", BOLD, RESET, GREEN, contarNodos(lista), RESET);

        printf("\n  %sMENÚ PRINCIPAL%s\n\n", BOLD, RESET);
        printf("    %s[1]%s  Ingresar valores (0 para terminar)\n", CYAN, RESET);
        printf("    %s[2]%s  Mostrar listas\n", CYAN, RESET);
        printf("    %s[3]%s  Lista ordenada\n", CYAN, RESET);
        printf("    %s[4]%s  Lista invertida\n", CYAN, RESET);
        printf("    %s[5]%s  Eliminar duplicados\n", CYAN, RESET);
        printf("    %s[6]%s  Ver estadísticas\n", CYAN, RESET);
        printf("    %s[7]%s  Salir\n\n", RED, RESET);

        printf("  %sSeleccione una opción:%s ", BOLD, RESET);
        scanf("%d", &op);

        switch (op)
        {
        case 1:
        {
            mostrarEncabezado("INGRESAR VALORES");

            liberar(&lista);
            liberar(&ordenada);
            liberar(&invertida);

            printf("  %sIngrese enteros (0 para terminar):%s\n\n", BOLD, RESET);

            int v;
            int count = 0;
            do {
                printf("    %sValor #%d:%s ", DIM, count + 1, RESET);
                scanf("%d", &v);
                if (v != 0)
                {
                    agregar(&lista, v);
                    count++;
                }
            } while (v != 0);

            mostrarExito("Lista creada exitosamente.");
            printf("  %s→ Nodos ingresados: %s%d%s\n", DIM, RESET, count, RESET);
            pausar();
            break;
        }

        case 2:
        {
            mostrarEncabezado("MOSTRAR LISTAS");

            printf("\n  %s%sLISTA ORIGINAL:%s\n", BOLD, BLUE, RESET);
            mostrar(lista);

            printf("\n  %s%sLISTA ORDENADA:%s\n", BOLD, BLUE, RESET);
            if (ordenada == NULL)
                mostrarAdvertencia("Aún no se generó la lista ordenada (opción 3).");
            else
                mostrar(ordenada);

            printf("\n  %s%sLISTA INVERTIDA:%s\n", BOLD, BLUE, RESET);
            if (invertida == NULL)
                mostrarAdvertencia("Aún no se generó la lista invertida (opción 4).");
            else
                mostrar(invertida);

            pausar();
            break;
        }

        case 3:
        {
            mostrarEncabezado("COPIAR LISTA ORDENADA");

            if (lista == NULL) { mostrarAdvertencia("La lista está vacía."); pausar(); break; }

            copiarOrdenada(lista, &ordenada);

            mostrarExito("Lista copiada en orden ascendente.");
            printf("\n  %sLista ordenada:%s\n", BOLD, RESET);
            mostrar(ordenada);
            pausar();
            break;
        }

        case 4:
        {
            mostrarEncabezado("INVERTIR LISTA");

            if (lista == NULL) { mostrarAdvertencia("La lista está vacía."); pausar(); break; }

            invertir(lista, &invertida);

            mostrarExito("Lista invertida generada.");
            printf("\n  %sOriginal:%s\n", BOLD, RESET);
            mostrar(lista);
            printf("\n  %sInvertida:%s\n", BOLD, RESET);
            mostrar(invertida);
            pausar();
            break;
        }

        case 5:
        {
            mostrarEncabezado("ELIMINAR DUPLICADOS");

            if (lista == NULL) { mostrarAdvertencia("La lista está vacía."); pausar(); break; }

            printf("  %sAntes:%s\n", BOLD, RESET);
            mostrar(lista);

            eliminarDuplicados(&lista);

            copiarOrdenada(lista, &ordenada);
            invertir(lista, &invertida);

            printf("\n  %sDespués (sin duplicados):%s\n", BOLD, RESET);
            mostrar(lista);

            mostrarExito("Duplicados eliminados exitosamente.");
            pausar();
            break;
        }

        case 6:
        {
            if (lista == NULL) { mostrarAdvertencia("La lista está vacía."); pausar(); break; }
            if (ordenada == NULL) copiarOrdenada(lista, &ordenada);
            menuMostrarEstadisticas(lista, ordenada);
            break;
        }

        case 7:
            mostrarEncabezado("CERRANDO SISTEMA");
            liberar(&lista);
            liberar(&ordenada);
            liberar(&invertida);

            printf("\n%s", GREEN);
            printf("  ╔══════════════════════════════════════════════════╗\n");
            printf("  ║        ¡Hasta pronto! Memoria liberada.         ║\n");
            printf("  ╚══════════════════════════════════════════════════╝\n");
            printf("%s\n", RESET);
            exit(0);
            break;

        default:
            mostrarError("Opción no válida. Intente de nuevo.");
            pausar();
            break;
        }
    }
}

int main()
{
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    menuPrincipal();

    return 0;
}