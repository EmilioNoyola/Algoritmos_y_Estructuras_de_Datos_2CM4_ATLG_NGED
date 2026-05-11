/*
    Práctica 6: Listas estáticas
    Alumnos: 
     - Álvarez Tahuilán Luis Gustavo
     - Noyola Gómez Emilio Damian
    Fecha: 11 de mayo de 2026
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ALUMNOS 100
#define ARCHIVO_ENTRADA "listadoAlumnos.txt"
#define ARCHIVO_SALIDA  "listadoAlumnos.txt"

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
#define BG_BLUE "\033[44m"

typedef struct {
    long long boleta;
    char nombre[100];
} Alumno;

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
    printf("\n%s%s Presione ENTER para continuar...%s", DIM, CYAN, RESET);
    limpiarBuffer();
    getchar();
}

void mostrarExito(const char *msg) {
    printf("\n  %s%s  %s%s\n", BOLD, GREEN, msg, RESET);
}

void mostrarError(const char *msg) {
    printf("\n  %s%s  Error: %s%s\n", BOLD, RED, msg, RESET);
}

void mostrarAdvertencia(const char *msg) {
    printf("\n  %s%s  %s%s\n", BOLD, YELLOW, msg, RESET);
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

    printf("  ║%s%s%*s%s%*s%s║\n",
           BOLD, BLUE,
           pad + len, titulo,
           CYAN,
           ancho - 2 - pad - len, "",
           CYAN);

    printf("  ╚");
    for (int i = 0; i < ancho - 2; i++) printf("═");
    printf("╝%s\n\n", RESET);
}

void imprimirFilaAlumno(int pos, Alumno *a) {
    char boleta_str[20];
    char nombre_str[31];
    sprintf(boleta_str,  "%-13lld", a->boleta);
    snprintf(nombre_str, sizeof(nombre_str), "%-30s", a->nombre);

    printf("  %s║%s %s%3d%s %s║%s %s%s%s %s║%s %s%s%s %s║%s\n",
           CYAN, RESET,
           YELLOW, pos + 1, RESET,
           CYAN, RESET,
           GREEN, boleta_str, RESET,
           CYAN, RESET,
           WHITE, nombre_str, RESET,
           CYAN, RESET);
}

void imprimirCabeceraTabla() {
    printf("  %s╔═════╦═══════════════╦════════════════════════════════╗%s\n", CYAN, RESET);
    printf("  %s║%s %s%-3s%s %s║%s %s%-13s%s %s║%s %s%-30s%s %s║%s\n",
           CYAN, RESET,
           BOLD, "No.", RESET,
           CYAN, RESET,
           BOLD, "Boleta", RESET,
           CYAN, RESET,
           BOLD, "Nombre", RESET,
           CYAN, RESET);
    printf("  %s╠═════╬═══════════════╬════════════════════════════════╣%s\n", CYAN, RESET);
}

void imprimirPieTabla() {
    printf("  %s╚═════╩═══════════════╩════════════════════════════════╝%s\n", CYAN, RESET);
}

int listaLlena(int n, int tam) {
    if (n == tam) return 1;
    else          return 0;
}

int listaVacia(int n) {
    if (n == 0) return 1;
    else        return 0;
}

int buscar(Alumno lista[], int n, long long boleta) {
    for (int i = 0; i < n; i++)
        if (lista[i].boleta == boleta) return i;
    return -1;
}

void insertarFinal(Alumno lista[], int tam, int *n, Alumno nuevo) {
    if (!listaLlena(*n, tam)) {
        lista[*n] = nuevo;
        (*n)++;
    } else {
        printf("Lista llena\n");
    }
}

void insertar(Alumno lista[], int tam, int *n, Alumno nuevo, int pos) {
    if (!listaLlena(*n, tam) && pos >= 0 && pos <= *n) {
        for (int i = *n; i > pos; i--)
            lista[i] = lista[i - 1];
        lista[pos] = nuevo;
        (*n)++;
    } else {
        printf("Lista llena o posición inválida\n");
    }
}

Alumno eliminar(Alumno lista[], int *n, long long boleta, int *exito) {
    Alumno vacio = {0, ""};
    *exito = 0;

    if (listaVacia(*n)) return vacio;

    int pos = buscar(lista, *n, boleta);
    if (pos == -1) return vacio;

    Alumno eliminado = lista[pos];
    for (int i = pos; i < (*n) - 1; i++)
        lista[i] = lista[i + 1];
    (*n)--;

    *exito = 1;
    return eliminado;
}

int modificar(Alumno lista[], int n, long long boleta,
              long long nuevaBoleta, char nuevoNombre[],
              int cambiarBoleta, int cambiarNombre) {
    int pos = buscar(lista, n, boleta);
    if (pos == -1) return -1;

    if (cambiarBoleta) lista[pos].boleta = nuevaBoleta;
    if (cambiarNombre) strcpy(lista[pos].nombre, nuevoNombre);

    return pos;
}

int cargarArchivo(Alumno lista[], int tam, int *n) {
    FILE *f = fopen(ARCHIVO_ENTRADA, "r");
    if (!f) return 0;

    *n = 0;
    while (*n < tam) {
        long long bol;
        char nom[100];
        if (fscanf(f, "%lld", &bol) != 1) break;
        fgetc(f);
        if (!fgets(nom, sizeof(nom), f)) break;
        nom[strcspn(nom, "\n\r")] = '\0';
        lista[*n].boleta = bol;
        strcpy(lista[*n].nombre, nom);
        (*n)++;
    }
    fclose(f);
    return *n;
}

void guardarArchivo(Alumno lista[], int n) {
    FILE *f = fopen(ARCHIVO_SALIDA, "w");
    if (!f) {
        mostrarError("No se pudo abrir el archivo para escritura.");
        return;
    }
    for (int i = 0; i < n; i++)
        fprintf(f, "%lld\t%s\n", lista[i].boleta, lista[i].nombre);
    fclose(f);
    mostrarExito("Lista guardada exitosamente en \"" ARCHIVO_SALIDA "\".");
}

void menuMostrar(Alumno lista[], int n) {
    mostrarEncabezado("LISTADO COMPLETO DE ALUMNOS");

    if (listaVacia(n)) {
        mostrarAdvertencia("No hay alumnos registrados en la lista.");
        pausar();
        return;
    }

    imprimirCabeceraTabla();
    for (int i = 0; i < n; i++)
        imprimirFilaAlumno(i, &lista[i]);
    imprimirPieTabla();

    printf("\n  %sTotal de alumnos registrados: %s%d%s\n", BOLD, GREEN, n, RESET);
    pausar();
}

void menuInsertar(Alumno lista[], int tam, int *n) {
    mostrarEncabezado("DAR DE ALTA A UN ALUMNO");

    if (listaLlena(*n, tam)) {
        mostrarError("La lista está llena (máximo 100 alumnos).");
        pausar();
        return;
    }

    Alumno nuevo;
    printf("  %sBoleta del alumno:%s ", BOLD, RESET);
    scanf("%lld", &nuevo.boleta);
    limpiarBuffer();

    printf("  %sNombre completo:%s ", BOLD, RESET);
    fgets(nuevo.nombre, sizeof(nuevo.nombre), stdin);
    nuevo.nombre[strcspn(nuevo.nombre, "\n\r")] = '\0';

    printf("\n  %s¿Dónde insertar?%s\n", BOLD, RESET);
    printf("    %s[1]%s Al final de la lista\n", CYAN, RESET);
    printf("    %s[2]%s En una posición específica\n\n", CYAN, RESET);
    printf("  %sOpción:%s ", BOLD, RESET);

    int op;
    scanf("%d", &op);

    if (op == 2) {
        printf("\n  %sPosición (1-%d):%s ", BOLD, *n + 1, RESET);
        int pos;
        scanf("%d", &pos);
        pos--;

        if (pos < 0 || pos > *n) {
            mostrarError("Posición inválida. Se insertará al final.");
            insertarFinal(lista, tam, n, nuevo);
            printf("  %s→ Posición:%s %s%d%s\n", DIM, RESET, YELLOW, *n, RESET);
        } else {
            insertar(lista, tam, n, nuevo, pos);
            printf("\n");
            mostrarExito("Alumno insertado exitosamente.");
            printf("  %s→ Posición:%s %s%d%s\n", DIM, RESET, YELLOW, pos + 1, RESET);
        }
    } else {
        insertarFinal(lista, tam, n, nuevo);
        printf("\n");
        mostrarExito("Alumno agregado al final de la lista.");
        printf("  %s→ Posición:%s %s%d%s\n", DIM, RESET, YELLOW, *n, RESET);
    }

    printf("  %s→ Boleta:%s  %s%lld%s\n", DIM, RESET, GREEN, nuevo.boleta, RESET);
    printf("  %s→ Nombre:%s  %s%s%s\n",   DIM, RESET, WHITE, nuevo.nombre, RESET);
    pausar();
}

void menuBuscar(Alumno lista[], int n) {
    mostrarEncabezado("BUSCAR ALUMNO POR BOLETA");

    printf("  %sBoleta a buscar:%s ", BOLD, RESET);
    long long boleta;
    scanf("%lld", &boleta);

    int pos = buscar(lista, n, boleta);
    printf("\n");

    if (pos == -1) {
        mostrarAdvertencia("Alumno no encontrado en la lista.");
        printf("  %sBoleta buscada:%s %lld\n", DIM, RESET, boleta);
    } else {
        mostrarExito("¡Alumno encontrado!");
        printf("\n");
        imprimirCabeceraTabla();
        imprimirFilaAlumno(pos, &lista[pos]);
        imprimirPieTabla();
        printf("\n  %s→ Encontrado en la posición:%s %s%d%s de %s%d%s\n",
               DIM, RESET, GREEN, pos + 1, RESET, GREEN, n, RESET);
    }
    pausar();
}

void menuBorrar(Alumno lista[], int *n) {
    mostrarEncabezado("BORRAR ALUMNO POR BOLETA");

    if (listaVacia(*n)) {
        mostrarAdvertencia("La lista está vacía.");
        pausar();
        return;
    }

    printf("  %sBoleta del alumno a borrar:%s ", BOLD, RESET);
    long long boleta;
    scanf("%lld", &boleta);

    int pos = buscar(lista, *n, boleta);
    printf("\n");

    if (pos == -1) {
        mostrarAdvertencia("No se encontró ningún alumno con esa boleta.");
    } else {
        int exito;
        Alumno eliminado = eliminar(lista, n, boleta, &exito);

        mostrarExito("Alumno eliminado exitosamente.");
        printf("\n  %sDatos del alumno eliminado:%s\n", BOLD, RESET);
        printf("  %s│%s Boleta:  %s%lld%s\n", CYAN, RESET, GREEN,  eliminado.boleta, RESET);
        printf("  %s│%s Nombre:  %s%s%s\n",   CYAN, RESET, WHITE,  eliminado.nombre, RESET);
        printf("  %s│%s Pos.:    %s%d%s\n",   CYAN, RESET, YELLOW, pos + 1,          RESET);
        printf("\n  %sAlumnos restantes: %s%d%s\n", DIM, YELLOW, *n, RESET);
    }
    pausar();
}

void menuModificar(Alumno lista[], int n) {
    mostrarEncabezado("MODIFICAR DATOS DE UN ALUMNO");

    if (listaVacia(n)) {
        mostrarAdvertencia("La lista está vacía.");
        pausar();
        return;
    }

    printf("  %sBoleta del alumno a modificar:%s ", BOLD, RESET);
    long long boleta;
    scanf("%lld", &boleta);
    limpiarBuffer();

    int pos = buscar(lista, n, boleta);
    printf("\n");

    if (pos == -1) {
        mostrarAdvertencia("No se encontró ningún alumno con esa boleta.");
    } else {
        printf("  %sDatos actuales:%s\n", BOLD, RESET);
        imprimirCabeceraTabla();
        imprimirFilaAlumno(pos, &lista[pos]);
        imprimirPieTabla();

        printf("\n  %s¿Qué desea modificar?%s\n", BOLD, RESET);
        printf("    %s[1]%s Boleta\n", CYAN, RESET);
        printf("    %s[2]%s Nombre\n", CYAN, RESET);
        printf("    %s[3]%s Ambos\n\n", CYAN, RESET);
        printf("  %sOpción:%s ", BOLD, RESET);

        int op;
        scanf("%d", &op);
        limpiarBuffer();

        long long nuevaBoleta = lista[pos].boleta;
        char nuevoNombre[100];
        strcpy(nuevoNombre, lista[pos].nombre);

        int cambiarBoleta = (op == 1 || op == 3);
        int cambiarNombre = (op == 2 || op == 3);

        if (cambiarBoleta) {
            printf("\n  %sNueva boleta:%s ", BOLD, RESET);
            scanf("%lld", &nuevaBoleta);
            limpiarBuffer();
        }
        if (cambiarNombre) {
            printf("  %sNuevo nombre:%s ", BOLD, RESET);
            fgets(nuevoNombre, sizeof(nuevoNombre), stdin);
            nuevoNombre[strcspn(nuevoNombre, "\n\r")] = '\0';
        }

        modificar(lista, n, boleta, nuevaBoleta, nuevoNombre, cambiarBoleta, cambiarNombre);

        printf("\n");
        mostrarExito("Datos actualizados exitosamente.");
        printf("\n  %sDatos actualizados:%s\n", BOLD, RESET);
        imprimirCabeceraTabla();
        imprimirFilaAlumno(pos, &lista[pos]);
        imprimirPieTabla();
    }
    pausar();
}

void menuPrincipal(Alumno lista[], int tam, int *n) {
    int op = 0;

    while (op != 7) {
        mostrarEncabezado("GESTIÓN DE LISTA DE ALUMNOS");

        printf("  %s Alumnos en lista:%s %s%d / %d%s\n\n", BOLD, RESET, GREEN, *n, tam, RESET);
        printf("\n  %s Menú principal%s\n\n", BOLD, RESET);

        printf("    %s[1]%s  Dar de alta a un alumno\n",       CYAN, RESET);
        printf("    %s[2]%s  Buscar alumno por boleta\n",      CYAN, RESET);
        printf("    %s[3]%s  Borrar alumno por boleta\n",      CYAN, RESET);
        printf("    %s[4]%s  Modificar datos de un alumno\n",  CYAN, RESET);
        printf("    %s[5]%s  Mostrar todos los alumnos\n",     CYAN, RESET);
        printf("    %s[6]%s  Guardar lista en archivo\n",      CYAN, RESET);
        printf("    %s[7]%s  Salir\n",                         RED,  RESET);

        printf("\n  %s Seleccione una opción:%s ", BOLD, RESET);
        scanf("%d", &op);

        switch (op) {
            case 1: menuInsertar(lista, tam, n); break;
            case 2: menuBuscar(lista, *n);        break;
            case 3: menuBorrar(lista, n);         break;
            case 4: menuModificar(lista, *n);     break;
            case 5: menuMostrar(lista, *n);       break;
            case 6:
                guardarArchivo(lista, *n);
                pausar();
                break;
            case 7:
                mostrarEncabezado("CERRANDO SISTEMA");
                printf("  %s¿Guardar cambios antes de salir?%s\n", BOLD, RESET);
                printf("    %s[1]%s Si, guardar\n",             GREEN, RESET);
                printf("    %s[2]%s No, salir sin guardar\n\n", RED,   RESET);
                printf("  %sOpción:%s ", BOLD, RESET);
                int conf = 0;
                scanf("%d", &conf);
                if (conf == 1) guardarArchivo(lista, *n);

                printf("\n%s", GREEN);
                printf("  ╔══════════════════════════════════════════════════╗\n");
                printf("  ║        ¡Hasta pronto! Sistema cerrado.           ║\n");
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

int main() {
    #ifdef _WIN32
        system("chcp 65001 > nul");
    #endif

    Alumno lista[MAX_ALUMNOS];
    int n = 0;

    int leidos = cargarArchivo(lista, MAX_ALUMNOS, &n);

    if (leidos == 0) {
        mostrarAdvertencia("No se encontró el archivo o está vacío.");
    } else {
        mostrarExito("Lista cargada exitosamente.");
        printf("  %sAlumnos cargados:%s %s%d%s\n", DIM, RESET, CYAN, leidos, RESET);
    }

    pausar();
    menuPrincipal(lista, MAX_ALUMNOS, &n);

    return 0;
}