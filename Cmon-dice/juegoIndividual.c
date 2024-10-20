#include "funciones.h"

void mostrarCaracter(const void* dato)
{
    printf("%c\n", *(char*)dato);
}

///ACA ESTOY JUGANDO UNA RONDA LITERALMENTE y DEBO ALMACENAR LA INFO PERTINENTE
int ingresoDeSecuenciaManejandoPuntosYVidas(tRecursos* recursos, tJugador* jugador, int cantidadDeVidasSegunConfiguracion, unsigned maximaCantidadDeCaracteresDeSecuencia, unsigned maximoTiempoParaIngresoDeRespuesta)
{
    int retorno;
    ///ACTUALIZAR recursos->ronda.puntosObtenidos[ingresoDeSecuencia ACÁ!!!]
    ///ACTUALIZAR recursos->ronda.vidasUsadas[ingresoDeSecuencia ACÁ!!!]

    retorno = ingresoDeSecuencia(recursos, jugador, cantidadDeVidasSegunConfiguracion, maximaCantidadDeCaracteresDeSecuencia, maximoTiempoParaIngresoDeRespuesta);
    (recursos->cantidadDeVidasUsadasTotales) += (recursos->ronda.vidasUsadas);

    if(DEBO_REPETIR_RONDA == retorno)
    {
        return DEBO_REPETIR_RONDA;
    }

    jugador->puntosTotales += recursos->ronda.puntosObtenidos;///ACTUALIZO TOTAL
    return FIN_DE_RONDA;
}

/// generarRondas es por CADA RONDA del jugador.
int generaRondas(tRecursos* recursos, tJugador* jugador, int cantidadDeVidasSegunConfiguracion, int* retornoCodigoDeError)
{
    char letra;
    unsigned cantidadDeCaracteresDeSecuencia = 1;
    unsigned tiempoParaVisualizarSecuencia = recursos->configuraciones[recursos->indiceDeNivelDeConfiguracionElegida].tiempoDeVisualizacionSecuenciaCorrecta;
    unsigned tiempoParaIngresarSecuencia = recursos->configuraciones[recursos->indiceDeNivelDeConfiguracionElegida].tiempoRespuestaPorRonda;
    int jugarRonda;

    *retornoCodigoDeError = OK;
    while(cantidadDeVidasSegunConfiguracion - recursos->cantidadDeVidasUsadasTotales >= 0) //mientras tenga vidas.
    {
        recursos->ronda.puntosObtenidos = 0;
        recursos->ronda.vidasUsadas = 0;
        crearListaSimple(&(recursos->ronda.secuenciaIngresada));///CREO UNA LISTA DE SECUENCIA INGRESADA POR RONDA -> LA VACIO en funcion que libera lista de Rondas por jugador

        if(OK != (*retornoCodigoDeError = pedirLetraAleatoria(recursos, &letra))) //si no recibo una letra válida
        {
            fprintf(stderr, "No pude obtener letra aleatoria para formar secuencia.\n");
            return *retornoCodigoDeError;
        }
        //si recibo una letra válida: inserto en la secuencia asignada
        insertarAlFinalEnListaSimple(&(jugador->secuenciaAsignada), &letra, sizeof(char));

        jugarRonda = DEBO_REPETIR_RONDA;
        while(FIN_DE_RONDA != jugarRonda)
        {
            mostrarSecuenciaAsignada(recursos, jugador, tiempoParaVisualizarSecuencia);
            jugarRonda = ingresoDeSecuenciaManejandoPuntosYVidas(recursos, jugador, cantidadDeVidasSegunConfiguracion, cantidadDeCaracteresDeSecuencia, tiempoParaIngresarSecuencia);
            printf("\n");
            system("pause");
            system("cls");
        }

        cantidadDeCaracteresDeSecuencia++;
        tiempoParaVisualizarSecuencia++;//POR CADA RONDA, LE SUMO 1 SEGUNDO EXTRA
        tiempoParaIngresarSecuencia++; //POR CADA RONDA, LE SUMO 1 SEGUNDO EXTRA

        insertarAlFinalEnListaSimple(&(jugador->rondasJugadas), &(recursos->ronda), sizeof(tRonda));
    }
    (recursos->cantidadDeJugadores) --;///AYUDO A CONSUMO API PARA SER MAS EFICIENTE.

    return *retornoCodigoDeError;
}

/// jugarRondas es UNA VEZ por jugador.
void jugarRondas(void* vRecursos, void* vJugador, int* retornoCodigoDeError) //ex funcion inicializarJugador[esta funcion es la que se ejecuta en el map]
{
    tRecursos* recursos = (tRecursos*)vRecursos;
    tJugador* jugador = (tJugador*)vJugador;
    recursos->cantidadDeVidasUsadasTotales = 0;
    jugador->puntosTotales = 0;

    printf("En este momento esta jugando:\n");
    mostrarJugador(jugador);
    system("pause");
    system("cls");

    if(OK == (*retornoCodigoDeError = generaRondas(recursos, jugador,
                                      (recursos->configuraciones)[recursos->indiceDeNivelDeConfiguracionElegida].cantidadDeVidas,
                                      retornoCodigoDeError)))
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

    mapEnListaSimple(&(recursos->listaDeJugadores), recursos, &retornoCodigoDeError, jugarRondas);

    if(OK != retornoCodigoDeError)
    {
        fprintf(stderr, "Juego suspendido.\n");
    }

    liberarRecursosParaConsumoDeAPI(recursos);
    return retornoCodigoDeError;
}
