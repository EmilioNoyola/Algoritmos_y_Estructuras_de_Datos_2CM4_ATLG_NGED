#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ALUMNOS 100
#define ARCHIVO_ENTRADA "listadoAlumnos.txt"
#define ARCHIVO_SALIDA "listadoAlumnos.txt"

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

int boleta;
char nombre[100];

}Alumno;

Alumno lista[MAX_ALUMNOS];
int total=0;

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

void pausar(){

    printf("\n%s%s Presione ENTER para continuar...%s", DIM, CYAN, RESET);
    limpiarBuffer();
    getchar();
  
    

}

void mostrarExito(const char *msg) {
    printf("\n  %s%s  %s%s\n", BOLD, GREEN, msg, RESET);
}

void mostrarError(const char *msg) {
    printf("\n  %s%s✘  Error: %s%s\n", BOLD, RED, msg, RESET);
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
    printf("  %s║%s %s%3d%s %s║%s %s%-13d%s %s║%s %s%-30s%s %s║%s\n",
           CYAN, RESET,
           YELLOW, pos + 1, RESET,
           CYAN, RESET,
           GREEN, a->boleta, RESET,
           CYAN, RESET,
           WHITE, a->nombre, RESET,
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

int cargarArchivo(){

FILE *f=fopen(ARCHIVO_ENTRADA, "r");
if(!f) return 0;
total=0;

while(total<MAX_ALUMNOS){
    int bol;
    char nom[100];

    if(fscanf(f, "%d", &bol)!=1) break;
    fgetc(f);

    if(!fgets(nom, sizeof(nom), f)) break;
    nom[strcspn(nom, "\n\r")]='\0';
    lista[total].boleta=bol;
    strcpy( lista[total].nombre, nom);

    total++;
}
fclose(f);
return total;
}

void guardarArchivo(){

    FILE *f=fopen(ARCHIVO_SALIDA, "w");
    if(!f){

    mostrarError("No se pudo abrir el archivo para escritura.");
    return;
    } 
    for(int i=0; i<total; i++){

    fprintf(f, "%d\t%s\n", lista[i].boleta, lista[i].nombre);

    }
    fclose(f);
    mostrarExito("Lista guardada exitosamente en \"" ARCHIVO_SALIDA "\".");
}

void mostrarLista(){
mostrarEncabezado("LISTADO COMPLETO DE ALUMNOS");
if(total==0){
    
    mostrarAdvertencia("No hay alumnos registrados en la lista.");
    pausar();
    return;
}
imprimirCabeceraTabla();
for(int i=0; i<total; i++){
    
    imprimirFilaAlumno(i, &lista[i]);
   
}
imprimirPieTabla();
printf("\n  %sTotal de alumnos registrados: %s%d%s\n", BOLD, GREEN, total, RESET);
pausar();
}


void insertarAlumno(){
mostrarEncabezado("DAR DE  ALTA A UN ALUMNO");
int op=0;

if(total>=MAX_ALUMNOS){

    mostrarError("La lista está llena (máximo " BOLD "100" RESET " alumnos).");
    pausar();
    return;
}

Alumno nuevo;
printf("  %sBoleta del alumno:%s ", BOLD, RESET);
scanf("%d", &nuevo.boleta);
limpiarBuffer();
printf("  %sNombre completo:%s ", BOLD, RESET);
fgets(nuevo.nombre, sizeof(nuevo.nombre), stdin);
nuevo.nombre[strcspn(nuevo.nombre, "\n\r")]='\0';

printf("\n  %s¿Dónde insertar?%s\n", BOLD, RESET);
    printf("    %s[1]%s Al final de la lista\n", CYAN, RESET);
    printf("    %s[2]%s En una posición específica\n\n", CYAN, RESET);
    printf("  %sOpción:%s ", BOLD, RESET);

    scanf("%d", &op);
    if(op==2){
    printf("\n  %sPosición (1-%d):%s ", BOLD, total + 1, RESET);
        int pos;
        scanf("%d", &pos);
        pos--;
        if(pos<0||pos>total){

        mostrarError("Posición inválida. Se insertará al final.");
        pos=total;
        }
    
        for(int i =total; i>pos;i--){

            lista[i]=lista[i-1];
            
        }
        lista[pos]=nuevo;
        total++;
        printf("\n");
        mostrarExito("Alumno insertado exitosamente.");
        printf("  %s→ Posición:%s %s%d%s\n", DIM, RESET, YELLOW, pos+1, RESET);
        
    }else{

        lista[total]=nuevo;
        total++;
        mostrarExito("Alumno agregado al final de la lista.");
        printf("  %s→ Posición:%s %s%d%s\n", DIM, RESET, YELLOW, total, RESET);

        }

    printf("  %s→ Boleta:%s  %s%d%s\n", DIM, RESET, GREEN, nuevo.boleta, RESET);
    printf("  %s→ Nombre:%s  %s%s%s\n", DIM, RESET, WHITE, nuevo.nombre, RESET);
    pausar();

}

int buscarBoleta(int boleta){

for(int i=0; i<total; i++){

    if(lista[i].boleta==boleta) return i;

}
return -1;

}

void buscarAlumno(){
mostrarEncabezado("BUSCAR ALUMNO POR BOLETA");
printf("  %sBoleta a buscar:%s ", BOLD, RESET);
    int boleta;
    scanf("%d", &boleta);
    int pos= buscarBoleta(boleta);
    printf("\n");
    if(pos==-1){
    mostrarAdvertencia("Alumno con boleta no encontrado en la lista.");
    printf("  %sBoleta buscada:%s %d\n", DIM, RESET, boleta);
    }else{
    
    mostrarExito("¡Alumno encontrado!");
    imprimirCabeceraTabla();
  imprimirFilaAlumno(pos, &lista[pos]);
    imprimirPieTabla();

        printf("\n  %s→ Encontrado en la posición:%s %s%d%s de %s%d%s\n",
               DIM, RESET, GREEN, pos + 1, RESET, GREEN, total, RESET);
    }
    
        pausar();
}

void borrarAlumno(){
mostrarEncabezado("BORRAR ALUMNO POR BOLETA");
if(total==0){
    mostrarAdvertencia("La lista está vacía.");
    pausar();
    return;
}
printf("  %sBoleta del alumno a borrar:%s ", BOLD, RESET);
    int boleta;
    scanf("%d", &boleta);

    int pos=buscarBoleta(boleta);
    printf("\n");

    if(pos==-1){

        mostrarAdvertencia("No se encontró ningún alumno con esa boleta.");
    }else{

    Alumno eliminado=lista[pos];

    for(int i=pos; i<total-1; i++){
        lista[i]=lista[i+1];

    }
    total--;
    mostrarExito("Alumno eliminado exitosamente.");
    printf("\n  %sDatos del alumno eliminado:%s\n", BOLD, RESET);
        printf("  %s│%s Boleta: %s%d%s\n", CYAN, RESET, GREEN, eliminado.boleta, RESET);
        printf("  %s│%s Nombre: %s%s%s\n", CYAN, RESET, WHITE, eliminado.nombre, RESET);
        printf("  %s│%s Pos.   : %s%d%s\n", CYAN, RESET, YELLOW, pos + 1, RESET);
        printf("\n  %sAlumnos restantes en lista: %s%d%s\n", DIM, YELLOW, total, RESET);
   }
   pausar();
}

void modificarAlumno(){
mostrarEncabezado("MODIFICAR DATOS DE UN ALUMNO");
if(total==0){

    mostrarAdvertencia("La lista está vacía.");
    pausar();
    return;
}

printf("  %sBoleta del alumno a modificar:%s ", BOLD, RESET);
    int boleta;
    scanf("%d", &boleta);
    limpiarBuffer();

    int pos=buscarBoleta(boleta);
    printf("\n");

    if(pos==-1){

        mostrarAdvertencia("No se encontró ningún alumno con esa boleta.");
    }else{

        printf("  %sDatos actuales:%s\n", BOLD, RESET);
        imprimirCabeceraTabla();
       imprimirFilaAlumno(pos, &lista[pos]);
       imprimirPieTabla();
         printf("\n  %s¿Que desea modificar?%s\n", BOLD, RESET);
    printf("    %s[1]%s Boleta\n", CYAN, RESET);
    printf("    %s[2]%s Nombre\n", CYAN, RESET);
    printf("    %s[3]%s Ambos\n", CYAN, RESET);
    printf("  %sOpcion:%s ", BOLD, RESET);
    int op=0;
    scanf("%d", &op);
    limpiarBuffer();
    if(op==1||op==3){
    printf("\n %sNueva boleta:%s ", BOLD, RESET);
    scanf("%d", &lista[pos].boleta);
    limpiarBuffer();
    }

    if(op==2||op==3){
    printf("\n %sNuevo nombre:%s ", BOLD, RESET);
    fgets(lista[pos].nombre, sizeof(lista[pos].nombre), stdin);
    lista[pos].nombre[strcspn(lista[pos].nombre, "\n\r")]='\0';
    
    }
    printf("\n");
    mostrarExito("Datos actualizados exitosamente.");
    printf("\n  %sDatos del alumno modificado:%s\n", BOLD, RESET);
    imprimirCabeceraTabla();
    imprimirFilaAlumno(pos, &lista[pos]);
    imprimirPieTabla();
        
    }

   
    pausar();
}

void menuPrincipal(){

int op=0;
while(op!=7){
mostrarEncabezado("GESTIÓN DE LISTA DE ALUMNOS  ");
printf("  %sAlumnos en lista:%s %s%d / %d%s\n\n", BOLD, RESET, GREEN, total, MAX_ALUMNOS, RESET);
printf("\n %sMenu principal%s\n\n", BOLD, RESET);

printf("    %s[1]%s  Dar de alta a un alumno\n", CYAN, RESET);
printf("    %s[2]%s  Buscar alumno por boleta\n", CYAN, RESET);
printf("    %s[3]%s  Borrar alumno por boleta\n", CYAN, RESET);
printf("    %s[4]%s  Modificar datos de un alumno\n", CYAN, RESET);
printf("    %s[5]%s  Mostrar todos los alumnos\n", CYAN, RESET);
printf("    %s[6]%s  Guardar lista en archivo\n", CYAN, RESET);
printf("    %s[7]%s  Salir\n", RED, RESET);
printf("\n  %sSeleccione una opcion:%s ", BOLD, RESET);
scanf("%d", &op);

switch(op){

case 1: insertarAlumno(); break;

case 2: buscarAlumno(); break;

case 3: borrarAlumno(); break;

case 4: modificarAlumno(); break;

case 5: mostrarLista(); break;

case 6: guardarArchivo(); pausar(); break;

case 7:
mostrarEncabezado("CERRANDO SISTEMA");
printf("  %s¿Guardar cambios antes de salir?%s\n", BOLD, RESET);
printf("    %s[1]%s Si, guardar\n", GREEN, RESET);
printf("    %s[2]%s No, salir sin guardar\n\n", RED, RESET);
printf("  %sopcion:%s ", BOLD, RESET);
int conf=0;
scanf("%d", &conf);

if(conf==1) guardarArchivo();

printf("\n%s", GREEN);
                printf("  ╔══════════════════════════════════════════════════╗\n");
                printf("  ║        ¡Hasta pronto! Sistema cerrado.          ║\n");
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


int main(){

#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

int leidos=cargarArchivo();
if(leidos==0){

    mostrarAdvertencia("No se encontró el archivo o está vacío.");
}else{
    mostrarExito("Lista cargada exitosamente.");
    printf("  %sAlumnos cargados:%s %s%d%s\n", DIM, RESET, CYAN, leidos, RESET);
}
pausar();
menuPrincipal();

return 0;
}