#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE* consolidado = fopen("consolidado.csv", "r");
    if (!consolidado) {
        printf("Error al abrir el consolidado.\n");
        return 1;
    }

    char linea[100];
    char* campo;

    char campo4_actual[20];
    char campo6_anterior[20];

    int contador_cambio_campo1 = 0;
    int cambio_campo4 = 0;

    while (fgets(linea, 100, consolidado)) {
        // Separar los campos de la linea
        campo = strtok(linea, ";");
        for (int i = 1; i < 4; i++) {
            campo = strtok(NULL, ";");
        }
        strcpy(campo4_actual, campo);

        for (int i = 0; i < 2; i++) {
            fgets(linea, 100, consolidado);
            campo = strtok(linea, ";");
            for (int j = 1; j < 4; j++) {
                campo = strtok(NULL, ";");
            }

            // Verificar si el campo 4 coincide y el campo 6 ha aumentado tres veces
            if (strcmp(campo, campo4_actual) == 0 && atoi(campo) == atoi(campo6_anterior) * 3) {
                cambio_campo4 = 1;
            }

            // Verificar si el campo 1 ha cambiado
            campo = strtok(NULL, ";");
            if (i == 1 && strcmp(campo4_actual, campo) == 0) {
                if (strcmp(campo, campo6_anterior) != 0) {
                    contador_cambio_campo1++;
                    if (contador_cambio_campo1 == 3) {
                        printf("el DNI %s cumple el patron 3.\n", campo4_actual);
                        contador_cambio_campo1 = 0;
                    }
                } else {
                    contador_cambio_campo1 = 0;
                }
            }

            strcpy(campo6_anterior, campo);
        }

        if (cambio_campo4) {
            printf("El DNI %s cumple el patron 2.\n", campo4_actual);
            cambio_campo4 = 0;
        }
        if

    }

    fclose(consolidado);

    return 0;
}