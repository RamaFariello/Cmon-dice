#include "funciones.h"

int inicializarRecursosParaConsumoDeAPI(tRecursos* recursos)
{
    /*
        CALCULO CUÁNTO ESPACIO RESERVAR PARA RESPUESTA DE LA API
        para hacer la solicitud a la api, se toma en cuenta la cantidad de jugadores y la cantidad de rondas (iteración por caracter) que van a jugar.
        se incluye en espacio reservado el /n para cada caracter y el /0 una única vez.
        Una vez calculada la cantidad de bytes, se realiza el malloc.
    */
    (recursos->datoRespuestaAPI).cantBytesFijosAReservar = recursos->cantidadDeJugadores * CANT_RONDAS_PROMEDIO_JUGADAS * INCLUIR_BARRA_N + CONTAR_BARRA_CERO;
    (recursos->datoRespuestaAPI).buffer = malloc((recursos->datoRespuestaAPI).cantBytesFijosAReservar);
    if(NULL == (recursos->datoRespuestaAPI).buffer)
    {
        fprintf(stderr, "No pude reservar memoria para buffer de respuesta de API.\n");
        return NO_PUDE_RESERVAR_MEMORIA;
    }
    *(char*)((recursos->datoRespuestaAPI).buffer) = '\0'; // por seguridad, en caso de que la API no agregue el /0
    recursos->datoRespuestaAPI.cantBytesReservados = recursos->datoRespuestaAPI.cantBytesFijosAReservar;
    recursos->datoRespuestaAPI.cantBytesCopiados = 0;

    return OK;
}

void liberarRecursosParaConsumoDeAPI(tRecursos* recursos)
{
    free((recursos->datoRespuestaAPI).buffer);
    (recursos->datoRespuestaAPI).buffer = NULL;
}

int validaFormatoRespuestaAPI(const char* respuesta)
{
    if(0 != strlen(respuesta) % 2)//SIEMPRE ES 1 CARACTER y 1 \n => la cantidad recibida es PAR(NO INCLUYO EL \0 en la validacion, se asume que esta-> lo pone mi funcion de write callback)
    {
        fprintf(stderr, "Respuesta de API en formato erroneo.\n");
        return RESPUESTA_SERVIDOR_CON_ERROR_DE_FORMATO;
    }
    return OK;
}

void construccionURL(char* URL, unsigned tam, unsigned ce)
{
    /**Armo la URL que le mando a la API*/
    /* A la URL base se le especifica:
    -params formato:indice max e indice minimo, base (10), format:plain para que devuelva texto.
    -param cantidad de elementos: "&num=", se va a completar con la cantidad de elementos calculada en el parámetro siguiente.
    -cantidad de elementos: cuántos números le vamos a pedir a la api. Si hay un solo jugador y nuestro promedio es de 15 rondes, pedirá 15 números.
    */
    snprintf(URL, tam, "%s%s%s%u", URL_BASE, QUERY_PARAMS_FORMATO, QUERY_PARAM_CANTIDAD_DE_ELEMENTOS, ce * CANT_RONDAS_PROMEDIO_JUGADAS);
}

int consumoAPI(tReconstruccionDato* datoRespuestaAPI, unsigned cantidadDeJugadores, void (*construccionURL)(char* URL, unsigned tam, unsigned ce))
{
    /** Esta es una función genérica para poder consumir cualquier tipo de API, según el armado de URL. */
    CURL* curl;
    char URL[TAM_URL];
    int retornoCodigoDeError;

    construccionURL(URL, sizeof(URL), cantidadDeJugadores);

    //se reservan los recursos del curl
    if(ERROR_INICIAR_ESTRUCTURA_CURL == inicializacionEstructuraCURL(&curl))
    {
        return ERROR_INICIAR_ESTRUCTURA_CURL;
    }

    configuracionEstructuraCURL(curl, URL, datoRespuestaAPI); // se configura el curl para hacer la solicitud a la API
    retornoCodigoDeError = ejecutarSolicitudHTTPS(curl); /// llamado a la API
    if(!(retornoCodigoDeError >= RESPUESTA_SERVIDOR_OK_MIN && retornoCodigoDeError <= RESPUESTA_SERVIDOR_OK_MAX))
    {
        fprintf(stderr, "Codigo de error: %d. %s.\n", retornoCodigoDeError,
                retornoCodigoDeError >= RESPUESTA_SERVIDOR_ERROR_CLIENTE_MIN && retornoCodigoDeError <= RESPUESTA_SERVIDOR_ERROR_CLIENTE_MAX ? "Error del Cliente" :
                retornoCodigoDeError >= RESPUESTA_SERVIDOR_ERROR_SERVIDOR_MIN && retornoCodigoDeError <= RESPUESTA_SERVIDOR_ERROR_SERVIDOR_MAX ? "Error del Servidor" :
                retornoCodigoDeError >= RESPUESTA_SERVIDOR_INFORMATIVA_MIN && retornoCodigoDeError <= RESPUESTA_SERVIDOR_INFORMATIVA_MAX ? "Mensaje informativo del Servidor" :
                retornoCodigoDeError >= RESPUESTA_SERVIDOR_REDIRECCION_MIN && retornoCodigoDeError <= RESPUESTA_SERVIDOR_REDIRECCION_MAX ? "Redireccion" :
                "Ocurrio un error en el codigo de CURL"
               );
        curl_easy_cleanup(curl);
        return retornoCodigoDeError;
    }

///    fprintf(stdout, "%s\n",(char*)(datoRespuestaAPI->buffer));
    curl_easy_cleanup(curl);

    if(OK != (retornoCodigoDeError = validaFormatoRespuestaAPI(datoRespuestaAPI->buffer))) // se valida el formato que recibimos de la API
    {
        fprintf(stderr, "La respuesta del servidor no llego en el formato esperado.\n");
        return retornoCodigoDeError;
    }

    return OK;
}

