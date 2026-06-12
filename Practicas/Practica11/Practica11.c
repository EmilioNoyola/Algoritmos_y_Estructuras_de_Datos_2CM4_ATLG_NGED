/*
    Práctica 11: Lista dinámica doblemente enlazada - Sistema de alumnos
    Alumnos:
     - Álvarez Tahuilán Luis Gustavo
     - Noyola Gómez Emilio Damian
    Fecha: 11 de junio de 2026
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
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

#define ARCHIVO_DEFECTO "listadoAlumnos.txt"

typedef struct Alumno
{
    char boleta[20];
    char nombre[40];
    char apellidos[40];
} Alumno;

typedef struct Nodo
{
    Alumno        info;
    struct Nodo  *ant;
    struct Nodo  *sig;
} Nodo;


void limpiarPantalla()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void leerLinea(char *buf, int size)
{
    if (fgets(buf, size, stdin) == NULL) { buf[0] = '\0'; printf("\n"); exit(0); }
    buf[strcspn(buf, "\r\n")] = '\0';
}

int leerOpcion()
{
    char b[64];
    leerLinea(b, sizeof(b));
    return atoi(b);
}

void pausar()
{
    char b[64];
    printf("\n  %s%sPresione ENTER para continuar...%s", DIM, CYAN, RESET);
    leerLinea(b, sizeof(b));
}

void mostrarEncabezado(const char *titulo)
{
    limpiarPantalla();
    int ancho = 62;
    int len   = (int) strlen(titulo);
    int pad   = (ancho - len - 2) / 2;

    printf("\n%s", CYAN);
    printf("  ╔");
    for (int i = 0; i < ancho - 2; i++) printf("═");
    printf("╗\n");
    printf("  ║%s%s%*s%s%*s%s ║\n",
           BOLD, BLUE, pad + len, titulo, CYAN, ancho - 2 - pad - len, "", CYAN);
    printf("  ╚");
    for (int i = 0; i < ancho - 2; i++) printf("═");
    printf("╝%s\n", RESET);
}

void mostrarExito(const char *msg)       { printf("\n  %s%s✔  %s%s\n", BOLD, GREEN,  msg, RESET); }
void mostrarError(const char *msg)       { printf("\n  %s%s✘  %s%s\n", BOLD, RED,    msg, RESET); }
void mostrarAdvertencia(const char *msg) { printf("\n  %s%s⚠  %s%s\n", BOLD, YELLOW, msg, RESET); }

int anchoVisible(const char *s)
{
    int n = 0;
    for (; *s; s++)
        if (((unsigned char)*s & 0xC0) != 0x80) n++;   
    return n;
}

void imprimirPad(const char *s, int ancho)
{
    int vis = 0;
    const char *p = s;
    while (*p && vis < ancho)
    {
        putchar(*p);
        p++;
        while (*p && (((unsigned char)*p & 0xC0) == 0x80)) { putchar(*p); p++; }
        vis++;
    }
    for (; vis < ancho; vis++) putchar(' ');
}

void imprimirCentrado(const char *s, int ancho)
{
    int vis  = anchoVisible(s);
    if (vis > ancho) vis = ancho;
    int izq  = (ancho - vis) / 2;
    int der  = ancho - vis - izq;
    for (int i = 0; i < izq; i++) putchar(' ');
    imprimirPad(s, vis);
    for (int i = 0; i < der; i++) putchar(' ');
}


void normalizar(const char *s, char *dest)
{
    int j = 0;
    for (int i = 0; s[i] && j < 63; i++)
    {
        unsigned char c = (unsigned char) s[i];

        if (c < 0x80)
        {
            if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
            dest[j++] = (char) c;
        }
        else if (c == 0xC3 && s[i+1])          
        {
            unsigned char d = (unsigned char) s[i+1];
            char r = 0;
            switch (d)
            {
                case 0xA1: case 0x81: r = 'a'; break; /* á Á */
                case 0xA9: case 0x89: r = 'e'; break; /* é É */
                case 0xAD: case 0x8D: r = 'i'; break; /* í Í */
                case 0xB3: case 0x93: r = 'o'; break; /* ó Ó */
                case 0xBA: case 0x9A: r = 'u'; break; /* ú Ú */
                case 0xBC: case 0x9C: r = 'u'; break; /* ü Ü */
                case 0xB1: case 0x91: r = 'n'; break; /* ñ Ñ */
                default:   r = (char) d;     break;
            }
            dest[j++] = r;
            i++;                                 /* consume el segundo byte */
        }
        /* otros bytes multibyte raros se ignoran */
    }
    dest[j] = '\0';
}

int compararAlumnos(const Alumno *a, const Alumno *b)
{
    char na[64], nb[64];
    normalizar(a->apellidos, na);
    normalizar(b->apellidos, nb);
    int c = strcmp(na, nb);
    if (c != 0) return c;
    normalizar(a->nombre, na);
    normalizar(b->nombre, nb);
    return strcmp(na, nb);
}


Nodo *crearNodo(Alumno a)
{
    Nodo *nuevo = (Nodo *) malloc(sizeof(Nodo));
    if (!nuevo) { mostrarError("Sin memoria."); exit(1); }
    nuevo->info = a;
    nuevo->ant  = NULL;
    nuevo->sig  = NULL;
    return nuevo;
}

int contar(Nodo *p)
{
    int n = 0;
    while (p) { n++; p = p->sig; }
    return n;
}

int posicionDe(Nodo *p, Nodo *objetivo)
{
    int pos = 1;
    while (p) { if (p == objetivo) return pos; pos++; p = p->sig; }
    return 0;
}


#define BW 14
#define SEP 6
void dibujarCajas(const char *labels[], const char *apes[], int n)
{
    /* etiquetas */
    printf("   ");
    for (int i = 0; i < n; i++)
    {
        imprimirCentrado(labels[i], BW + 2);
        if (i < n - 1) printf("%*s", SEP, "");
    }
    printf("\n");
    /* borde superior */
    printf("   ");
    for (int i = 0; i < n; i++)
    {
        printf("┌"); for (int k = 0; k < BW; k++) printf("─"); printf("┐");
        if (i < n - 1) printf("%*s", SEP, "");
    }
    printf("\n");
    /* contenido + flechas dobles (la flecha mide exactamente SEP columnas) */
    printf("   ");
    for (int i = 0; i < n; i++)
    {
        printf("│"); imprimirCentrado(apes[i], BW); printf("│");
        if (i < n - 1) printf(" %s◀──▶%s ", MAGENTA, RESET);
    }
    printf("\n");
    /* borde inferior */
    printf("   ");
    for (int i = 0; i < n; i++)
    {
        printf("└"); for (int k = 0; k < BW; k++) printf("─"); printf("┘");
        if (i < n - 1) printf("%*s", SEP, "");
    }
    printf("\n");
}

/* ---------------------------------------------------------------- */
/*  INSERCIÓN ORDENADA por apellidos                                */
/* ---------------------------------------------------------------- */
Nodo *insertarOrdenado(Nodo **p, Nodo **q, Alumno a, int animar)
{
    Nodo *nuevo = crearNodo(a);

    /* Lista vacía: p y q apuntan al nuevo nodo */
    if (*p == NULL)
    {
        *p = *q = nuevo;
        if (animar)
        {
            printf("\n  %sLa lista estaba vacía: %sp%s y %sq%s apuntan al nuevo nodo.%s\n",
                   DIM, BOLD, DIM, BOLD, DIM, RESET);
            const char *l[] = { "p, q → nuevo" };
            const char *ap[] = { a.apellidos };
            dibujarCajas(l, ap, 1);
            printf("  %s(nuevo->ant = NULL,  nuevo->sig = NULL)%s\n", DIM, RESET);
        }
        return nuevo;
    }

    Nodo *aux      = *p;
    Nodo *anterior = NULL;

    if (animar)
    {
        printf("\n  %sInsertando: %s%s%s  (%s)%s\n\n", BOLD, CYAN,
               a.apellidos, RESET, a.nombre, RESET);
        printf("  %s", DIM);
        imprimirPad("Paso", 6);
        imprimirPad("aux (apellido)", 18);
        imprimirPad("¿aux ≤ nuevo?", 16);
        printf("Acción%s\n", RESET);
    }

    int paso = 0;
    while (aux != NULL && compararAlumnos(&aux->info, &a) <= 0)
    {
        if (animar)
        {
            char ps[16];
            paso++;
            snprintf(ps, sizeof(ps), "%d", paso);
            printf("  "); imprimirPad(ps, 6);
            printf("%s", GREEN); imprimirPad(aux->info.apellidos, 18); printf("%s", RESET);
            imprimirPad("sí", 16);
            printf("%savanza →%s\n", BLUE, RESET);
        }
        anterior = aux;
        aux = aux->sig;
    }

    if (animar)
    {
        char ps[16];
        snprintf(ps, sizeof(ps), "%d", paso + 1);
        printf("  "); imprimirPad(ps, 6);
        printf("%s", YELLOW); imprimirPad(aux ? aux->info.apellidos : "(fin)", 18); printf("%s", RESET);
        imprimirPad("no", 16);
        printf("%sse detiene%s\n", RED, RESET);
    }

    if (anterior == NULL)                 /* insertar al INICIO */
    {
        nuevo->sig = aux;
        aux->ant   = nuevo;
        *p = nuevo;
        if (animar)
        {
            printf("\n  %s%sInsertar al inicio%s  (nuevo apellido es el menor)\n", BOLD, CYAN, RESET);
            const char *l[]  = { "p → nuevo", "aux (1° actual)" };
            const char *ap[] = { a.apellidos, aux->info.apellidos };
            dibujarCajas(l, ap, 2);
            printf("    %sp = nuevo;   nuevo->sig = aux;   aux->ant = nuevo;%s\n", DIM, RESET);
            printf("    %s(nuevo->ant = NULL)%s\n", DIM, RESET);
        }
    }
    else if (aux == NULL)                 /* insertar al FINAL */
    {
        anterior->sig = nuevo;
        nuevo->ant    = anterior;
        *q = nuevo;
        if (animar)
        {
            printf("\n  %s%sInsertar al final%s  (nuevo apellido es el mayor)\n", BOLD, CYAN, RESET);
            const char *l[]  = { "anterior", "q → nuevo" };
            const char *ap[] = { anterior->info.apellidos, a.apellidos };
            dibujarCajas(l, ap, 2);
            printf("    %sq = nuevo;   anterior->sig = nuevo;   nuevo->ant = anterior;%s\n", DIM, RESET);
            printf("    %s(nuevo->sig = NULL)%s\n", DIM, RESET);
        }
    }
    else                                  /* insertar EN MEDIO */
    {
        anterior->sig = nuevo;
        nuevo->ant    = anterior;
        nuevo->sig    = aux;
        aux->ant      = nuevo;
        if (animar)
        {
            printf("\n  %s%sInsertar en medio%s\n", BOLD, CYAN, RESET);
            const char *l[]  = { "anterior", "nuevo", "aux" };
            const char *ap[] = { anterior->info.apellidos, a.apellidos, aux->info.apellidos };
            dibujarCajas(l, ap, 3);
            printf("    %santerior->sig = nuevo;   nuevo->ant = anterior;%s\n", DIM, RESET);
            printf("    %snuevo->sig = aux;        aux->ant  = nuevo;%s\n", DIM, RESET);
        }
    }
    return nuevo;
}

/* ---------------------------------------------------------------- */
/*  BÚSQUEDA BIDIRECCIONAL por boleta (meet-in-the-middle)           */
/* ---------------------------------------------------------------- */
Nodo *buscarBidireccional(Nodo *p, Nodo *q, const char *boleta, int animar)
{
    Nodo *ini = p, *fin = q;

    if (animar)
    {
        printf("\n  %s%sBúsqueda bidireccional%s  (desde p →  y  ← desde q a la vez)\n\n",
               BOLD, BLUE, RESET);
        printf("  %s%-5s  %-22s  %-22s%s\n", DIM, "Paso", "desde inicio (p→)", "desde final (←q)", RESET);
    }

    int paso = 0;
    while (ini != NULL && fin != NULL)
    {
        paso++;
        if (animar)
            printf("  %-5d  %s%-22s%s  %s%-22s%s\n",
                   paso, GREEN, ini->info.boleta, RESET, CYAN, fin->info.boleta, RESET);

        if (strcmp(ini->info.boleta, boleta) == 0) return ini;
        if (strcmp(fin->info.boleta, boleta) == 0) return fin;
        if (ini == fin || ini->sig == fin) break;   /* se cruzaron */

        ini = ini->sig;
        fin = fin->ant;
    }
    return NULL;
}

/* ---------------------------------------------------------------- */
/*  ELIMINAR por boleta (4 casos: único, primero, último, medio)     */
/* ---------------------------------------------------------------- */
int eliminarPorBoleta(Nodo **p, Nodo **q, const char *boleta)
{
    Nodo *aux = *p;
    while (aux != NULL && strcmp(aux->info.boleta, boleta) != 0)
        aux = aux->sig;

    if (aux == NULL) return 0;            /* no encontrado */

    if (aux->ant == NULL && aux->sig == NULL)   /* único nodo */
    {
        *p = *q = NULL;
    }
    else if (aux->ant == NULL)                  /* primero */
    {
        *p = aux->sig;
        (*p)->ant = NULL;
    }
    else if (aux->sig == NULL)                  /* último */
    {
        *q = aux->ant;
        (*q)->sig = NULL;
    }
    else                                        /* en medio */
    {
        aux->ant->sig = aux->sig;
        aux->sig->ant = aux->ant;
    }
    free(aux);
    return 1;
}

void liberarLista(Nodo **p, Nodo **q)
{
    Nodo *aux = *p;
    while (aux) { Nodo *prox = aux->sig; free(aux); aux = prox; }
    *p = *q = NULL;
}


void dibujarCadena(Nodo *p, Nodo *q)
{
    if (p == NULL) { printf("  %s(lista vacía)%s\n", DIM, RESET); return; }

    printf("  %sNULL%s", DIM, RESET);
    int cont = 0;
    for (Nodo *aux = p; aux != NULL; aux = aux->sig)
    {
        const char *etq = aux->info.apellidos[0] ? aux->info.apellidos : aux->info.boleta;
        printf(" %s⇄%s %s%s%s", MAGENTA, RESET, GREEN, etq, RESET);
        cont++;
        if (cont % 5 == 0 && aux->sig) printf("\n      ");
    }
    printf(" %s⇄%s %sNULL%s\n", MAGENTA, RESET, DIM, RESET);

    const char *ep = p->info.apellidos[0] ? p->info.apellidos : p->info.boleta;
    const char *eq = q->info.apellidos[0] ? q->info.apellidos : q->info.boleta;
    printf("\n  %s⇄ = doble enlace (ant/sig).   p → %s%s%s,   q → %s%s%s\n",
           DIM, GREEN, ep, DIM, GREEN, eq, RESET);
}

void lineaTabla(const char *izq, const char *med, const char *der)
{
    int w[] = { 4, 14, 18, 18 };
    printf("  %s%s", CYAN, izq);
    for (int c = 0; c < 4; c++)
    {
        for (int k = 0; k < w[c]; k++) printf("═");
        printf("%s", (c < 3) ? med : der);
    }
    printf("%s\n", RESET);
}

void filaTabla(int num, const Alumno *a)
{
    char ns[8];
    snprintf(ns, sizeof(ns), "%d", num);
    printf("  %s║%s ", CYAN, RESET);          imprimirPad(ns, 2);
    printf(" %s│%s ", CYAN, RESET);            imprimirPad(a->boleta, 12);
    printf(" %s│%s ", CYAN, RESET);            imprimirPad(a->nombre, 16);
    printf(" %s│%s ", CYAN, RESET);            imprimirPad(a->apellidos, 16);
    printf(" %s║%s\n", CYAN, RESET);
}

void encabezadoTabla()
{
    lineaTabla("╔", "╤", "╗");
    printf("  %s║%s %s", CYAN, RESET, BOLD); imprimirPad("#", 2);
    printf("%s %s│%s %s", RESET, CYAN, RESET, BOLD); imprimirPad("Boleta", 12);
    printf("%s %s│%s %s", RESET, CYAN, RESET, BOLD); imprimirPad("Nombre", 16);
    printf("%s %s│%s %s", RESET, CYAN, RESET, BOLD); imprimirPad("Apellidos", 16);
    printf("%s %s║%s\n", RESET, CYAN, RESET);
    lineaTabla("╠", "╪", "╣");
}

void mostrarAscendente(Nodo *p)
{
    if (p == NULL) { mostrarAdvertencia("No hay alumnos en la lista."); return; }
    printf("  %s%sRecorrido ASCENDENTE  (desde p, siguiendo ->sig)%s\n\n", BOLD, BLUE, RESET);
    encabezadoTabla();
    int n = 1;
    for (Nodo *aux = p; aux != NULL; aux = aux->sig) filaTabla(n++, &aux->info);
    lineaTabla("╚", "╧", "╝");
}

void mostrarDescendente(Nodo *q)
{
    if (q == NULL) { mostrarAdvertencia("No hay alumnos en la lista."); return; }
    printf("  %s%sRecorrido DESCENDENTE  (desde q, siguiendo ->ant)%s\n\n", BOLD, BLUE, RESET);
    encabezadoTabla();
    int n = 1;
    for (Nodo *aux = q; aux != NULL; aux = aux->ant) filaTabla(n++, &aux->info);
    lineaTabla("╚", "╧", "╝");
}

/* ---------------------------------------------------------------- */
/*  Archivo: cargar y guardar                                        */
/* ---------------------------------------------------------------- */
void recortar(char *s)
{
    int n = (int) strlen(s);
    int i = 0;
    while (s[i] == ' ' || s[i] == '\t') i++;
    int j = n - 1;
    while (j >= i && (s[j] == ' ' || s[j] == '\t')) j--;
    int k = 0;
    for (int t = i; t <= j; t++) s[k++] = s[t];
    s[k] = '\0';
}

int existeBoleta(Nodo *p, const char *boleta)
{
    for (; p != NULL; p = p->sig)
        if (strcmp(p->info.boleta, boleta) == 0) return 1;
    return 0;
}

/* Parser robusto: detecta boleta, nombre y apellidos aunque los campos
   estén separados por TABULADORES o por ESPACIOS, y aunque el nombre y los
   apellidos vengan juntos en un solo campo. Devuelve 1 si obtuvo datos. */
int parsearLinea(const char *original, Alumno *a)
{
    char linea[256];
    strncpy(linea, original, sizeof(linea) - 1);
    linea[sizeof(linea) - 1] = '\0';
    linea[strcspn(linea, "\r\n")] = '\0';
    recortar(linea);

    memset(a, 0, sizeof(*a));
    if (linea[0] == '\0') return 0;

    /* boleta = primer token (hasta el primer espacio o tabulador) */
    int i = 0;
    while (linea[i] && linea[i] != ' ' && linea[i] != '\t') i++;
    int len = i < (int) sizeof(a->boleta) - 1 ? i : (int) sizeof(a->boleta) - 1;
    strncpy(a->boleta, linea, len);
    a->boleta[len] = '\0';

    /* resto = lo que sigue a la boleta */
    char resto[200];
    strncpy(resto, linea + i, sizeof(resto) - 1);
    resto[sizeof(resto) - 1] = '\0';
    recortar(resto);
    if (resto[0] == '\0') return 1;       /* solo había boleta */

    char *tab = strchr(resto, '\t');
    if (tab)                              /* nombre y apellidos separados por TAB */
    {
        *tab = '\0';
        char *ape = tab + 1;
        recortar(resto);
        recortar(ape);
        strncpy(a->nombre,    resto, sizeof(a->nombre) - 1);
        strncpy(a->apellidos, ape,   sizeof(a->apellidos) - 1);
    }
    else                                  /* vienen juntos: 1er palabra = nombre */
    {
        char *sp = strchr(resto, ' ');
        if (sp)
        {
            *sp = '\0';
            char *ape = sp + 1;
            recortar(ape);
            strncpy(a->nombre,    resto, sizeof(a->nombre) - 1);
            strncpy(a->apellidos, ape,   sizeof(a->apellidos) - 1);
        }
        else                              /* una sola palabra: se usa como apellido */
        {
            strncpy(a->apellidos, resto, sizeof(a->apellidos) - 1);
        }
    }
    return 1;
}

int cargarArchivo(Nodo **p, Nodo **q, const char *nombre)
{
    FILE *f = fopen(nombre, "r");
    if (!f) return -1;

    char linea[256];
    int  agregados = 0;

    while (fgets(linea, sizeof(linea), f))
    {
        Alumno a;
        if (!parsearLinea(linea, &a)) continue;
        if (a.boleta[0] == '\0')             continue;
        if (existeBoleta(*p, a.boleta))      continue;   /* evita duplicados por boleta */
        insertarOrdenado(p, q, a, 0);
        agregados++;
    }
    fclose(f);
    return agregados;
}

int guardarArchivo(Nodo *p, const char *nombre)
{
    FILE *f = fopen(nombre, "w");
    if (!f) return 0;
    for (Nodo *aux = p; aux != NULL; aux = aux->sig)
        fprintf(f, "%s\t%s\t%s\n", aux->info.boleta, aux->info.nombre, aux->info.apellidos);
    fclose(f);
    return 1;
}

/* ---------------------------------------------------------------- */
/*  Lectura/validación de datos de un alumno                         */
/* ---------------------------------------------------------------- */
int boletaValida(const char *s)
{
    if (!s || !*s) return 0;
    for (int i = 0; s[i]; i++)
        if (s[i] < '0' || s[i] > '9') return 0;
    return 1;
}

/* ---------------------------------------------------------------- */
/*  Opciones del menú                                                */
/* ---------------------------------------------------------------- */
void altaAlumno(Nodo **p, Nodo **q)
{
    mostrarEncabezado("ALTA DE ALUMNO");

    Alumno a;
    memset(&a, 0, sizeof(a));

    printf("  %s> Boleta:%s ", BOLD, RESET);
    leerLinea(a.boleta, sizeof(a.boleta));
    recortar(a.boleta);
    if (!boletaValida(a.boleta)) { mostrarError("La boleta debe contener solo dígitos."); pausar(); return; }

    if (buscarBidireccional(*p, *q, a.boleta, 0) != NULL)
    { mostrarError("Ya existe un alumno con esa boleta."); pausar(); return; }

    printf("  %s> Nombre:%s ", BOLD, RESET);
    leerLinea(a.nombre, sizeof(a.nombre));
    recortar(a.nombre);
    printf("  %s> Apellidos:%s ", BOLD, RESET);
    leerLinea(a.apellidos, sizeof(a.apellidos));
    recortar(a.apellidos);

    if (strlen(a.nombre) == 0 || strlen(a.apellidos) == 0)
    { mostrarError("Nombre y apellidos no pueden estar vacíos."); pausar(); return; }

    insertarOrdenado(p, q, a, 1);        /* con animación */

    printf("\n  %sLista actualizada (ordenada por apellidos):%s\n", BOLD, RESET);
    dibujarCadena(*p, *q);
    mostrarExito("Alumno dado de alta correctamente.");
    pausar();
}

void leerArchivo(Nodo **p, Nodo **q)
{
    mostrarEncabezado("LEER LISTA DESDE ARCHIVO");

    char nombre[128];
    printf("  %s> Nombre del archivo%s %s[%s]%s: ", BOLD, RESET, DIM, ARCHIVO_DEFECTO, RESET);
    leerLinea(nombre, sizeof(nombre));
    if (strlen(nombre) == 0) strcpy(nombre, ARCHIVO_DEFECTO);

    if (*p != NULL)
    {
        printf("\n  %sEsto reemplazará la lista actual. ¿Continuar? (s/n) %s[s]%s: ",
               BOLD, DIM, RESET);
        char r[16]; leerLinea(r, sizeof(r));
        if (r[0] == 'n' || r[0] == 'N') { mostrarAdvertencia("Operación cancelada."); pausar(); return; }
        liberarLista(p, q);
    }

    int n = cargarArchivo(p, q, nombre);
    if (n < 0) { mostrarError("No se pudo abrir el archivo."); pausar(); return; }

    printf("\n  %sSe leyeron %s%d%s registros desde %s%s%s.%s\n",
           BOLD, GREEN, n, RESET, CYAN, nombre, RESET, RESET);
    printf("\n  %sLista (ordenada por apellidos):%s\n", BOLD, RESET);
    dibujarCadena(*p, *q);
    mostrarExito("Archivo leído correctamente.");
    pausar();
}

void mostrarAlumnos(Nodo *p, Nodo *q)
{
    int op = 0;
    while (op != 4)
    {
        mostrarEncabezado("MOSTRAR ALUMNOS");
        printf("  %sTotal de alumnos:%s %s%d%s\n\n", BOLD, RESET, GREEN, contar(p), RESET);
        printf("    %s[1]%s  Listado ASCENDENTE  (A → Z)\n", CYAN, RESET);
        printf("    %s[2]%s  Listado DESCENDENTE (Z → A)\n", CYAN, RESET);
        printf("    %s[3]%s  Diagrama de la lista doblemente enlazada\n", CYAN, RESET);
        printf("    %s[4]%s  Volver\n\n", RED, RESET);
        printf("  %sSeleccione:%s ", BOLD, RESET);
        op = leerOpcion();

        switch (op)
        {
        case 1: mostrarEncabezado("LISTADO ASCENDENTE");  mostrarAscendente(p); pausar(); break;
        case 2: mostrarEncabezado("LISTADO DESCENDENTE"); mostrarDescendente(q); pausar(); break;
        case 3:
            mostrarEncabezado("DIAGRAMA DE LA LISTA");
            if (p == NULL) mostrarAdvertencia("La lista está vacía.");
            else dibujarCadena(p, q);
            pausar();
            break;
        case 4: break;
        default: mostrarError("Opción no válida."); pausar(); break;
        }
    }
}

void modificarAlumno(Nodo **p, Nodo **q)
{
    mostrarEncabezado("MODIFICACIÓN DE ALUMNO");
    if (*p == NULL) { mostrarAdvertencia("No hay alumnos para modificar."); pausar(); return; }

    char boleta[20];
    printf("  %s> Boleta del alumno a modificar:%s ", BOLD, RESET);
    leerLinea(boleta, sizeof(boleta));

    Nodo *nodo = buscarBidireccional(*p, *q, boleta, 1);
    if (nodo == NULL) { mostrarError("No existe un alumno con esa boleta."); pausar(); return; }

    printf("\n  %sAlumno encontrado:%s\n\n", BOLD, RESET);
    encabezadoTabla();
    filaTabla(posicionDe(*p, nodo), &nodo->info);
    lineaTabla("╚", "╧", "╝");

    /* Construimos los nuevos datos (ENTER = conservar el valor actual) */
    Alumno nuevo = nodo->info;
    char buf[40];

    printf("\n  %sNuevo nombre %s[%s]%s: ", BOLD, DIM, nodo->info.nombre, RESET);
    leerLinea(buf, sizeof(buf));
    if (strlen(buf) > 0) strncpy(nuevo.nombre, buf, sizeof(nuevo.nombre) - 1);

    printf("  %sNuevos apellidos %s[%s]%s: ", BOLD, DIM, nodo->info.apellidos, RESET);
    leerLinea(buf, sizeof(buf));
    if (strlen(buf) > 0) strncpy(nuevo.apellidos, buf, sizeof(nuevo.apellidos) - 1);

    int cambiaOrden = (compararAlumnos(&nodo->info, &nuevo) != 0);
    int posAntes    = posicionDe(*p, nodo);

    if (!cambiaOrden)
    {
        nodo->info = nuevo;              /* mismo lugar: actualización directa */
        mostrarExito("Datos actualizados. El alumno conserva su posición.");
        pausar();
        return;
    }

    /* El campo de ordenamiento cambió: reubicar para conservar el orden */
    printf("\n  %s%sVerificando posición...%s el apellido cambió, se reubica el alumno.\n",
           BOLD, YELLOW, RESET);

    eliminarPorBoleta(p, q, nodo->info.boleta);
    Nodo *reins = insertarOrdenado(p, q, nuevo, 0);
    int posDespues = posicionDe(*p, reins);

    printf("\n  %sReubicado de la posición %s%d%s a la posición %s%d%s.%s\n",
           BOLD, YELLOW, posAntes, RESET, GREEN, posDespues, RESET, RESET);
    printf("\n  %sLista actualizada:%s\n", BOLD, RESET);
    dibujarCadena(*p, *q);
    mostrarExito("Modificación aplicada. La lista sigue ordenada por apellidos.");
    pausar();
}

void guardar(Nodo *p)
{
    mostrarEncabezado("GUARDAR LISTA EN ARCHIVO");
    if (p == NULL) { mostrarAdvertencia("La lista está vacía; no hay nada que guardar."); pausar(); return; }

    char nombre[128];
    printf("  %s> Nombre del archivo%s %s[%s]%s: ", BOLD, RESET, DIM, ARCHIVO_DEFECTO, RESET);
    leerLinea(nombre, sizeof(nombre));
    if (strlen(nombre) == 0) strcpy(nombre, ARCHIVO_DEFECTO);

    if (guardarArchivo(p, nombre))
    {
        printf("\n  %sSe guardaron %s%d%s alumnos en %s%s%s.%s\n",
               BOLD, GREEN, contar(p), RESET, CYAN, nombre, RESET, RESET);
        mostrarExito("Lista guardada correctamente.");
    }
    else mostrarError("No se pudo escribir el archivo.");
    pausar();
}

/* ---------------------------------------------------------------- */
/*  Menú principal                                                   */
/* ---------------------------------------------------------------- */
int main()
{
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    Nodo *p = NULL, *q = NULL;

    /* Inicialmente, leer los registros desde el archivo */
    mostrarEncabezado("SISTEMA DE ALUMNOS - LISTA DOBLEMENTE ENLAZADA");
    printf("  %sPráctica 11 - Algoritmos y Estructuras de Datos%s\n", DIM, RESET);
    int n = cargarArchivo(&p, &q, ARCHIVO_DEFECTO);
    if (n < 0)
        mostrarAdvertencia("No se encontró '" ARCHIVO_DEFECTO "'. Se inicia con la lista vacía.");
    else
        printf("\n  %sSe cargaron %s%d%s registros iniciales desde %s%s%s.%s\n",
               BOLD, GREEN, n, RESET, CYAN, ARCHIVO_DEFECTO, RESET, RESET);
    pausar();

    int op = 0;
    while (op != 6)
    {
        mostrarEncabezado("MENÚ - SISTEMA DE ALUMNOS");
        printf("  %sAlumnos en la lista:%s %s%d%s\n\n", BOLD, RESET, GREEN, contar(p), RESET);
        printf("    %s[1]%s  Altas de alumnos\n", CYAN, RESET);
        printf("    %s[2]%s  Mostrar alumnos (ascendente / descendente)\n", CYAN, RESET);
        printf("    %s[3]%s  Modificaciones de alumnos (por boleta)\n", CYAN, RESET);
        printf("    %s[4]%s  Leer lista desde archivo\n", CYAN, RESET);
        printf("    %s[5]%s  Guardar lista en el archivo\n", CYAN, RESET);
        printf("    %s[6]%s  Salir\n\n", RED, RESET);
        printf("  %sSeleccione una opción:%s ", BOLD, RESET);
        op = leerOpcion();

        switch (op)
        {
        case 1: altaAlumno(&p, &q);        break;
        case 2: mostrarAlumnos(p, q);      break;
        case 3: modificarAlumno(&p, &q);   break;
        case 4: leerArchivo(&p, &q);       break;
        case 5: guardar(p);                break;
        case 6:
        {
            mostrarEncabezado("SALIR");
            if (p != NULL)
            {
                printf("  %s¿Guardar los cambios en %s%s%s antes de salir? (s/n):%s ",
                       BOLD, CYAN, ARCHIVO_DEFECTO, BOLD, RESET);
                char r[16]; leerLinea(r, sizeof(r));
                if (r[0] == 's' || r[0] == 'S')
                {
                    if (guardarArchivo(p, ARCHIVO_DEFECTO)) mostrarExito("Cambios guardados.");
                    else mostrarError("No se pudo guardar.");
                }
            }
            liberarLista(&p, &q);
            printf("\n%s", GREEN);
            printf("  ╔══════════════════════════════════════════════════╗\n");
            printf("  ║        ¡Hasta pronto!  Memoria liberada.         ║\n");
            printf("  ╚══════════════════════════════════════════════════╝\n");
            printf("%s\n", RESET);
            break;
        }
        default: mostrarError("Opción no válida."); pausar(); break;
        }
    }
    return 0;
}