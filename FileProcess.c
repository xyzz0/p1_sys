#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

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
