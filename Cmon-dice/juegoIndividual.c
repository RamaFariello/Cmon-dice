#include "funciones.h"

void mostrarCaracter(const void* dato)
{
    printf("%c\n", *(char*)dato);
}

/// generarRondas es por CADA RONDA del jugador.
int generaRondas(tRecursos* recursos, tJugador* jugador, int* retornoCodigoDeError) ///ESTA FUNCION ANDA BIEN[NO CAMBIAR!]
{
    int cantidadDeVidasDelJugador = recursos->configuraciones[recursos->indiceDeNivelDeConfiguracionElegida].cantidadDeVidas;
    unsigned tiempoParaVisualizarSecuencia = recursos->configuraciones[recursos->indiceDeNivelDeConfiguracionElegida].tiempoDeVisualizacionSecuenciaCorrecta;
    unsigned tiempoParaIngresarSecuencia = recursos->configuraciones[recursos->indiceDeNivelDeConfiguracionElegida].tiempoRespuestaPorRonda;
    char letra;

    tRonda ronda;///puntosObtenidos - vidasUsadas
    int juegoRondaActual;

    *retornoCodigoDeError = OK;
    ronda.cantidadDeCaracteresDeSecuencia = 0;
    while(cantidadDeVidasDelJugador >= 0)
    {
        ///inicializo el tRonda
        juegoRondaActual = 1;
        ronda.puntosObtenidos = 0;
        ronda.vidasUsadas = 0;
        crearListaSimple(&(ronda.secuenciaIngresada));///CREO UNA LISTA DE SECUENCIA INGRESADA POR RONDA -> LA VACIO en funcion que libera lista de Rondas por jugador

        if(OK != (*retornoCodigoDeError = pedirLetraAleatoria(recursos, &letra))) //si no recibo una letra válida
        {
            fprintf(stderr, "No pude obtener letra aleatoria para formar secuencia.\n");
            vaciarListaSimple(&(ronda.secuenciaIngresada));
            return *retornoCodigoDeError;
        }

        insertarAlFinalEnListaSimple(&(jugador->secuenciaAsignada), &letra, sizeof(char));
        (ronda.cantidadDeCaracteresDeSecuencia)++;

        while(FIN_DE_RONDA_ACTUAL != juegoRondaActual)
        {
            mostrarSecuenciaAsignada(recursos, jugador, tiempoParaVisualizarSecuencia);
            juegoRondaActual = ingresoDeSecuencia(recursos, jugador, &ronda, &cantidadDeVidasDelJugador, ronda.cantidadDeCaracteresDeSecuencia, tiempoParaIngresarSecuencia);
        }

        tiempoParaVisualizarSecuencia++;//POR CADA RONDA, LE SUMO 1 SEGUNDO EXTRA
        tiempoParaIngresarSecuencia++; //POR CADA RONDA, LE SUMO 1 SEGUNDO EXTRA

        jugador->puntosTotales += ronda.puntosObtenidos;
        insertarAlFinalEnListaSimple(&(jugador->rondasJugadas), &ronda, sizeof(tRonda));///INSERTO CADA UNA DE LAS RONDAS JUGADAS
    }

    recursos->mayorPuntajeTotal = MAYOR(recursos->mayorPuntajeTotal, jugador->puntosTotales);

    (recursos->cantidadDeJugadores) --;///AYUDO A CONSUMO API PARA SER MAS EFICIENTE.

    return *retornoCodigoDeError;
}

/// jugarRondas es UNA VEZ por jugador.
void jugarRondas(void* vRecursos, void* vJugador, int* retornoCodigoDeError) //ex funcion inicializarJugador[esta funcion es la que se ejecuta en el map]
{
    tRecursos* recursos = (tRecursos*)vRecursos;
    tJugador* jugador = (tJugador*)vJugador;

    printf("En este momento esta jugando:\n");
    mostrarJugador(jugador);
    system("pause");
    system("cls");

    if(OK == (*retornoCodigoDeError = generaRondas(recursos, jugador, retornoCodigoDeError)))
    {
        system("cls");
        printf("Secuencia final asignada:");
        mostrarListaSimpleEnOrden(&(jugador->secuenciaAsignada), mostrarCaracter);
        printf("Cantidad de puntos totales acumulados por el jugador: %d\n", jugador->puntosTotales);
    }
    system("pause");
    system("cls");
}

int iniciarJuego(tRecursos* recursos)
{
    int retornoCodigoDeError;

    recursos->datoRespuestaAPI.buffer = NULL;//para hacer free de NULL y no de basura en caso de errores
    recursos->cantidadDeIndicesDeCaracteresDeSecuenciaRestantes = 0;
    recursos->mayorPuntajeTotal = 0;

    mapEnListaSimple(&(recursos->listaDeJugadores), recursos, &retornoCodigoDeError, jugarRondas);

    if(OK != retornoCodigoDeError)
    {
        fprintf(stderr, "Juego suspendido.\n");
    }

    liberarRecursosParaConsumoDeAPI(recursos);
    return retornoCodigoDeError;
}
