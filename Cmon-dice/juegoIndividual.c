#include "funciones.h"

void mostrarCaracter(const void* dato)
{
    printf("%c\n", *(char*)dato);
}

void mostrarSecuenciaAsignada(tRecursos* recursos, tJugador* jugador, unsigned tiempoParaVisualizarSecuencia)
{
    pthread_t id;

    inicializacionDeRecursos(recursos, tiempoParaVisualizarSecuencia);
    configuracionesGraficas(recursos);
    printf("Secuencia asignada para esta ronda:\n");

    pthread_create(&id, NULL, accionParaThreadDeTemporizador, recursos);

    mostrarListaSimpleEnOrden(&(jugador->secuenciaAsignada), mostrarCaracter);

    pthread_join(id, NULL);

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    system("cls");
    Sleep(100);
}

int comparaCaracteres(const void* a, const void* b)
{
    char caracterA = *(char*)a;
    char caracterB = *(char*)b;

    if(ES_LETRA(caracterA) && ES_LETRA(caracterB))
    {
        caracterA = toupper(caracterA);
        caracterB = toupper(caracterB);
    }
    return caracterA - caracterB;
}

int ingresaYValida(int min, int max, int cantidadDeVidasDelJugador, int cantidadDeCaracteresDeSecuenciaIngresados)
{
    int cantidadIngresada;

    printf("Tiene %d vida/s.\n", cantidadDeVidasDelJugador);
    printf("Ha ingresado %d caracter/es de secuencia.\n", cantidadDeCaracteresDeSecuenciaIngresados);
    do
    {
        printf("\nIngrese cantidad de posiciones a retroceder(1 o mas):\t");
        fflush(stdin);
        scanf("%d", &cantidadIngresada);

        if(!(cantidadIngresada >= min && cantidadIngresada <= max))
        {
            printf("Ingreso invalido, intente nuevamente.\n");
        }
    }
    while(!(cantidadIngresada >= min && cantidadIngresada <= max));

    return cantidadIngresada;
}

int determinarAccion(tRecursos* recursos, tJugador* jugador, tRonda* ronda, int* cantidadDeVidasDelJugador, int* cantidadDeCaracteresDeSecuenciaIngresados, unsigned cantidadDeCaracteresDeSecuencia, unsigned tiempoParaIngresarSecuencia, char ch)
{
    int min;
    int max;
    int cantidadIngresada;

    //CAMINO BONITO -> SECUENCIA CORRECTA
    if(SON_IGUALES == verificarIgualdadEnCantidadDeElementosYContenidoEnListaSimple(&(jugador->secuenciaAsignada), &(ronda->secuenciaIngresada), comparaCaracteres))
    {
        printf("\nCorrecto!\n\n");
        if(ronda->vidasUsadas > 0)
        {
            ronda->puntosObtenidos = 1;
        }
        else
        {
            ronda->puntosObtenidos = 3;
        }
        return FIN_DE_RONDA_ACTUAL;
    }

    // EL JUGADOR YA NO TIENE VIDAS -> PERDIÓ.
    if(!*cantidadDeVidasDelJugador)
    {
        system("cls");
        printf("No tiene mas vidas.\n\n");
        (*cantidadDeVidasDelJugador)--;
        return FIN_DE_RONDA_ACTUAL;
    }

    recursos->temporizador.tiempoRestanteParaTemporizador = tiempoParaIngresarSecuencia;

    //caso 1 - 3 -> TIME OUT CON VIDA o USO DE VIDA CON VIDA
    if((recursos->temporizador.timeout && !*cantidadDeCaracteresDeSecuenciaIngresados)  || ('X' == ch && !*cantidadDeCaracteresDeSecuenciaIngresados))
    {
        if(recursos->temporizador.timeout)
        {
            printf("\nSe ha quedado sin tiempo.");
        }
        printf("\nSe le ha restado una vida.\n\n");
        (*cantidadDeVidasDelJugador)--;
        (ronda->vidasUsadas)++;
        return REINICIAR_NIVEL;
    }

    //caso 2-> INGRESÉ UNA SECUENCIA PERO: APRETÉ X O ESTÁ INCORRECTA --> debo elegir cuantas posiciones retroceder
    if('X' == ch)
    {
        printf("\nUso de vida.\n");
    }
    else
    {
        printf("\nHa ingresado una secuencia pero cometiendo uno o varios errores.\n");
    }
    min = 1;
    max = MENOR(*cantidadDeVidasDelJugador, *cantidadDeCaracteresDeSecuenciaIngresados + 1);
    cantidadIngresada = ingresaYValida(min, max, *cantidadDeVidasDelJugador, *cantidadDeCaracteresDeSecuenciaIngresados);
    printf("\nSe le han restado vidas.\n");
    (*cantidadDeVidasDelJugador) -= cantidadIngresada;
    ronda->vidasUsadas += cantidadIngresada;

    while(cantidadIngresada && sacarUltimoEnListaSimple(&ronda->secuenciaIngresada, &ch, sizeof(char)))
    {
        cantidadIngresada--;
        (*cantidadDeCaracteresDeSecuenciaIngresados)--;
    }

    // Si retrocede cantidadDeCaracteresIngresados +1 -> vuelvo a mostrar la secuencia asignada
    if(cantidadIngresada)
    {
        return REINICIAR_NIVEL;
    }
    return INGRESO_SIN_MOSTRAR;
}

int jugarRonda(tRecursos* recursos, tJugador* jugador, tRonda* ronda, int* cantidadDeVidasDelJugador, unsigned cantidadDeCaracteresDeSecuencia, unsigned tiempoParaVisualizarSecuencia, unsigned tiempoParaIngresarSecuencia)
{
    pthread_t id;
    char ch;
    int cantidadDeCaracteresDeSecuenciaIngresados = 0;
    int accionActual = REINICIAR_NIVEL;

    inicializacionDeRecursos(recursos, tiempoParaIngresarSecuencia);
    configuracionesGraficas(recursos);

    while(FIN_DE_RONDA_ACTUAL != accionActual)
    {
        if(REINICIAR_NIVEL == accionActual)
        {
            mostrarSecuenciaAsignada(recursos, jugador, tiempoParaVisualizarSecuencia);
            printf("Ingresa un caracter: %s.\n", CARACTERES_VALIDOS_A_INGRESAR_PARA_SECUENCIA);
        }

        pthread_create(&id, NULL, accionParaThreadDeTemporizador, recursos);
        recursos->temporizador.timeout = 0;
        recursos->temporizador.detenerTemporizador = 0;
        recursos->temporizador.tiempoRestanteParaTemporizador = tiempoParaIngresarSecuencia;
        while(!recursos->temporizador.timeout && cantidadDeCaracteresDeSecuenciaIngresados < cantidadDeCaracteresDeSecuencia)
        {

            if(_kbhit())//Verifica si presione una tecla
            {
                ch = _getch();//Lee un caracter SIN ESPERAR ENTER

                if(ES_LETRA(ch))
                {
                    ch = toupper(ch);
                }
                if('V' == ch || 'A' == ch || 'R' == ch || 'N' == ch)
                {
                    //Posición para mostrar secuencia ingresada
                    recursos->temporizador.coordenadas.posicionDeTextoLetraIngresada.X = recursos->temporizador.coordenadas.posicionDeOrigen.X;
                    recursos->temporizador.coordenadas.posicionDeTextoLetraIngresada.Y = recursos->temporizador.coordenadas.posicionDeOrigen.Y + cantidadDeCaracteresDeSecuenciaIngresados + 2;

                    SetConsoleCursorPosition(recursos->temporizador.hConsole, recursos->temporizador.coordenadas.posicionDeTextoLetraIngresada); // Mueve el cursor para mostrar el caracter ingresado
                    printf("%c", ch); //Mostrar el caracter ingresado

                    insertarAlFinalEnListaSimple(&(ronda->secuenciaIngresada), &ch, sizeof(char));
                    cantidadDeCaracteresDeSecuenciaIngresados++; //Incrementar el contador de intentos
                }

                if('X' == ch)
                {
                    break;
                }
            }
        }
        recursos->temporizador.detenerTemporizador = 1;
        pthread_join(id, NULL);
        if(recursos->temporizador.timeout)
        {
            recursos->temporizador.coordenadas.posicionDeTextoFinal.X = recursos->temporizador.coordenadas.posicionDeOrigen.X;
            recursos->temporizador.coordenadas.posicionDeTextoFinal.Y = recursos->temporizador.coordenadas.posicionDeOrigen.Y + cantidadDeCaracteresDeSecuenciaIngresados + 2;
            SetConsoleCursorPosition(recursos->temporizador.hConsole, recursos->temporizador.coordenadas.posicionDeTextoFinal);
        }

        accionActual = determinarAccion(recursos, jugador, ronda, cantidadDeVidasDelJugador, &cantidadDeCaracteresDeSecuenciaIngresados, cantidadDeCaracteresDeSecuencia, tiempoParaIngresarSecuencia, ch);

        system("pause");
        system("cls");

        if(INGRESO_SIN_MOSTRAR == accionActual)
        {
            printf("Ingresa un caracter: %s.\n", CARACTERES_VALIDOS_A_INGRESAR_PARA_SECUENCIA);
            mostrarListaSimpleEnOrden(&(ronda->secuenciaIngresada), mostrarCaracter);
        }
    }

    return FIN_DE_RONDA_ACTUAL;
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

int generaRondas(tRecursos* recursos, tJugador* jugador, int* retornoCodigoDeError)///generarRondas se ejecuta 1 vez por ronda para cada jugador.
{
    tRonda ronda;///puntosObtenidos - vidasUsadas
    int cantidadDeVidasDelJugador = recursos->configuraciones[recursos->indiceDeNivelDeConfiguracionElegida].cantidadDeVidas;
    unsigned tiempoParaVisualizarSecuencia = recursos->configuraciones[recursos->indiceDeNivelDeConfiguracionElegida].tiempoDeVisualizacionSecuenciaCorrecta;
    unsigned tiempoParaIngresarSecuencia = recursos->configuraciones[recursos->indiceDeNivelDeConfiguracionElegida].tiempoRespuestaPorRonda;
    char letra;

    *retornoCodigoDeError = OK;
    ronda.cantidadDeCaracteresDeSecuencia = 0;
    while(cantidadDeVidasDelJugador >= 0)
    {
        ///inicializo el tRonda
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

        jugarRonda(recursos, jugador, &ronda, &cantidadDeVidasDelJugador, ronda.cantidadDeCaracteresDeSecuencia, tiempoParaVisualizarSecuencia, tiempoParaIngresarSecuencia);

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
void jugarRondas(void* vRecursos, void* vJugador, int* retornoCodigoDeError)
{
    tRecursos* recursos = (tRecursos*)vRecursos;
    tJugador* jugador = (tJugador*)vJugador;

    printf("En este momento esta jugando:\n");
    mostrarJugador(jugador);
    printf("\n");
    system("pause");
    system("cls");

    if(OK == (*retornoCodigoDeError = generaRondas(recursos, jugador, retornoCodigoDeError)))
    {
        system("cls");
        printf("Secuencia final asignada:");
        mostrarListaSimpleEnOrden(&(jugador->secuenciaAsignada), mostrarCaracter);
        printf("Cantidad de puntos totales acumulados por el jugador: %d\n\n", jugador->puntosTotales);
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
    printf("Final de juego para el jugador.\n\n");
    system("pause");
    system("cls");

    liberarRecursosParaConsumoDeAPI(recursos);
    return retornoCodigoDeError;
}
