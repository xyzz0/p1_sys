// Practica Sistemas Arturo Fernandez - Juan Carlos Oniate - David Ortega


/*
1- 3 Archivos (FileProcessor, Monitor , Config)
2- Un programa para procesar ficheros en busca de patrones ludopaticos.
3- Reportes de 4 casas (Bet365,Sportium,Bwin,Betfair)
4- De cada empresa se pueden recibir varios por dia.
5. Los ficheros tienen una estructura predefinida: IdApuesta;FECHA_INICIO;FECHA_FIN;IdUsuario;IdSesionJuego;Participación;Apuesta;Estado
6- Al final del dia se ejecuta montitor.

=================
ESPECIFICACIONES
=================

1. Debe ser parametrizable mediante fp.conf
2. La solucion debe procesar los ficheros de forma aleatoria y sin previo aviso.
3. El numero de hilos es proporcional al # casas de apuestas.
4. El directorio comun se especifica en fp.conf
5. FileProcessor esta siempre en ejecucion y se dispone de un pool de hilos.
6. Los hilos deben estar en ejecucion si hay ficheros en espera o si los ficheros no estan en el directorio.
7. Maximo 5 ficheros simultaneamente, y SIEMPRE todas las casas son atendidas a tiempo.
8. Los ficheros se procesaran 1 vez por 1 unico proceso.
9. Todos los printf(), se deben almacenar en un log.
10. Cada vez que llegue un fichero al directorio, esto debe mostratse por pantalla y escribirse en log.
11. Se pide crear y actualizar un fichero consolidado que muestre las actividades leidas, en CSV.
12. Se debe de poder configurar por parametro  el # maximo de ficheros a tratar simultanemente.
13. Monitor debe mostrar por pantalla a los ludopatas mediante tuberias.
14. Los patrones de comportamiento son hilos para buscar ese patron, la solucion incorpora 3 patrones definidos pero se debera poder agregar mas, los patrones solo deben reportar al proceso que los lance mientras que monitor es independiente.
15. Usar semaforos entre procesos para evitar erroes.
16. Cada proceso simula "un retardo pseudoaleatorio entre x e y segundos", usando los valores en el fichero de configuracion, (SIMULATE_SLEEP_MAX=X) (SIMULATE_SLEEP_MIN=X)

=========
PATRONES
=========

1. Patron : Tiempo de sesion diaria aumenta en poco tiempo
2. Patron: # Dinero apostado incrementa en poco tiempo
3. Patron: Ususario participa en diversas apuestas

========
ENTREGA
========

1. PDF MEMORIA (Portada,Indice,Apartados organizados,Explicacion,Esquema general, Plan de pruebas,Caputras de su funcionamiento,Codigo FUente)
*/

/*
    Dudas: 
        - El fichero csv lo deberia crear el programa o deberia estar previamente creado?
        - Debe ser FileProcess.C quien arranque Monitor, o tu te encargas de arrancarlo a la par que FP.c
        - Cuando deberia cerrarse el archivo de configuracion? Afectaria a los valores guardados en los valores si se cierra?
        - Los ficheros que envian las casas de apuestas contienen varios usuarios o solo el mismo con distintas acciones.
*/



// Librerias 

#include <stdio.h> // Standard I/0
#include <stdlib.h> // Standard Lib
#include <pthread.h> // Libreria Hilos
#include <unistd.h> // Libreria para Sleep
#include <sys/syscall.h> // Gettid()
#include <sys/stat.h> // Mkdir()

#define MaxThreads 5 // Definimos el numero maximo de hilos


// Funciones

void *procesar_Bet365() // Funcion para procesar Poker (Bet365)
{
    sleep(1);
    printf ("PID Bet365: %d\n", gettid());
}


void *procesar_Sportium() // Funcion para procesar BlackJack (Sportium) 
{
    sleep(2);
    printf ("PID Sportium: %d\n", gettid());
}


void *procesar_Bwin() // FUncion para procesar LaLiga (Bwin)
{
    sleep(3);
    printf ("PID BWIN: %d\n", gettid());
}


void *procesar_Betfair() // Funcion para procesar Ruleta (Betfair)
{
    sleep(4);
    printf("PID Betfair: %d\n", gettid());
}



int main()
{
    printf("\nTesteo Programa V.03\n\n");
    
    // fp.conf

    printf("\nAbriendo el archivo fp.conf y mostrando sus opciones:\n\n");

    FILE *fpconf; // Declaracion por puntero de fp.conf
    fpconf = fopen("fp.conf", "r"); // Abrimos en modo lectura fp.conf

    if ( fpconf == NULL ) // Aseguranza de fopen()
    {
        printf ("No se pudo abrir el archivo");
        return 1;
    }

        // PARAMETROS fp.conf

        char PATH_FILES[250];
        char INVENTORY_FILE[250];
        char LOG_FILE[250];
        int NUM_PROCESOS;
        int SIMULATE_SLEEP_MAX;
        int SIMULATE_SLEEP_MIN;

        // Leemos los parametros de configuracion de fp.conf

        fscanf(fpconf, "PATH_FILES=%249s\n", PATH_FILES);
        fscanf(fpconf, "INVENTORY_FILE=%249s\n", INVENTORY_FILE);
        fscanf(fpconf, "LOG_FILE=%249s\n", LOG_FILE);
        fscanf(fpconf, "NUM_PROCESOS=%d\n", &NUM_PROCESOS);
        fscanf(fpconf, "SIMULATE_SLEEP_MAX=%d\n", &SIMULATE_SLEEP_MAX);
        fscanf(fpconf, "SIMULATE_SLEEP_MIN=%d\n", &SIMULATE_SLEEP_MIN);

        // Otros ...

        printf (" PATH_FILES = %s\n", PATH_FILES);
        printf (" INVENTORY_FILE = %s\n", INVENTORY_FILE);
        printf (" LOG_FILE = %s\n", LOG_FILE);
        printf (" NUM_PROCESOS = %d\n", NUM_PROCESOS);
        printf (" SIMULATE_SLEEP_MAX = %d\n", SIMULATE_SLEEP_MAX);
        printf (" SIMULATE_SLEEP_MIN = %d\n",SIMULATE_SLEEP_MIN);
        

    fclose(fpconf); // Cerramos el archivo



    // Definimos los hilos

    pthread_t bet365; // [Poker]
    pthread_t sportium; // [BlackJack]
    pthread_t bwin; // [LaLiga]
    pthread_t betfair; // [Ruleta]
    pthread_t *hilos[4] = {&bet365, &sportium, &bwin, &betfair}; // Array de 4 componentes


    // Definimos las funciones para los hilos

    void *funcionesHilo[4] = {procesar_Bet365, procesar_Sportium, procesar_Bwin, procesar_Betfair}; // // Definimos las funciones para los hilos en un array de 4 componentes indicando las funciones de los hilos


    // Creamos los hilos

    for (int i = 0; i < 4; i++) // Bucle para la creacion de hilos
    {
        int threadError = pthread_create(hilos[i], NULL, funcionesHilo[i], NULL); // // Variable por si la creacion del hilo falla

        if (threadError != 0) // En caso de que falle la creacion
        {
            printf ("\nLa creacion del hilo %d, ha fallado.\n", i);
            return 1;
        }
    }


    // Otros ...

    printf("\n\nPID Main: %d\n\n", getpid());
    sleep(5);
    printf ("\n\nPrograma finalizado");


    return 0; // Cerramos el programa
    
}