#include "funciones.h"

void imprimirResultados(FILE* pf, tRecursos* recursos)
{
    fprintf(pf, "\nIMPLEMENTAR FORMATO DE INFORME EN LA FUNCION imprimirResultado\n"
           );
}

void construccionNombreArchivoTxtInforme(char* NOMBRE_ARCHIVO_TXT_INFORME, unsigned tam, struct tm* fechaYHora)
{
    /** se utiliza la biblioteca time.h */
    snprintf(NOMBRE_ARCHIVO_TXT_INFORME, tam,
             "informe-juego_%4d-%02d-%02d-%02d-%02d.txt",
             fechaYHora->tm_year + 1900, // +1900 para sumarle la fecha base y que de 2024
             fechaYHora->tm_mon + 1,
             fechaYHora->tm_mday,
             fechaYHora->tm_hour,
             fechaYHora->tm_min
            );
}

int generarInforme(tRecursos* recursos, void (*construccionNombreArchivoTxtInforme)(char* NOMBRE_ARCHIVO_TXT_INFORME, unsigned tam, struct tm* fechaYHora))
{
    FILE* aInforme;
    char NOMBRE_ARCHIVO_TXT_INFORME[TAM_NOMBRE_ARCHIVO_INFORME];
    time_t tiempoTranscurrido;
    struct tm* fechaYHora;

    imprimirResultados(stdout, recursos);

    tiempoTranscurrido = time(NULL);
    fechaYHora = localtime(&tiempoTranscurrido);

    construccionNombreArchivoTxtInforme(NOMBRE_ARCHIVO_TXT_INFORME, sizeof(NOMBRE_ARCHIVO_TXT_INFORME), fechaYHora);

    if(!abrirArchivo(&aInforme, NOMBRE_ARCHIVO_TXT_INFORME, "wt"))
    {
        return NO_PUDE_ABRIR_ARCHIVO_TXT_INFORME;
    }

    imprimirResultados(aInforme, recursos);

    fclose(aInforme);

    return OK;
}
