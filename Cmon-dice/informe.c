#include "funciones.h"

int comparaPuntosTotales(const void* a, const void* b)
{
    return ((tJugador*)a)->puntosTotales - *(unsigned*)b;
}

void imprimirSecuencia(void* pArchivo, void* vLetra, int* retornoCodigoDeError)
{
    fprintf(stdout, "\t%c\n", *(char*)(vLetra));
    fprintf((FILE*)pArchivo, "\t%c\n", *(char*)(vLetra));
}

void imprimirRonda(void* vERondaFinalizada, void* vRonda, int* retornoCodigoDeError)
{
    tRonda* ronda = (tRonda*)vRonda;
    tRondaFinalizadaInforme* rondaFinalizada = (tRondaFinalizadaInforme*)vERondaFinalizada;

    fprintf(stdout, "\tSecuencia %u:\n", ronda->cantidadDeCaracteresDeSecuencia);
    fprintf(*((FILE**)rondaFinalizada->archivo), "\tSecuencia %u:\n", ronda->cantidadDeCaracteresDeSecuencia);
    imprimirNNodosEnListaSimple(rondaFinalizada->secuenciaAsignada, *(rondaFinalizada->archivo), ronda->cantidadDeCaracteresDeSecuencia, NULL, imprimirSecuencia);

    fprintf(stdout, "\n\tRespuesta %u:\n", ronda->cantidadDeCaracteresDeSecuencia);
    fprintf(*((FILE**)rondaFinalizada->archivo), "\n\tRespuesta %u:\n", ronda->cantidadDeCaracteresDeSecuencia);
    mapEnListaSimple(&ronda->secuenciaIngresada, *(rondaFinalizada->archivo), NULL, imprimirSecuencia);

    fprintf(stdout, "\tCantidad de vidas usadas en ronda %u: %u.\n", ronda->cantidadDeCaracteresDeSecuencia, ronda->vidasUsadas);
    fprintf(*((FILE**)rondaFinalizada->archivo), "\tCantidad de vidas usadas en ronda %u: %u.\n", ronda->cantidadDeCaracteresDeSecuencia, ronda->vidasUsadas);

    fprintf(stdout, "\tPuntos obtenidos en ronda %u: %u.\n\n", ronda->cantidadDeCaracteresDeSecuencia, ronda->puntosObtenidos);
    fprintf(*((FILE**)rondaFinalizada->archivo), "\tPuntos obtenidos en ronda %u: %u.\n\n", ronda->cantidadDeCaracteresDeSecuencia, ronda->puntosObtenidos);
}

void imprimirJugador(void* pArchivo, void* vJugador, int* retornoCodigoDeError)
{
    tJugador* jugador = (tJugador*)vJugador;
    tRondaFinalizadaInforme rondaFinal;

    fprintf(stdout, "Jugador de ID: %u.\nNyA: %s.\nPuntos totales obtenidos: %u.\n\n",
        jugador->id,
        jugador->nya,
        jugador->puntosTotales
        );
    fprintf(*(FILE**)pArchivo, "Jugador de ID: %u.\nNyA: %s.\nPuntos totales obtenidos: %u.\n\n",
            jugador->id,
            jugador->nya,
            jugador->puntosTotales
            );
    rondaFinal.archivo = (FILE**)pArchivo;
    rondaFinal.secuenciaAsignada = &jugador->secuenciaAsignada;

    mapEnListaSimple(&jugador->rondasJugadas, &rondaFinal, NULL, imprimirRonda);
}

void imprimirResultados(FILE* pf, tRecursos* recursos)
{
    mapEnListaSimple(&recursos->listaDeJugadores, &pf, NULL, imprimirJugador);
}

void imprimirGanador(void* pArchivo, void* vJugador, int* retornoCodigoDeError)
{
    tJugador* jugador = (tJugador*)vJugador;

    fprintf(stdout, "\nJugador de ID: %u.\nNyA: %s.\n",
            jugador->id,
            jugador->nya
            );

    fprintf((FILE*)pArchivo, "\nJugador de ID: %u.\nNyA: %s.\n",
            jugador->id,
            jugador->nya
            );
}

void imprimirGanadores(FILE* pf, tRecursos* recursos)
{
    fprintf(stdout, "+-----------------------------------------------------------------------+\n");
    fprintf(pf, "+-----------------------------------------------------------------------+\n");
    if(recursos->mayorPuntajeTotal > 0)
    {
        fprintf(stdout, "El puntaje maximo alcanzado ha sido de %u punto/s.\nLista de ganadores:\n", recursos->mayorPuntajeTotal);
        fprintf(pf, "El puntaje maximo alcanzado ha sido de %u punto/s.\nLista de ganadores:\n", recursos->mayorPuntajeTotal);
        filtrarPorClaveEnListaSimple(&(recursos->listaDeJugadores), &(recursos->mayorPuntajeTotal), pf, NULL, comparaPuntosTotales, imprimirGanador);
    }
    else
    {
        fprintf(stdout, "No gano nadie, todos obtuvieron 0 puntos.\n");
        fprintf(pf, "No gano nadie, todos obtuvieron 0 puntos.\n");
    }
    fprintf(stdout, "+-----------------------------------------------------------------------+\n\n");
    fprintf(pf, "+-----------------------------------------------------------------------+\n\n");
}
//uso de time.h
void construccionNombreArchivoTxtInforme(char* NOMBRE_ARCHIVO_TXT_INFORME, unsigned tam, struct tm* fechaYHora)
{
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

    tiempoTranscurrido = time(NULL);
    fechaYHora = localtime(&tiempoTranscurrido);

    construccionNombreArchivoTxtInforme(NOMBRE_ARCHIVO_TXT_INFORME, sizeof(NOMBRE_ARCHIVO_TXT_INFORME), fechaYHora);

    if(!abrirArchivo(&aInforme, NOMBRE_ARCHIVO_TXT_INFORME, "wt"))
    {
        return NO_PUDE_ABRIR_ARCHIVO_TXT_INFORME;
    }

    imprimirResultados(aInforme, recursos);
    imprimirGanadores(aInforme, recursos);

    fclose(aInforme);

    return OK;
}
