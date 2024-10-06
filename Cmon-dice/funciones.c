#include "funciones.h"

void mostrarConfiguracionElegida(tConfiguracion* configuracion, unsigned indiceDeNivelDeConfiguracionElegida)
{
    printf("Configuracion seleccionada:\n");
    printf("\tNivel de dificultad: %s.\n",
           INDICE_NIVEL_FACIL == indiceDeNivelDeConfiguracionElegida ? "FACIL" : INDICE_NIVEL_MEDIO == indiceDeNivelDeConfiguracionElegida ? "MEDIO" : "DIFICIL"
          );
    printf("\tTiempo de visualizacion de secuencia: %u.\n", configuracion[indiceDeNivelDeConfiguracionElegida].tiempoDeVisualizacionSecuenciaCorrecta);
    printf("\tTiempo de respuesta: %u.\n", configuracion[indiceDeNivelDeConfiguracionElegida].tiempoRespuestaPorRonda);
    printf("\tCantidad de vidas: %u.\n", configuracion[indiceDeNivelDeConfiguracionElegida].cantidadDeVidas);
}

int cargarConfiguraciones(FILE* aConfiguracion, tConfiguracion* configuraciones)
{
    char buffer[TAM_BUFFER_CARGA_CONFIGURACIONES];
    unsigned indice;

    while(fgets(buffer, TAM_BUFFER_CARGA_CONFIGURACIONES, aConfiguracion))
    {
        if(INDICE_INVALIDO == (indice = defineIndiceDeNivelSegunCaracter(*buffer)))
        {
            fprintf(stderr, "Error de grabacion de archivo de configuraciones: LETRA INICIAL INCORRECTA.\n");
            return ARCHIVO_TXT_DE_CONFIGURACION_CON_ERRORES;
        }

        sscanf(buffer, "%*c|%u|%u|%u",
            &(configuraciones[indice].tiempoDeVisualizacionSecuenciaCorrecta),
            &(configuraciones[indice].tiempoRespuestaPorRonda),
            &(configuraciones[indice].cantidadDeVidas)
        );//%*c ignora la primer letra

        if(
            !(configuraciones[indice].tiempoDeVisualizacionSecuenciaCorrecta > MIN_TIEMPO_JUEGO_POR_RONDA && configuraciones[indice].tiempoDeVisualizacionSecuenciaCorrecta <= MAX_TIEMPO_JUEGO_POR_RONDA) ||
            !(configuraciones[indice].tiempoRespuestaPorRonda > MIN_TIEMPO_JUEGO_POR_RONDA && configuraciones[indice].tiempoRespuestaPorRonda <= MAX_TIEMPO_JUEGO_POR_RONDA) ||
            !(configuraciones[indice].cantidadDeVidas >= MIN_CANT_VIDAS && configuraciones[indice].cantidadDeVidas <= MAX_CANT_VIDAS)
        )
        {
            fprintf(stderr, "Error de grabacion de archivo de configuraciones: VALORES FUERA DE RANGO.\n");
            return ARCHIVO_TXT_DE_CONFIGURACION_CON_ERRORES;
        }
    }
    return OK;
}

void mostrarJugador(const void* dato)
{
    printf("[ID: %u]%s\n",
           ((tJugador*)dato)->id,
           ((tJugador*)dato)->nya
          );
}

int validoIngresoDeNombre(const char* cadena)
{
    if(!ES_LETRA(*cadena))
    {
        return NOMBRE_INVALIDO;
    }

    while('\0' != *cadena)
    {
        if((!ES_LETRA(*cadena) && !ES_BLANCO(*cadena)) ||
                ((ES_LETRA(*cadena) && '\0' != *(cadena + 1) && ES_BLANCO(*(cadena + 1)) && '\0' != *(cadena + 2) && !ES_LETRA(*(cadena + 2)))))
        {
            return NOMBRE_INVALIDO;
        }

        cadena++;
    }

    if(ES_BLANCO(*(cadena - 1)))
    {
        return NOMBRE_INVALIDO;
    }

    return OK;
}

int ingresoDeNombresAListaSimple(t_lista* listaDeJugadores, unsigned* cantidadDeJugadores)
{
    tJugador jugador;
    unsigned ingresoNombres = 1;
    char* retorno;

    jugador.id = 0;
    *cantidadDeJugadores = 0;

    printf("\t\tINGRESO DE NOMBRES DE LOS JUGADORES\n");
    printf(" _______________________________________________________________________\n");
    printf("|Reglas de formato de ingreso:                                          |\n");
    printf("|-Solo puede hacer ingreso de caracteres LETRA o ESPACIO para nombre.   |\n");
    printf("|-Solo puede utilizar un ESPACIO entre dos caracteres LETRA para nombre.|\n");
    printf("|-Al finalizar ingreso de nombre, presione ENTER.                       |\n");
    printf("|FINALIZAR INGRESO DE NOMBRES: ingrese 'X' y presione ENTER.            |\n");
    printf("|_______________________________________________________________________|\n\n");
    system("pause");
    system("cls");

    do
    {
        printf("[FINALIZAR INGRESO DE NOMBRE DE LOS JUGADORES: ingrese 'X' y presione ENTER]\n");
        printf("\nIngrese Nombre de Jugador con ID = %d:\t", jugador.id + 1);
        fflush(stdin);
        fgets(jugador.nya, TAM_NyA, stdin);

        if(NULL != (retorno = strrchr(jugador.nya, '\n')))
        {
            *retorno = '\0';
        }
        *(jugador.nya) = ES_LETRA(*(jugador.nya)) ? A_MAYUS(*(jugador.nya)) : *(jugador.nya);

        if(CARACTER_DE_SALIDA == *(jugador.nya) && '\0' == *(jugador.nya + 1))
        {
            ingresoNombres = 0;
        }
        else if(!validoIngresoDeNombre(jugador.nya))
        {
            printf("\nIngreso de nombre invalido, intente nuevamente.\n");
            system("pause");
        }
        else
        {
            (jugador.id)++;///SOLO SI ES UN NOMBRE VALIDO LE ASIGNO ID
            (*cantidadDeJugadores) = jugador.id;
            insertarAlFinalEnListaSimple(listaDeJugadores, &jugador, sizeof(tJugador));
        }
        system("cls");
    }
    while(ingresoNombres);

    if(!*cantidadDeJugadores)
    {
        return NO_HAY_JUGADORES;
    }

    return OK;
}

int defineIndiceDeNivelSegunCaracter(char caracter)
{
    if(!ES_LETRA(caracter))
    {
        return INDICE_INVALIDO;
    }

    caracter = A_MAYUS(caracter);
    if(FACIL == caracter)
    {
        return INDICE_NIVEL_FACIL;
    }
    else if(MEDIO == caracter)
    {
        return INDICE_NIVEL_MEDIO;
    }
    else if(DIFICIL == caracter)
    {
        return INDICE_NIVEL_DIFICIL;
    }

    return INDICE_INVALIDO;
}

void ingresoDeNivel(unsigned* indiceDeNivelDeConfiguracionElegida)//aca no soy tan estricto con lo que ingresa porque me interesa que elija uno
{
    int indiceNivelDeDificultad;
    char nivelDeDificultadIngresado;

    do
    {
        printf("Ingrese nivel de dificiltad[FACIL(F) - MEDIO(M) - DIFICIL(D)]:\t");
        fflush(stdin);
        scanf("%c", &nivelDeDificultadIngresado);
        nivelDeDificultadIngresado = A_MAYUS(nivelDeDificultadIngresado);

        if(INDICE_INVALIDO == (indiceNivelDeDificultad = defineIndiceDeNivelSegunCaracter(nivelDeDificultadIngresado)))
        {
            printf("\nIngreso invalido, intente nuevamente.\n");
            system("pause");
        }
        system("cls");
    }
    while(INDICE_INVALIDO == indiceNivelDeDificultad);
    printf("Nivel de dificultad ingresado: %c\n", nivelDeDificultadIngresado);

    *indiceDeNivelDeConfiguracionElegida = indiceNivelDeDificultad;
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

void construccionURL(char* URL, unsigned tam, unsigned ce)//doy la flexibilidad de usar cantidad de elementos o no
{
    snprintf(URL, tam, "%s%s%s%u", URL_BASE, QUERY_PARAMS_FORMATO, QUERY_PARAM_CANTIDAD_DE_ELEMENTOS, ce * CANT_RONDAS_PROMEDIO_JUGADAS);
}

int consumoAPI(tReconstruccionDato* datoRespuestaAPI, unsigned cantidadDeJugadores,void (*construccionURL)(char* URL, unsigned tam, unsigned ce))
{
    CURL* curl;
    char URL[TAM_URL];
    int retornoCodigoDeError;

    construccionURL(URL, sizeof(URL),cantidadDeJugadores);

    if(ERROR_INICIAR_ESTRUCTURA_CURL == inicializacionEstructuraCURL(&curl))
    {
        return ERROR_INICIAR_ESTRUCTURA_CURL;
    }

    configuracionEstructuraCURL(curl, URL, datoRespuestaAPI);
    retornoCodigoDeError = ejecutarSolicitudHTTPS(curl);
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

    fprintf(stdout, "%s\n",(char*)(datoRespuestaAPI->buffer));
    curl_easy_cleanup(curl);

    if(OK != (retornoCodigoDeError = validaFormatoRespuestaAPI(datoRespuestaAPI->buffer)))
    {
        fprintf(stderr, "La respuesta del servidor no llego en el formato esperado.\n");
        return retornoCodigoDeError;
    }

    return OK;
}

void imprimirResultados(FILE* pf, tRecursos* recursos)
{
    fprintf(pf, "\nIMPLEMENTAR FORMATO DE INFORME EN LA FUNCION imprimirResultado\n"
            );
}

void construccionNombreArchivoTxtInforme(char* NOMBRE_ARCHIVO_TXT_INFORME, unsigned tam, struct tm* fechaYHora)
{
        snprintf(NOMBRE_ARCHIVO_TXT_INFORME, tam,
            "informe-juego_%4d-%02d-%02d-%02d-%02d.txt",
            fechaYHora->tm_year + 1900,
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

char obtenerCaracterDeSecuencia(const char* cadenaConIndices, const char* caracteresDeSecuencia)
{
    return '\0' == *cadenaConIndices ? CARACTER_DE_SALIDA : caracteresDeSecuencia[A_NUMERO(*cadenaConIndices)];
}

int generarRondas(tRecursos* recursos)
{
    char caracteresDeSecuencia[] = {COLOR_VERDE, COLOR_AMARILLO, COLOR_ROJO, COLOR_NARANJA};
    int retornoCodigoDeError;

    (recursos->datoRespuestaAPI).cantBytesFijosAReservar = recursos->cantidadDeJugadores * CANT_RONDAS_PROMEDIO_JUGADAS * INCLUIR_BARRA_N + CONTAR_BARRA_CERO;
    (recursos->datoRespuestaAPI).buffer = malloc((recursos->datoRespuestaAPI).cantBytesFijosAReservar);
    if(NULL == (recursos->datoRespuestaAPI).buffer)
    {
        fprintf(stderr, "No pude reservar memoria para buffer de respuesta de API.\n");
        return NO_PUDE_RESERVAR_MEMORIA;
    }
    *(char*)((recursos->datoRespuestaAPI).buffer) = '\0';
    recursos->datoRespuestaAPI.cantBytesReservados = recursos->datoRespuestaAPI.cantBytesFijosAReservar;
    recursos->datoRespuestaAPI.cantBytesCopiados = 0;

    if(OK != (retornoCodigoDeError = consumoAPI(&(recursos->datoRespuestaAPI), recursos->cantidadDeJugadores, construccionURL)))
    {
        fprintf(stderr, "No pude generar rondas.\n");
        free((recursos->datoRespuestaAPI).buffer);
        return retornoCodigoDeError;
    }

    recursos->cadenaConIndices = (recursos->datoRespuestaAPI).buffer;
    while('\0' != *(recursos->cadenaConIndices))
    {
        printf("LETRA OBTENIDA: %c\n", obtenerCaracterDeSecuencia(recursos->cadenaConIndices, caracteresDeSecuencia));
        (recursos->cadenaConIndices) += 2;///porque 2?, 1 por el caracter que ya utilice y 1 por el \n
    }

    free((recursos->datoRespuestaAPI).buffer);
    return retornoCodigoDeError;
}

int iniciarJuego(tRecursos* recursos)
{
    int retornoCodigoDeError;

    if(OK != (retornoCodigoDeError = generarRondas(recursos)))
    {
        return retornoCodigoDeError;
    }

    return OK;
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
        printf("\nCaracteres validos para ingreso en secuencias: %s.\n", CARACTERES_VALIDOS_A_INGRESAR_PARA_SECUENCIA);
        printf("\tX: uso de vida.\n");
        printf("\tV: color verde.\n");
        printf("\tA: color amarillo.\n");
        printf("\tR: color rojo.\n");
        printf("\tN: color naranja.\n\n");
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

        ///TODO:
        ///CODEAR LOGICA DE RONDAS
        //CALCULAR PUNTOS POR JUGADOR POR RONDA
        //IR CALCULANDO EL TOTAL DE PUNTOS PARA NO VOLVER A RECORRER
        //COLA DE GANADORES?[cuanto termina de jugar el primero, lo acolo, luego, comparo con el primero de la cola]-> solo guardar ID, NOMBRE Y PUNTOS que consiguio para ganr
        ///CODEAR FORMATO DEL INFORME
//
//        if(OK != (retornoCodigoDeError = generarInforme(recursos, construccionNombreArchivoTxtInforme)))
//        {
//            fprintf(stderr, "No pude grabar archivo de informe.\n");
//            vaciarListaSimple(&(recursos->listaDeJugadores));
//            return retornoCodigoDeError;
//        }
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


