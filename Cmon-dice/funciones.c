#include "funciones.h"

int convertirIndiceEnCaracterDeSecuencia(char caracterIndice, char* letra)
{
    char caracteresDeSecuencia[] = {COLOR_VERDE, COLOR_AMARILLO, COLOR_ROJO, COLOR_NARANJA}; // V, A, R, N

    if(!('\0' != caracterIndice && ES_RANGO_INDICE_VALIDO(caracterIndice)))
    {
        fprintf(stderr, "No pude asignar caracter de secuencia, caracterIndice no estaba dentro del rango de %c - %c o era fin de cadena.\n", RANGO_MIN_DE_INDICE_VALIDO, RANGO_MAX_DE_INDICE_VALIDO);
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
        fprintf(stderr, "No pude obtenerCaracter de secuenciaAleatorio.\n");
        return retornoCodigoDeError;
    }

    recursos->cantidadDeIndicesDeCaracteresDeSecuenciaRestantes -= 1;///use uno de los caracteres que traje de la API
    (recursos->cadenaDeIndicesTraidosDeAPI) += 2;///proximo caracter valido a convertir

    return retornoCodigoDeError;
}

int pedirLetraAleatoria(tRecursos* recursos, char* letra)
{
    int retornoCodigoDeError; //puede devolver un error o puede devolver que está todo ok.

    if(!recursos->cantidadDeIndicesDeCaracteresDeSecuenciaRestantes)    ///AB: si nunca le pegue a la API o si me quede sin caracteres que traje, la consumo.
    {
        printf("\n*************************************\n");//AB: HARDCODEADO PARA TESTEAR
        printf("CONSUMI API");
        printf("\n*************************************\n");
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

    if(recursos->cantidadDeIndicesDeCaracteresDeSecuenciaRestantes) //AB: HARDCODEADO PARA TESTEAR
        printf("LETRA OBTENIDA: %c\n", *letra);

    if(!recursos->cantidadDeIndicesDeCaracteresDeSecuenciaRestantes)    ///AB: SI me quede sin caracteres, hago free. NUNCA voy a poder entrar si al menos no consumi 1 vez la api
    {
        printf("ULTIMA LETRA OBTENIDA: %c\n", *letra);  //AB: HARDCODEADO PARA TESTEAR
        printf("\n*************************************\n");
        printf("HICE FREE PORQUE ME CONSUMI TODO EL LOTE TRAIDO DE API\n");
        printf("\n*************************************\n");

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

int jugar(tRecursos* recursos)
{
    int retornoCodigoDeError;

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


        printf("\n**************************ARRANCA EL JUEGO**************************\n");
        if(OK != (retornoCodigoDeError = iniciarJuego(recursos)))
        {
            fprintf(stderr, "No pude iniciar el juego.\n");
            vaciarListaSimple(&(recursos->listaDeJugadores));
            return retornoCodigoDeError;
        }

        ///PUNTOS

        if(OK != (retornoCodigoDeError = generarInforme(recursos, construccionNombreArchivoTxtInforme)))
        {
            fprintf(stderr, "No pude grabar archivo de informe.\n");
            vaciarListaSimple(&(recursos->listaDeJugadores));
            return retornoCodigoDeError;
        }
    }
    //RESETEO TODOS LOS PARAMETROS PARA VOLVER A JUGAR
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


