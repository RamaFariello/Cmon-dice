#include "funciones.h"

void mostrarJugador(const void* dato)
{
    printf("[ID: %u]%s\n",
           ((tJugador*)dato)->id,
           ((tJugador*)dato)->nya
          );
}

int convertirIndiceEnCaracterDeSecuencia(char caracterIndice, char* letra)
{
    char caracteresDeSecuencia[] = {COLOR_VERDE, COLOR_AMARILLO, COLOR_ROJO, COLOR_NARANJA}; // V, A, R, N

    if(!ES_RANGO_INDICE_VALIDO(caracterIndice))
    {
        fprintf(stderr, "No pude asignar caracter de secuencia, caracterIndice no estaba dentro del rango de %c a %c.\n", RANGO_MIN_DE_INDICE_VALIDO, RANGO_MAX_DE_INDICE_VALIDO);
        *letra = caracterIndice;
        return NO_PUDE_ASIGNAR_CARACTER_DE_SECUENCIA;
    }

    *letra = caracteresDeSecuencia[A_NUMERO(caracterIndice)];
    return OK;
}

int obtenerCaracterDeSecuenciaAleatorio(tRecursos* recursos, char* letra)
{
    int retornoCodigoDeError;

    if(OK != (retornoCodigoDeError = convertirIndiceEnCaracterDeSecuencia(*(recursos->cadenaDeIndicesTraidosDeAPI), letra)))
    {
        fprintf(stderr, "No pude obtener caracter de secuencia aleatorio.\n");
        return retornoCodigoDeError;
    }

    recursos->cantidadDeIndicesDeCaracteresDeSecuenciaRestantes -= 1;///use uno de los caracteres que traje de la API
    (recursos->cadenaDeIndicesTraidosDeAPI) += 2;///proximo caracter valido a convertir

    return retornoCodigoDeError;
}

int pedirLetraAleatoria(tRecursos* recursos, char* letra)
{
    int retornoCodigoDeError;

    if(!recursos->cantidadDeIndicesDeCaracteresDeSecuenciaRestantes)
    {
        if(OK != (retornoCodigoDeError = inicializarRecursosParaConsumoDeAPI(recursos)))
        {
            fprintf(stderr, "No pude inicializar los recursos para el consumo de la API.\n");
            liberarRecursosParaConsumoDeAPI(recursos);
            return retornoCodigoDeError;
        }

        if(OK != (retornoCodigoDeError = consumoAPI(&(recursos->datoRespuestaAPI), recursos->cantidadDeJugadores, construccionURL)))
        {
            fprintf(stderr, "No pude consumir API.\n");
            liberarRecursosParaConsumoDeAPI(recursos);
            return retornoCodigoDeError;
        }
        recursos->cadenaDeIndicesTraidosDeAPI = recursos->datoRespuestaAPI.buffer;
        recursos->cantidadDeIndicesDeCaracteresDeSecuenciaRestantes = recursos->cantidadDeJugadores * CANT_RONDAS_PROMEDIO_JUGADAS;
    }

    if(OK != (retornoCodigoDeError = obtenerCaracterDeSecuenciaAleatorio(recursos, letra)))
    {
        fprintf(stderr, "No pude asignar una letra aleatoria por un error inesperado en conversion de indice traido por API a letra.\n");
        liberarRecursosParaConsumoDeAPI(recursos);
        return retornoCodigoDeError;
    }

    if(!recursos->cantidadDeIndicesDeCaracteresDeSecuenciaRestantes)
    {
        liberarRecursosParaConsumoDeAPI(recursos);
    }
    return retornoCodigoDeError;
}

void mostrarCaracteresValidos()
{
    printf("\nCaracteres validos para ingreso en secuencias: %s.\n", CARACTERES_VALIDOS_A_INGRESAR_PARA_SECUENCIA);
    printf("\tX: uso de vida.\n");
    printf("\tV: color verde.\n");
    printf("\tA: color amarillo.\n");
    printf("\tR: color rojo.\n");
    printf("\tN: color naranja.\n\n");
}

void liberarListasDeCadaJugador(void* vRecursos, void* vJugador, int* retornoCodigoDeError)///¡¡¡IMPORTANTE:HACER FREE de&jugador.rondasJugadas!!!
{
    tJugador* jugador = (tJugador*)vJugador;
     vaciarListaSimple(&(jugador->rondasJugadas));
     vaciarListaSimple(&(jugador->secuenciaAsignada));
}

int jugar(tRecursos* recursos)
{
    int retornoCodigoDeError;

    crearListaSimple(&(recursos->listaDeJugadores));

    if(!ingresoDeNombresAListaSimple(&(recursos->listaDeJugadores), &(recursos->cantidadDeJugadores)))
    {
        printf("No se ingresaron jugadores.\n");
    }
    else
    {
        ingresoDeNivel(&(recursos->indiceDeNivelDeConfiguracionElegida));
        mezclarListaSimpleAleatoriamente(&(recursos->listaDeJugadores), recursos->cantidadDeJugadores);

        system("cls");

        mostrarConfiguracionElegida(recursos->configuraciones, recursos->indiceDeNivelDeConfiguracionElegida);
        mostrarCaracteresValidos();
        system("pause");
        system("cls");

        printf("Lista de jugadores en el orden que deben jugar:\n");
        mostrarListaSimpleEnOrden(&(recursos->listaDeJugadores), mostrarJugador);
        system("pause");
        system("cls");

        if(OK != (retornoCodigoDeError = iniciarJuego(recursos)))   ///ACA EMPIEZA EL JUEGO
        {
            fprintf(stderr, "No se pudo jugar, intente nuevamente.\n");
            mapEnListaSimple(&(recursos->listaDeJugadores), recursos, NULL, liberarListasDeCadaJugador);
            vaciarListaSimple(&(recursos->listaDeJugadores));
            return retornoCodigoDeError;
        }

        if(OK != (retornoCodigoDeError = generarInforme(recursos, construccionNombreArchivoTxtInforme)))
        {
            fprintf(stderr, "No pude grabar archivo de informe.\n");
            mapEnListaSimple(&(recursos->listaDeJugadores), recursos, NULL, liberarListasDeCadaJugador);
            vaciarListaSimple(&(recursos->listaDeJugadores));
            return retornoCodigoDeError;
        }
    }
    //RESETEO TODOS LOS PARAMETROS PARA VOLVER A JUGAR
    mapEnListaSimple(&(recursos->listaDeJugadores), recursos, NULL, liberarListasDeCadaJugador);
    vaciarListaSimple(&(recursos->listaDeJugadores));

    return OK;
}

void switchTextoMenu(int opcion, void* recursos)
{
    switch(opcion)
    {
        case JUGAR:
            jugar(recursos);
        break;
    }
}


