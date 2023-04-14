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
        - Que hace realmente el programa? Le llegan datos , los envia a una carpeta aparte depues de haber consolidado todos los datos en el csv?
*/



// Librerias 

#include <stdio.h> // Standard I/0
#include <stdlib.h> // Standard Lib
#include <pthread.h> // Libreria Hilos
#include <unistd.h> // Libreria para Sleep
#include <sys/syscall.h> // Gettid()
#include <string.h> // Libreria de strings (Size measurement)
#include <sys/stat.h> // Mkdir()
#include <sys/inotify.h> // Escaneo en busca de archivos
#include <dirent.h> //libreria para trabajar con directorios


#define EVENT_SIZE (sizeof(struct inotify_event)) // INOTIFY
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16)) // INOTIFY

// Funciones

void* trabajo_Hilos()
{
    printf("\n Soy un hilo");
    pthread_exit(NULL);
}


// MAIN


int main()
{
    printf("\nTesteo Programa V.04\n\n");
    
    //registro.log
    FILE *log; //declaracion por puntero de registro.log

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

        // Imprimimos por pantalla los parametros de fp.conf
        printf (" PATH_FILES = %s\n", PATH_FILES);
        printf (" INVENTORY_FILE = %s\n", INVENTORY_FILE);
        printf (" LOG_FILE = %s\n", LOG_FILE);
        printf (" NUM_PROCESOS = %d\n", NUM_PROCESOS);
        printf (" SIMULATE_SLEEP_MAX = %d\n", SIMULATE_SLEEP_MAX);
        printf (" SIMULATE_SLEEP_MIN = %d\n",SIMULATE_SLEEP_MIN);
        
    fclose(fpconf); // Cerramos el archivo



    // Definimos el numero de hilos
    pthread_t hilos[NUM_PROCESOS];

    // Creamos los hilos
    for (int i = 0; i < 5; i++)
    {
        if (pthread_create(&hilos[i], NULL, trabajo_Hilos, NULL) != 0)
        {
            fprintf(stderr, "Error al crear el hilo numero %d\n", i);
        }
    }


    // Creacion del detector de nuevos archivos

    printf("\n\n Monitoreando %s en busca de nuevos archivos \n", PATH_FILES);

    int inotify_Instance = inotify_init(); // Se crea una instancia de inotify
    if (inotify_Instance < 0) // Bucle por si falla la creacion de la instancia
    {
        sleep(3);
        printf("Error al crear instancia de Inotify");
        return 1;
    }

    int vigilante_inotify = inotify_add_watch(inotify_Instance, PATH_FILES, IN_MOVED_TO);
    if (vigilante_inotify < 0 ) // En la linea anterior agregamos un vigilante de nuevos y archivos movidos.
    {
        sleep(3);
        printf("Error al crear el vigilante de inotify");
        return 1;
    }

    char buffer[EVENT_BUF_LEN]; // Creacion de un buffer para los eventos

    FILE *consolidado;
    
    char directorio[] = "."; // Directorio actual
    DIR *dir;

    struct dirent *entrada;

    // Subcadenas a buscar
    char subcadena[] = "CA"; 

    //fichero origen
    FILE *fichero_origen;

    while(1) // Bucle para gestionar los archivos
    {
        int length = read(inotify_Instance, buffer, EVENT_BUF_LEN);
        if (length < 0) // Leemos el buffer y controlamos errores.
        {
            printf("No se ha podido leer");
            return 1;
        }
    
        int t = 0;
        while (t < length)
        {
            struct inotify_event *event = (struct inotify_event *) &buffer[t];
            if (event->len)
            {
                if (event->mask & IN_MOVED_TO) //condicional en caso de que se agregue un archivo
                {
                    printf(" A new file was added: %s/%s\n", PATH_FILES, event->name);
                    log = fopen("registro.log", "a");
                    fprintf(log, "A new file was added: %s/%s\n", PATH_FILES, event->name);
                    fclose(log);

                    // Abrir directorio
                    dir = opendir(directorio);

                    if (dir == NULL) {
                        printf("No se pudo abrir el directorio.\n");
                        return 1;
                    }

                    // Leer contenido del directorio
                    printf("Contenido del directorio:\n");
                    
                    while ((entrada = readdir(dir)) != NULL) {
                        // Ignorar archivos ocultos y directorios
                        if (entrada->d_name[0] == '.') {
                            continue;
                        }

                        // Imprimir nombre de archivo
                        printf("%s\n", entrada->d_name);

                        // Buscar subcadena en nombre de archivo
                        if (strstr(entrada->d_name, subcadena) != NULL) {
                            printf("La subcadena '%s' fue encontrada en el archivo '%s'.\n", subcadena, entrada->d_name);

                            fichero_origen = fopen(entrada->d_name, "r");
                            consolidado = fopen("Consolidado_Ejemplo_v01.csv", "a");
                            
                            // Verifica si los archivos se abrieron correctamente
                            if (fichero_origen == NULL || consolidado == NULL) {
                                printf("Error al abrir los archivos.\n");
                                return 1;
                            }
                            
                            char linea[150]; //para almacenar cada linea del fichero y pasarlo al consolidado

                            // Lee linea por linea del archivo entrante y escribe en el consolidado
                            while (fgets(linea, 150, fichero_origen) != NULL) {
                                fputs(linea, consolidado);
                            }

                            // cerrar fichero
                            fclose(fichero_origen);
                            fclose(consolidado);
                            
                            if (remove(entrada->d_name) == 0) {
                                printf("Archivo eliminado correctamente.");
                            } else {
                                printf("Error al eliminar el archivo.");
                            }
                        }
                    }

                    // Cerrar directorio
                    closedir(dir);
                }
            }

            t += EVENT_SIZE + event->len;
        } 
    }


    // Otros ...
    printf("\n\nPID Main: %d\n\n", getpid());
    printf ("\n\nPrograma finalizado");


    return 0; // Cerramos el programa
    
}