/*
    Práctica 10: Colas de prioridad
    Estudiantes:
     - Álvarez Tahuilán Luis Gustavo
     - Noyola Gómez Emilio Damian
    Date: 08 de junio de 2026

    Zero-based array notation:
        PARENT(i) = (i - 1) / 2     LEFT(i) = 2i + 1     RIGHT(i) = 2i + 2
    The heap array is handled with base index 0, so all positions are valid from
    0 to heapSize - 1.

    The program draws the heap as a binary tree in the console, shows each
    HEAPIFY / sift-up / sift-down step, and supports both max-priority and
    min-priority queues.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

#define HL_A "\033[1;32m"   /* target node / destination position  (green) */
#define HL_B "\033[1;31m"   /* extreme node / exchange position     (red)   */
#define INFO  "\033[1;36m"   /* informational action text            (cyan)  */

/* ---------------------------------------------------------------- */
/* Global priority queue state                                      */
/* ---------------------------------------------------------------- */
int *heapArray      = NULL;   /* array 0..capacity - 1                */
int  heapSize       = 0;      /* number of elements in the queue      */
int  capacity       = 0;      /* maximum size                         */
int  initialSize    = 0;      /* initial number of elements           */
int  isMaxHeap      = 1;      /* 1 = max priority, 0 = min priority   */

/* ---------------------------------------------------------------- */
/* Interface utilities                                              */
/* ---------------------------------------------------------------- */
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void clearInputBuffer()
{
    int character;
    while ((character = getchar()) != '\n' && character != EOF);
}

void pauseProgram()
{
    printf("\n  %s%sPresione ENTER para continuar...%s", DIM, CYAN, RESET);
    clearInputBuffer();   /* consumes the pending '\n' left by scanf */
    getchar();            /* waits for the user to press ENTER       */
}

void showHeader(const char *title)
{
    clearScreen();
    int width = 60;
    int length = (int) strlen(title);
    int padding = (width - length - 2) / 2;

    printf("\n%s", CYAN);
    printf("  ╔");
    for (int i = 0; i < width - 2; i++) printf("═");
    printf("╗\n");
    printf("  ║%s%s%*s%s%*s%s ║\n",
            BOLD, BLUE, padding + length, title, CYAN, width - 2 - padding - length, "", CYAN);
    printf("  ╚");
    for (int i = 0; i < width - 2; i++) printf("═");
    printf("╝%s\n", RESET);
}

void showSuccess(const char *message) { printf("\n  %s%s✔  %s%s\n", BOLD, GREEN, message, RESET); }
void showError(const char *message)   { printf("\n  %s%s✘  %s%s\n", BOLD, RED, message, RESET); }
void showWarning(const char *message) { printf("\n  %s%s⚠  %s%s\n", BOLD, YELLOW, message, RESET); }

void printRule(int length)
{
    printf("  %s", DIM);
    for (int i = 0; i < length; i++) printf("─");
    printf("%s\n", RESET);
}

void printHeapifyHeader()
{
    printf("  %s%-6s %-5s %-7s %-7s %-7s %-10s %-14s%s\n",
           DIM, "Paso", "i", "A[i]", "izq", "der", "extremo", "Acción", RESET);
    printRule(66);
}

void printSiftUpHeader()
{
    printf("  %s%-6s %-5s %-8s %-9s %-10s %-14s%s\n",
           DIM, "Paso", "i", "A[i]", "padre", "A[padre]", "Acción", RESET);
    printRule(64);
}

/* ---------------------------------------------------------------- */
/* Heap index functions                                             */
/* ---------------------------------------------------------------- */
int parentIndex(int index)     { return (index - 1) / 2; }
int leftChildIndex(int index)  { return 2 * index + 1; }
int rightChildIndex(int index) { return 2 * index + 2; }

/* For max-heap, "higher priority" means greater; for min-heap, smaller. */
int hasHigherPriority(int firstValue, int secondValue)
{
    return isMaxHeap ? (firstValue > secondValue) : (firstValue < secondValue);
}

int nodeDepth(int index)
{
    int depth = 0;
    while (index > 0)
    {
        index = parentIndex(index);
        depth++;
    }
    return depth;
}

/* ---------------------------------------------------------------- */
/* Array view (Optimized for wrapping long arrays)                 */
/* ---------------------------------------------------------------- */
void printArray(int highlightA, int highlightB)
{
    if (heapSize == 0) { printf("  %s[ cola vacía ]%s\n", DIM, RESET); return; }

    int maxPerLine = 10; // Número máximo de elementos que quieres por fila

    for (int base = 0; base < heapSize; base += maxPerLine)
    {
        int limit = base + maxPerLine;
        if (limit > heapSize) limit = heapSize;

        // 1. Imprimir fila de Índices
        if (base == 0) {
            printf("  %sÍndice:%s ", DIM, RESET);
        } else {
            printf("          "); // 10 espacios en blanco para emular el ancho de "  Índice: "
        }
        
        for (int i = base; i < limit; i++) 
        {
            printf("%s%4d%s ", DIM, i, RESET);
        }
        printf("\n");

        // 2. Imprimir fila de Valores
        if (base == 0) {
            printf("  %sValor :%s ", DIM, RESET);
        } else {
            printf("          "); // 10 espacios en blanco para emular el ancho de "  Valor : "
        }

        for (int i = base; i < limit; i++)
        {
            const char *color = YELLOW;
            if      (i == highlightA) color = HL_A;
            else if (i == highlightB) color = HL_B;
            printf("%s%4d%s ", color, heapArray[i], RESET);
        }
        printf("\n");

        // Espacio extra de separación si quedan más bloques por delante
        if (limit < heapSize)
        {
            printf("\n");
        }
    }
}

/* ---------------------------------------------------------------- */
/* TREE DRAWING                                                     */
/* Extended canvas capacity to perfectly map up to 50 items         */
/* ---------------------------------------------------------------- */
#define NW 4
#define TH 60
#define TW 500
static char cell[TH][TW][16];

void clearCanvas(int height, int width)
{
    for (int row = 0; row < height && row < TH; row++)
        for (int column = 0; column < width && column < TW; column++)
            strcpy(cell[row][column], " ");
}

void putString(int row, int column, const char *text)
{
    if (row < 0 || row >= TH || column < 0 || column >= TW) return;
    strncpy(cell[row][column], text, 15);
    cell[row][column][15] = '\0';
}

/* Places a centered numeric label at column c, with optional color. */
void putLabel(int row, int column, int value, const char *color)
{
    char buffer[16];
    sprintf(buffer, "%d", value);
    int length = (int) strlen(buffer);

    for (int k = 0; k < length; k++)
    {
        char singleCell[16];
        if      (color && length == 1) sprintf(singleCell, "%s%c%s", color, buffer[k], RESET);
        else if (color && k == 0)      sprintf(singleCell, "%s%c",   color, buffer[k]);
        else if (color && k == length - 1) sprintf(singleCell, "%c%s", buffer[k], RESET);
        else                               sprintf(singleCell, "%c", buffer[k]);
        putString(row, column + k, singleCell);
    }
}

/* Horizontal position of a node, using the maximum tree depth. */
int centerX(int index, int maxDepth)
{
    int depth = nodeDepth(index);
    int firstIndexAtLevel = (1 << depth) - 1;
    int positionAtLevel = index - firstIndexAtLevel;
    int span = 1 << (maxDepth - depth);
    int startLeaf = positionAtLevel * span;
    int endLeaf = startLeaf + span - 1;
    int startColumn = startLeaf * NW + NW / 2;
    int endColumn = endLeaf * NW + NW / 2;
    return (startColumn + endColumn) / 2;
}

/* Top-down view: optimized to scale and trace horizontal charts up to 50 nodes. */
void drawTreeTopDown(int highlightA, int highlightB)
{
    int maxDepth = nodeDepth(heapSize - 1);
    int leafCount = 1 << maxDepth;
    int width = leafCount * NW;
    clearCanvas(2 * maxDepth + 2, width + 2);

    for (int i = 0; i < heapSize; i++)
    {
        int depth = nodeDepth(i);
        int center = centerX(i, maxDepth);
        int length = (heapArray[i] >= 10) ? 2 : 1;
        int column = center - length / 2;

        const char *color = NULL;
        if      (i == highlightA) color = HL_A;
        else if (i == highlightB) color = HL_B;
        putLabel(2 * depth, column, heapArray[i], color);

        int leftIndex = leftChildIndex(i);
        int rightIndex = rightChildIndex(i);
        int connectorRow = 2 * depth + 1;

        if (leftIndex < heapSize)
        {
            int leftX = centerX(leftIndex, maxDepth);
            if (rightIndex < heapSize)
            {
                int rightX = centerX(rightIndex, maxDepth);
                for (int columnIndex = leftX; columnIndex <= rightX; columnIndex++) putString(connectorRow, columnIndex, "─");
                putString(connectorRow, leftX, "┌");
                putString(connectorRow, rightX, "┐");
                putString(connectorRow, center, "┴");
            }
            else
            {
                for (int columnIndex = leftX; columnIndex <= center; columnIndex++) putString(connectorRow, columnIndex, "─");
                putString(connectorRow, leftX, "┌");
                putString(connectorRow, center, "┘");
            }
        }
    }

    for (int row = 0; row <= 2 * maxDepth; row++)
    {
        printf("  ");
        for (int column = 0; column < width; column++) printf("%s", cell[row][column]);
        printf("\n");
    }
}

/* Always print horizontally, regardless of depth or sizes */
void drawTree(int highlightA, int highlightB)
{
    if (heapSize == 0) { printf("  %s(árbol vacío)%s\n", DIM, RESET); return; }
    drawTreeTopDown(highlightA, highlightB);
}

/* ---------------------------------------------------------------- */
/* MAX/MIN-HEAPIFY                                                  */
/* ---------------------------------------------------------------- */
void heapify(int index, int showSteps, int *step)
{
    int leftIndex = leftChildIndex(index);
    int rightIndex = rightChildIndex(index);
    int bestIndex = index;

    if (leftIndex < heapSize && hasHigherPriority(heapArray[leftIndex], heapArray[bestIndex])) bestIndex = leftIndex;
    if (rightIndex < heapSize && hasHigherPriority(heapArray[rightIndex], heapArray[bestIndex])) bestIndex = rightIndex;

    if (showSteps)
    {
        char leftValue[8], rightValue[8];
        if (leftIndex < heapSize) sprintf(leftValue, "%d", heapArray[leftIndex]); else strcpy(leftValue, "-");
        if (rightIndex < heapSize) sprintf(rightValue, "%d", heapArray[rightIndex]); else strcpy(rightValue, "-");
        (*step)++;
        printf("  %-6d %-5d %s%-7d%s %-7s %-7s %sidx %-6d%s %s%-14s%s\n\n",
                *step, index, YELLOW, heapArray[index], RESET, leftValue, rightValue,
                MAGENTA, bestIndex, RESET,
                (bestIndex != index) ? GREEN : INFO,
                (bestIndex != index) ? "intercambia" : "en posición", RESET);
    }

    if (bestIndex != index)
    {
        int temporary = heapArray[index];
        heapArray[index] = heapArray[bestIndex];
        heapArray[bestIndex] = temporary;
        if (showSteps) { printArray(index, bestIndex); printf("\n"); }
        heapify(bestIndex, showSteps, step);
    }
}

/* ---------------------------------------------------------------- */
/* BUILD-MAX/MIN-HEAP                                               */
/* ---------------------------------------------------------------- */
void buildHeap(int showSteps)
{
    heapSize = initialSize;

    printf("\n  %s%s[2] CONSTRUCCIÓN DEL MONTÍCULO  (BUILD-%s-HEAP)%s\n\n",
            BOLD, BLUE, isMaxHeap ? "MAX" : "MIN", RESET);
    printf("  %sArreglo inicial:%s\n", BOLD, RESET);
    printArray(-1, -1);

    if (showSteps && initialSize <= 20)
    {
        for (int i = initialSize / 2 - 1; i >= 0; i--)
        {
            printf("\n  %s→ %s-HEAPIFY(A, %d)%s\n", CYAN, isMaxHeap ? "MAX" : "MIN", i, RESET);
            printHeapifyHeader();
            int step = 0;
            heapify(i, 1, &step);
        }
    }
    else
    {
        for (int i = initialSize / 2 - 1; i >= 0; i--) { int step = 0; heapify(i, 0, &step); }
        if (initialSize > 20)
            showWarning("Paso a paso omitido por tamaño; el montículo se construyó completo.");
    }

    printf("\n  %sMontículo resultante:%s\n", BOLD, RESET);
    printArray(-1, -1);
    printf("\n");
    drawTree(-1, -1);
}

/* ---------------------------------------------------------------- */
/* HEAP-PEEK (Consultar elemento de mayor prioridad sin extraer)   */
/* ---------------------------------------------------------------- */
void peek()
{
    if (heapSize < 1)
    {
        showError("La cola está vacía. No hay elementos que consultar.");
        return;
    }
    printf("\n  %sElemento en la cima (raíz):%s %s%d%s\n", 
           BOLD, RESET, GREEN, heapArray[0], RESET);
    printf("  %sPrioridad:%s %s\n", BOLD, RESET, isMaxHeap ? "MÁXIMA (Mayor valor)" : "MÍNIMA (Menor valor)");
}

/* ---------------------------------------------------------------- */
/* HEAP-EXTRACT  (dequeue: removes and returns the root)            */
/* ---------------------------------------------------------------- */
int dequeue(int showSteps)
{
    if (heapSize < 1)
    {
        showError("La cola está vacía (heap underflow).");
        return INT_MIN;
    }

    int root = heapArray[0];

    if (showSteps)
    {
        printf("\n  %sElemento de %s prioridad (raíz) = %s%d%s\n",
               BOLD, isMaxHeap ? "MÁXIMA" : "MÍNIMA", GREEN, root, RESET);
        printf("  %sSe coloca el último elemento A[%d]=%d en la raíz y se reduce el tamaño.%s\n\n",
               DIM, heapSize - 1, heapArray[heapSize - 1], RESET);
    }

    heapArray[0] = heapArray[heapSize - 1];
    heapSize--;

    if (showSteps && heapSize > 0)
    {
        printArray(0, -1);
        printf("\n  %s→ %s-HEAPIFY(A, 0) para restaurar la propiedad de montículo:%s\n",
               CYAN, isMaxHeap ? "MAX" : "MIN", RESET);
        printHeapifyHeader();
        int step = 0;
        heapify(0, 1, &step);
        printf("\n  %sMontículo actualizado:%s\n", BOLD, RESET);
        printArray(-1, -1);
        printf("\n");
        drawTree(-1, -1);
    }
    else if (heapSize > 0)
    {
        int step = 0;
        heapify(0, 0, &step);
    }

    return root;
}

/* ---------------------------------------------------------------- */
/* HEAP-INSERT  (enqueue: adds a value and moves it up)             */
/* ---------------------------------------------------------------- */
void enqueue(int key, int showSteps)
{
    if (heapSize >= capacity)
    {
        showError("La cola está LLENA: no se puede encolar.");
        return;
    }

    int insertedIndex = heapSize;
    heapArray[insertedIndex] = key;
    heapSize++;

    if (showSteps)
    {
        printf("\n  %sSe agrega %s%d%s como nueva hoja en la posición %d.%s\n",
               BOLD, GREEN, key, RESET, insertedIndex, RESET);
        printArray(insertedIndex, -1);
        printf("\n  %s→ Sift-up: se compara con el padre y sube mientras tenga mayor prioridad.%s\n",
               CYAN, RESET);
        printSiftUpHeader();
    }

    int index = insertedIndex;
    int step = 0;
    while (index > 0 && hasHigherPriority(heapArray[index], heapArray[parentIndex(index)]))
    {
        int parent = parentIndex(index);
        if (showSteps)
        {
            step++;
            printf("  %-6d %-5d %s%-8d%s %-9d %s%-10d%s %s%-14s%s\n",
                   step, index, YELLOW, heapArray[index], RESET, parent,
                   YELLOW, heapArray[parent], RESET, GREEN, "intercambia", RESET);
        }
        int temporary = heapArray[index];
        heapArray[index] = heapArray[parent];
        heapArray[parent] = temporary;
        index = parent;
        if (showSteps) { printArray(index, -1); printf("\n"); }
    }

    if (showSteps)
    {
        printf("  %sPosición final: índice %d.%s\n", DIM, index, RESET);
        printf("\n  %sMontículo actualizado:%s\n", BOLD, RESET);
        printArray(index, -1);
        printf("\n");
        drawTree(index, -1);
    }
}

/* ---------------------------------------------------------------- */
/* Dequeue all elements                                             */
/* ---------------------------------------------------------------- */
void dequeueAll()
{
    if (heapSize < 1) { showWarning("La cola ya está vacía."); return; }

    printf("\n  %sOrden de salida (de %s a %s prioridad):%s\n\n  ",
            BOLD, isMaxHeap ? "MAYOR" : "MENOR", isMaxHeap ? "menor" : "mayor", RESET);

    int isFirst = 1;
    while (heapSize > 0)
    {
        int value = dequeue(0);
        printf("%s%s%d%s", isFirst ? "" : " → ", GREEN, value, RESET);
        isFirst = 0;
    }
    printf("\n");
    showSuccess("Se desencolaron todos los elementos. La cola quedó vacía.");
    printf("  %sNota: extraer la raíz repetidamente equivale a HEAPSORT.%s\n", DIM, RESET);
}

/* ---------------------------------------------------------------- */
/* EXTRA: HEAPSORT demonstration over a copy                        */
/* ---------------------------------------------------------------- */
void heapifyArray(int *array, int size, int index)
{
    int leftIndex = 2 * index + 1;
    int rightIndex = 2 * index + 2;
    int bestIndex = index;

    if (leftIndex < size && (isMaxHeap ? array[leftIndex] > array[bestIndex] : array[leftIndex] < array[bestIndex])) bestIndex = leftIndex;
    if (rightIndex < size && (isMaxHeap ? array[rightIndex] > array[bestIndex] : array[rightIndex] < array[bestIndex])) bestIndex = rightIndex;

    if (bestIndex != index)
    {
        int temporary = array[index];
        array[index] = array[bestIndex];
        array[bestIndex] = temporary;
        heapifyArray(array, size, bestIndex);
    }
}

void HeapSort()
{
    if (heapSize < 1) { showWarning("La cola está vacía."); return; }

    int elementCount = heapSize;
    int *copyArray = malloc(elementCount * sizeof(int));
    if (!copyArray) { showError("Sin memoria."); return; }

    for (int i = 0; i < elementCount; i++) copyArray[i] = heapArray[i];

    printf("  %sEl montículo YA cumple la propiedad de heap, así que HEAPSORT%s\n", DIM, RESET);
    printf("  %ssolo intercambia la raíz con el último y reacomoda.%s\n\n", DIM, RESET);

    printf("  %sPaso 0  %s", DIM, RESET);
    for (int i = 0; i < elementCount; i++) printf("%s%3d%s ", YELLOW, copyArray[i], RESET);
    printf("\n");

    int activeSize = elementCount;
    for (int step = 1, i = elementCount - 1; i >= 1; i--, step++)
    {
        int temporary = copyArray[0];
        copyArray[0] = copyArray[i];
        copyArray[i] = temporary;

        activeSize--;
        heapifyArray(copyArray, activeSize, 0);

        printf("  %sPaso %-2d %s", DIM, step, RESET);
        for (int k = 0; k < elementCount; k++)
        {
            if (k >= activeSize) printf("%s%3d%s ", DIM, copyArray[k], RESET);
            else                 printf("%s%3d%s ", YELLOW, copyArray[k], RESET);
        }
        printf("\n");
    }

    printf("\n  %sArreglo ordenado (%s):%s  ",
            BOLD, isMaxHeap ? "ascendente" : "descendente", RESET);
    for (int i = 0; i < elementCount; i++) printf("%s%d%s ", GREEN, copyArray[i], RESET);
    printf("\n");
    showSuccess("HEAPSORT completado sobre una copia (la cola no se modificó).");

    free(copyArray);
}

/* ---------------------------------------------------------------- */
/* Safe integer reading                                             */
/* ---------------------------------------------------------------- */
int readInteger(int *destination)
{
    int value;
    if (scanf("%d", &value) != 1) { clearInputBuffer(); return 0; }
    *destination = value;
    return 1;
}

/* ---------------------------------------------------------------- */
/* Initial configuration                                            */
/* ---------------------------------------------------------------- */
void configure()
{
    showHeader("COLAS DE PRIORIDAD");
    printf("  %sPráctica 10 - Algoritmos y Estructuras de Datos%s\n\n", DIM, RESET);

    int option = 0;
    do {
        printf("  %sTipo de cola de prioridad:%s\n", BOLD, RESET);
        printf("    %s[1]%s Máxima prioridad (la raíz es el MAYOR)\n", CYAN, RESET);
        printf("    %s[2]%s Mínima prioridad (la raíz es el MENOR)\n\n", CYAN, RESET);
        printf("  %sSeleccione [1 o 2]:%s ", BOLD, RESET);
        if (!readInteger(&option)) option = 0;
        if (option != 1 && option != 2) {
            showError("Opción inválida. Seleccione estrictamente 1 o 2.");
            printf("\n");
        }
    } while (option != 1 && option != 2);

    isMaxHeap = (option == 2) ? 0 : 1;

    do {
        printf("\n  %s> Número de elementos (1 a 50):%s ", BOLD, RESET);
        if (!readInteger(&initialSize)) initialSize = 0;
    } while (initialSize < 1 || initialSize > 50);

    capacity = initialSize + 16;            /* leaves extra room for enqueue */
    if (capacity < 16) capacity = 16;

    heapArray = malloc(capacity * sizeof(int));
    if (!heapArray) { showError("Sin memoria."); exit(1); }

    srand((unsigned) time(NULL));
    for (int i = 0; i < initialSize; i++) heapArray[i] = rand() % 99 + 1;
    heapSize = initialSize;

    printf("\n  %s%s[1] VALORES ALEATORIOS EN EL ARREGLO (1 a 99)%s\n\n", BOLD, BLUE, RESET);
    printArray(-1, -1);

    buildHeap(1);
    pauseProgram();
}

/* ---------------------------------------------------------------- */
/* Main menu                                                        */
/* ---------------------------------------------------------------- */
void menu()
{
    int option = 0;
    while (option != 6)
    {
        showHeader("MENÚ - COLA DE PRIORIDAD");
        printf("  %sTipo:%s %s%s%s   %sElementos:%s %s%d%s / %d\n\n",
               BOLD, RESET, CYAN, isMaxHeap ? "MÁXIMA prioridad" : "MÍNIMA prioridad", RESET,
               BOLD, RESET, GREEN, heapSize, RESET, capacity);

        printf("    %s[1]%s  Encolar (insertar un elemento)\n", CYAN, RESET);
        printf("    %s[2]%s  Desencolar (extraer la raíz)\n", CYAN, RESET);
        printf("    %s[3]%s  Mostrar estado actual (Cola y Árbol)\n", CYAN, RESET);
        printf("    %s[4]%s  Desencolar TODOS los elementos\n", CYAN, RESET);
        printf("    %s[5]%s  Ordenamiento HEAPSORT\n", CYAN, RESET);
        printf("    %s[6]%s  Salir\n\n", RED, RESET);

        printf("  %sSeleccione una opción:%s ", BOLD, RESET);
        if (!readInteger(&option)) option = 0;

        switch (option)
        {
        case 1:
        {
            showHeader("ENCOLAR (HEAP-INSERT)");
            if (heapSize >= capacity) { showError("La cola está LLENA."); pauseProgram(); break; }
            int value;
            printf("  %s> Valor a encolar (1 a 99):%s ", BOLD, RESET);
            if (!readInteger(&value)) { showError("Entrada no válida."); pauseProgram(); break; }
            enqueue(value, 1);
            showSuccess("Elemento encolado correctamente.");
            pauseProgram();
            break;
        }
        case 2:
        {
            showHeader("DESENCOLAR (HEAP-EXTRACT)");
            if (heapSize < 1) { showError("La cola está vacía."); pauseProgram(); break; }
            int value = dequeue(1);
            printf("\n  %sValor que SALE de la cola:%s %s%d%s\n", BOLD, RESET, GREEN, value, RESET);
            pauseProgram();
            break;
        }
        case 3:
            showHeader("ESTADO DE LA COLA DE PRIORIDAD");
            printf("  %s[1] CONSULTAR CIMA (PEEK):%s\n", BOLD, RESET);
            peek();
            
            printRule(60);
            printf("\n  %s[2] ARREGLO EQUIVALENTE (VISTA DE COLA):%s\n", BOLD, RESET);
            printArray(-1, -1);
            printf("  %sTotal de elementos actuales:%s %s%d%s  (capacidad %d)\n",
                   BOLD, RESET, GREEN, heapSize, RESET, capacity);
            
            printRule(60);
            printf("\n  %s[3] VISTA EN ÁRBOL (ESTRUCTURA):%s\n\n", BOLD, RESET);
            drawTree(-1, -1);
            
            pauseProgram();
            break;

        case 4:
            showHeader("DESENCOLAR TODOS");
            dequeueAll();
            pauseProgram();
            break;

        case 5:
            showHeader("DEMOSTRACIÓN DE HEAPSORT");
            HeapSort();
            pauseProgram();
            break;

        case 6:
            free(heapArray);
            printf("\n%s", GREEN);
            printf("  ╔══════════════════════════════════════════════════╗\n");
            printf("  ║     ¡Hasta pronto!  Memoria liberada.            ║\n");
            printf("  ╚══════════════════════════════════════════════════╝\n");
            printf("%s\n", RESET);
            break;

        default:
            showError("Opción no válida.");
            pauseProgram();
            break;
        }
    }
}

int main()
{
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif
    configure();
    menu();
    return 0;
}