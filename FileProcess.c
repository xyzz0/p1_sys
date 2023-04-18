// Practica Sistemas Arturo Fernandez - Juan Carlos Oniate - David Ortega

// Librerias 
#include <stdio.h> // Standard I/0
#include <stdlib.h> // Standard Lib
#include <pthread.h> // Libreria Hilos
#include <unistd.h> // Libreria para Sleep
#include <sys/syscall.h> // Gettid()
#include <string.h> // Libreria de strings (Size measurement)
#include <sys/stat.h> // Mkdir()
#include <sys/inotify.h> // Escaneo en busca de archivos
#include <semaphore.h> // Semaforos

#define EVENT_SIZE (sizeof(struct inotify_event)) // INOTIFY
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16)) // INOTIFY

// Sleep aleatorio
int rand_sleep;

// Declaracion por puntero del nombre del fichero entrante
char *nombreF_Entrante;

// PARAMETROS fp.conf
char PATH_FILES[250];
char INVENTORY_FILE[250];
char LOG_FILE[250];
int NUM_PROCESOS;
int SIMULATE_SLEEP_MAX;
int SIMULATE_SLEEP_MIN;

int NUM_PROCESOS_GLOBAL;


// Creamos en base a NP
pthread_t* hilos;

// Semaforo Global
//sem_t semaforoGlobal;

// Semaforos individuales
sem_t* semaforo;


// Funciones
void* trabajo_Hilos()
{
    //sem_wait(&semaforoGlobal); // El semaforo global espera

    for (int i = 0; i < NUM_PROCESOS; i++) // Esperan todos los semaforos individuales
    {
        if (pthread_equal(hilos[i], pthread_self()))
        {
            sem_wait(&semaforo[i]);
            printf("Accede el hilo %d, con el archivo %s", i, nombreF_Entrante);
        }
    }

    // Agregar al consolidado
    FILE *f_entrante; // Declaracion por puntero del fichero entrante
    FILE *consolidado; // Declaracion por puntero del consolidado

    f_entrante = fopen(nombreF_Entrante, "r"); // Abrimos el fichero entrante en modo lectura
    if (f_entrante == NULL ) // Aseguranza de f_entrante
    {
        printf(" No se pudo abrir el fichero entrante");
        exit(1);
    }

    consolidado = fopen(INVENTORY_FILE, "a"); // Abrimos el consolidado
    if (consolidado == NULL) // Aseguranza de consolidado
    {
        printf (" No se pudo abrir el fichero consolidado");
        exit(1);
    }

    char linea[150]; // Array para almacenar las lineas de los ficheros entrantes

    // Una vez abiertos consolidado y f_entrante escribimos en consolidado
    while (fgets(linea, 150, f_entrante) != NULL) { fputs(linea, consolidado) ;}     // Lee linea por linea del archivo entrante y escribe en el consolidado

    fclose(consolidado); // Cerramos el consolidado
    fclose(f_entrante); // Cerramos el archivo entrante

    // Borrar el archivo 
    if (remove(nombreF_Entrante) == 0) { printf(" %s, ha sido borrado", nombreF_Entrante);} 
    else { printf(" No ha sido posible borrar %s", nombreF_Entrante); }

}


// MAIN
int main()
{

    printf("\nTesteo Programa V.05\n\n");
 
    // Archivo log
    FILE *log; // Declaracion por puntero del log

    // fp.conf
    printf("\nAbriendo el archivo fp.conf y mostrando sus opciones:\n\n");

    FILE *fpconf; // Declaracion por puntero de fp.conf
    fpconf = fopen("fp.conf", "r"); // Abrimos en modo lectura fp.conf

    if ( fpconf == NULL ) // Aseguranza de fopen()
    {
        printf ("\n No se pudo abrir fpconf, asegurese de que se encuentra en el mismo directorio que FileProcess");
        exit(1);
    }

        // Leemos los parametros de configuracion de fp.conf
        fscanf(fpconf, "PATH_FILES=%249s\n", PATH_FILES);
        fscanf(fpconf, "INVENTORY_FILE=%249s\n", INVENTORY_FILE);
        fscanf(fpconf, "LOG_FILE=%249s\n", LOG_FILE);
        fscanf(fpconf, "NUM_PROCESOS=%d\n", &NUM_PROCESOS);
        fscanf(fpconf, "SIMULATE_SLEEP_MAX=%d\n", &SIMULATE_SLEEP_MAX);
        fscanf(fpconf, "SIMULATE_SLEEP_MIN=%d\n", &SIMULATE_SLEEP_MIN);

        // Imprimimos por pantalla los parametros de fp.conf
        printf (" PATH_FILES = %s\n", PATH_FILES);
        printf (" INVENTORY_FILE = %s\n", INVENTORY_FILE);
        printf (" LOG_FILE = %s\n", LOG_FILE);
        printf (" NUM_PROCESOS = %d\n", NUM_PROCESOS);
        printf (" SIMULATE_SLEEP_MAX = %d\n", SIMULATE_SLEEP_MAX);
        printf (" SIMULATE_SLEEP_MIN = %d\n",SIMULATE_SLEEP_MIN);
        
    fclose(fpconf); // Cerramos el archivo

    // Array de strings
    char CA[5][69]; // Referencia picantona marlon  >;)
    strcpy(CA[0],"CA001");
    strcpy(CA[1],"CA002");
    strcpy(CA[2],"CA003");
    strcpy(CA[3],"CA004");
    strcpy(CA[4],"CA005");

    semaforo = (sem_t*)malloc(NUM_PROCESOS * sizeof(sem_t)); // Malloc de semaforo
    hilos = malloc(NUM_PROCESOS * sizeof(pthread_t)); // Malloc de hilos

    // Inicializar el semaforo global
    //sem_init(&semaforoGlobal, 0, 0);

    // Inicializamos los semaforos a 0
    for ( int i = 0; i < NUM_PROCESOS; i++ )
    {
        sem_init(&semaforo[i], 0, 0);
    }

    // Creamos los hilos
    for (int i = 0; i < NUM_PROCESOS; i++)
    {
        if (pthread_create(&hilos[i], NULL, trabajo_Hilos, (void*)nombreF_Entrante) != 0)
        {
            fprintf(stderr, "Error al crear el hilo numero %d\n", i);
        }
    }

    //sem_post(&semaforoGlobal); // Avanzamos en el semaforo Global
        
    if (hilos == NULL) {
        printf("No se pudo asignar memoria para el array de hilos\n");
        exit(1);
    }

    // Creacion del detector de nuevos archivos
    printf("\n\n Monitoreando %s en busca de nuevos archivos ...\n", PATH_FILES);


    int inotify_Instance = inotify_init(); // Se crea una instancia de inotify
    if (inotify_Instance < 0) // Bucle por si falla la creacion de la instancia
    {
        sleep(3);
        printf("Error al crear instancia de Inotify");
        exit(1);
    }

    int vigilante_inotify = inotify_add_watch(inotify_Instance, PATH_FILES, IN_MOVED_TO);
    if (vigilante_inotify < 0 ) // En la linea anterior agregamos un vigilante de nuevos y archivos movidos.
    {
        sleep(3);
        printf("Error al crear el vigilante de inotify");
        exit(1);
    }

    char buffer[EVENT_BUF_LEN]; // Creacion de un buffer para los eventos

    while(1) // Bucle para gestionar los archivos
    {
        int length = read(inotify_Instance, buffer, EVENT_BUF_LEN);
        if (length < 0) // Leemos el buffer y controlamos errores.
        {
            printf(" No se ha podido leer el directorio");
            exit(1);
        }
    
        int i = 0;
        while (i < length)
        {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->len)
            {
                if (event->mask & IN_MOVED_TO) // Si se agrega un archivo > condicional
                {
                    srand(time(NULL)); //Numero aleatorio para sleep
                    int dif=SIMULATE_SLEEP_MAX-SIMULATE_SLEEP_MIN;
                    rand_sleep = rand()%(dif+1)+SIMULATE_SLEEP_MIN;   

                    // Cuando llega un archivo  CA, se imprime en el log
                    printf("\n Se ha agregado un nuevo archivo: %s/%s\n", PATH_FILES, event->name);
                    log = fopen(LOG_FILE, "a"); // Abrimos el log en modo append
                    if (log == NULL) // Aseguranza de log
                    {
                        printf("No se pudo abrir el log");
                        exit(1);
                    }

                    fprintf(log, "Se ha agregado un nuevo archivo: %s%s\n", PATH_FILES, event->name); // Se agregan datos al log
                    fclose(log); // Se cierra el fichero log

                    printf("\n El nombre del archivo es: %s", nombreF_Entrante);
                    for (int j = 0; j < NUM_PROCESOS; j++)
                    {
                        if(strstr(event->name, CA[j]) != NULL)
                        { 
                            nombreF_Entrante = event->name;
                            sem_post(&semaforo[j]); // Liberamos el semaforo
                        }
                    }
                }
            }
         i += EVENT_SIZE + event->len;
        }   
    } 

    printf ("\n\nPrograma finalizado");

    // Destruimos los semaforos
    //sem_destroy(&semaforoGlobal);
    for (int i = 0; i < NUM_PROCESOS; i++)
    {
    sem_destroy(&semaforo[i]);
    }

    // Liberamos memoria
    free(hilos);
    free(semaforo);
    
    // Cerramos el programa
    return 0;
}
