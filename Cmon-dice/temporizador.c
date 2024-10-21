#include "funciones.h"

void ocultarCursor(tRecursos* recursos)
{
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(recursos->temporizador.hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE; //Oculta el cursor
    SetConsoleCursorInfo(recursos->temporizador.hConsole, &cursorInfo); //Cambia el cursor usando la referencia
}

// Función para limpiar la consola
void limpiarConsola(tRecursos* recursos)
{
    DWORD written;
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD length;

    GetConsoleScreenBufferInfo(recursos->temporizador.hConsole, &screen);
    length = screen.dwSize.X * screen.dwSize.Y;
    FillConsoleOutputCharacter(recursos->temporizador.hConsole, ' ', length, recursos->temporizador.coordenadas.posicionDeOrigen, &written);
    SetConsoleCursorPosition(recursos->temporizador.hConsole, recursos->temporizador.coordenadas.posicionDeOrigen);
}

void deshabilitarQuickEditMode()
{
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD prev_mode;

    // Obtiene el modo actual de la consola
    GetConsoleMode(hInput, &prev_mode);

    //Deshabilita el modo de edición rápida(Quick Edit) que permite la selección con el mouse[Mantiene la entrada por teclado]
    SetConsoleMode(hInput, prev_mode & ~ENABLE_QUICK_EDIT_MODE);///~ es para hacer el complemento bit a bit
}

void* accionParaThreadDeTemporizador(void* arg)
{
    tRecursos* recursos = (tRecursos*)arg;
    LARGE_INTEGER frequency, start, end;
    double tiempoTranscurrido = 0.0;

    Sleep(10);///EVITO QUE HAYA CONDICION DE CARRERA[para no tener que implementar un semaforo]
    QueryPerformanceFrequency(&frequency); // Obtiene la frecuencia del contador
    QueryPerformanceCounter(&start);       // Inicio del conteo

    SetConsoleCursorPosition(recursos->temporizador.hConsole, recursos->temporizador.coordenadas.posicionDelTemporizadorEnConsola);
    printf("%d seg   ", recursos->temporizador.tiempoRestanteParaTemporizador); // Muestra el temporizador
    fflush(stdout);

    //Bucle del temporizador
    while(recursos->temporizador.tiempoRestanteParaTemporizador > 0 && !recursos->temporizador.detenerTemporizador)
    {
        QueryPerformanceCounter(&end);
        tiempoTranscurrido = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;

        if(tiempoTranscurrido >= 1.0) // Cada segundo
        {
            recursos->temporizador.tiempoRestanteParaTemporizador--;
            start = end; // Reiniciar el contador para el próximo segundo

            if (recursos->temporizador.detenerTemporizador)
                break;

            // Mueve el cursor a la posición del cronómetro
            SetConsoleCursorPosition(recursos->temporizador.hConsole, recursos->temporizador.coordenadas.posicionDelTemporizadorEnConsola);
            printf("%d seg   ", recursos->temporizador.tiempoRestanteParaTemporizador);
            fflush(stdout);
        }

        Sleep(10); //Breve pausa para evitar uso intensivo de CPU
    }

    if(!recursos->temporizador.tiempoRestanteParaTemporizador)//Si no queda tiempo restante
    {
        recursos->temporizador.timeout = 1;///SE TERMINO EL TIEMPO
    }

    return NULL;
}

void configuracionesGraficas(tRecursos* recursos)
{
    //Deshabilitar el modo de edición rápida(Quick Edit)
    deshabilitarQuickEditMode();
    //Ocultar el cursor del temporizador
    ocultarCursor(recursos);
    //Limpiar consola al iniciar
    limpiarConsola(recursos);
}

void inicializacionDeRecursos(tRecursos* recursos, unsigned tiempoMaximo)
{
    recursos->temporizador.detenerTemporizador = 0;  //Mantener el temporizador corriendo para toda la secuencia

    recursos->temporizador.coordenadas.posicionDelTemporizadorEnConsola.X = 90; //Posición del temporizador X
    recursos->temporizador.coordenadas.posicionDelTemporizadorEnConsola.Y = 0; //Posición del temporizador Y

    recursos->temporizador.coordenadas.posicionDeOrigen.X = 0;
    recursos->temporizador.coordenadas.posicionDeOrigen.Y = 0;

    recursos->temporizador.timeout = 0;
    recursos->temporizador.tiempoRestanteParaTemporizador = tiempoMaximo;
    recursos->temporizador.hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
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

int ingresaYValida(int min, int max)
{
    int cantidadIngresada;

    do
    {
        printf("Ingrese valor:\t");
        scanf("%d", &cantidadIngresada);

        if(!(cantidadIngresada >= min && cantidadIngresada <= max))
        {
            printf("Ingreso invalido, intente nuevamente.\n");
        }
    }while(!(cantidadIngresada >= min && cantidadIngresada <= max));

    return cantidadIngresada;
}

///NO TERMINE DE ESCRIBIR LA SECUENCIA
int usarVida(tRecursos* recursos, tJugador* jugador, tRonda* ronda, int* cantidadDeVidasDelJugador, int* cantidadDeCaracteresDeSecuenciaIngresados, unsigned cantidadDeCaracteresDeSecuencia, unsigned tiempoParaIngresarSecuencia)
{
    int min;///REVISAR!!!
    int max;
    int cantidadIngresada;
    char aux;

    recursos->temporizador.detenerTemporizador = 1;

    if(!*cantidadDeVidasDelJugador)
    {
        printf("\nNo tiene mas vidas.\nFinalizo el juego, ");
        (*cantidadDeVidasDelJugador)--;
        return FIN_DE_JUEGO;
    }
    recursos->temporizador.tiempoRestanteParaTemporizador = tiempoParaIngresarSecuencia;
    printf("\nTiene %d cantidad de vida/s.\n", *cantidadDeVidasDelJugador);

    min = 1;
    max = MENOR(*cantidadDeVidasDelJugador, *cantidadDeCaracteresDeSecuenciaIngresados + 1);
    cantidadIngresada = ingresaYValida(min, max);
    (*cantidadDeVidasDelJugador) -= cantidadIngresada;
    ronda->vidasUsadas += cantidadIngresada;

    while(cantidadIngresada && sacarUltimoEnListaSimple(&ronda->secuenciaIngresada, &aux, sizeof(char)))
    {
        cantidadIngresada--;
        (*cantidadDeCaracteresDeSecuenciaIngresados)--;
    }

    if(cantidadIngresada)
    {
        return REINICIAR_NIVEL;
    }

    recursos->temporizador.detenerTemporizador = 0;
    return INGRESO_SIN_MOSTRAR;
}

///RETORNAR 0 CONDICION DE CORTE[FIN DE RONDA]
///ACTUALIZAR *cantidadDeVidasDelJugador CONDICION DE CORTE(cantVidas < 0)[FIN DE RONDA ACTUAL]
///ACTUALIZAR ronda.puntosObtenidos(esta inicializado en 0)
///ACTUALIZAR ronda.vidasUsadas(esta inicializado en 0)
int ingresoDeSecuencia(tRecursos* recursos, tJugador* jugador, tRonda* ronda, int* cantidadDeVidasDelJugador, unsigned cantidadDeCaracteresDeSecuencia, unsigned tiempoParaIngresarSecuencia)
{
    pthread_t id;
    char ch;
    int cantidadDeCaracteresDeSecuenciaIngresados = 0; //Contador de intentos

    inicializacionDeRecursos(recursos, tiempoParaIngresarSecuencia);
    configuracionesGraficas(recursos);

    pthread_create(&id, NULL, accionParaThreadDeTemporizador, recursos);   //EJECUTA TEMPORIZADOR EN PARALELO
    ///*************************************************************************************************************************************************
                                                                                                    ronda->vidasUsadas = 0;///BORRAR!!!
    ///*************************************************************************************************************************************************
    printf("Ingresa un caracter: %s.\n", CARACTERES_VALIDOS_A_INGRESAR_PARA_SECUENCIA);
    while(1)//DEBO SEGUIR INGRESANDO MIENTRAS -> (TENGA VIDAS y NO CONTESTE BIEN SECUENCIA INGRESADA), (SECUENCIA INGRESADA INCOMPLETA y TENGO TIEMPO)
    {
        //Bucle para esperar la entrada del usuario o el timeout
        while(!recursos->temporizador.timeout)
        {
            if(_kbhit())//Verifica si presione una tecla
            {
                ch = _getch(); //Lee un caracter SIN ESPERAR ENTER

                if(ES_LETRA(ch))
                {
                    ch = toupper(ch);
                }

                if ('X' == ch || 'V' == ch || 'A' == ch || 'R' == ch || 'N' == ch)
                {
                    //Posición para mostrar secuencia ingresada
                    recursos->temporizador.coordenadas.posicionDeTextoLetraIngresada.X = recursos->temporizador.coordenadas.posicionDeOrigen.X;
                    recursos->temporizador.coordenadas.posicionDeTextoLetraIngresada.Y = recursos->temporizador.coordenadas.posicionDeOrigen.Y + cantidadDeCaracteresDeSecuenciaIngresados + 2;

                    SetConsoleCursorPosition(recursos->temporizador.hConsole, recursos->temporizador.coordenadas.posicionDeTextoLetraIngresada); // Mueve el cursor para mostrar el caracter ingresado
                    printf("Letra de secuencia ingresada: %c", ch); //Mostrar el caracter ingresado

                    if('X' != ch)
                    {
                        insertarAlFinalEnListaSimple(&(ronda->secuenciaIngresada), &ch, sizeof(char));
                        cantidadDeCaracteresDeSecuenciaIngresados++; //Incrementar el contador de intentos

                        if(cantidadDeCaracteresDeSecuencia == cantidadDeCaracteresDeSecuenciaIngresados)///INGRESE TODO
                        {
                            if(SON_IGUALES == verificarIgualdadEnCantidadDeElementosYContenidoEnListaSimple(&jugador->secuenciaAsignada, &ronda->secuenciaIngresada, comparaCaracteres))
                            {
                                if(!ronda->vidasUsadas)
                                {
                                    ronda->puntosObtenidos = 3;
                                }
                                else
                                {
                                    ronda->puntosObtenidos = 1;
                                }
                                printf("\nGano la ronda actual. Sumo %d punto/s.\n", ronda->puntosObtenidos);
                                recursos->temporizador.detenerTemporizador = 1;
                                pthread_join(id, NULL);
                                return FIN_DE_RONDA_ACTUAL;
                            }
                            else///INGRESE TODO Y NO ESTA BIEN
                            {
                                if(FIN_DE_JUEGO == usarVida(recursos, jugador, ronda, cantidadDeVidasDelJugador, &cantidadDeCaracteresDeSecuenciaIngresados, cantidadDeCaracteresDeSecuencia, tiempoParaIngresarSecuencia))
                                {
                                    printf("ingreso toda la secuencia y cometio errores.\n");
                                    pthread_join(id, NULL);
                                    return FIN_DE_RONDA_ACTUAL;
                                }
                                else
                                {
                                    ///hacer algo
                                }
                            }
                        }
                    }
                    else
                    {
                        break;  ///ESTO HACE QUE EL CASO !cantidadDeCaracteresDeSecuenciaIngresados funcione BIEN
                    }
                }
            }
        }

        if(!cantidadDeCaracteresDeSecuenciaIngresados)
        {
            if(FIN_DE_JUEGO == usarVida(recursos, jugador, ronda, cantidadDeVidasDelJugador, &cantidadDeCaracteresDeSecuenciaIngresados, cantidadDeCaracteresDeSecuencia, tiempoParaIngresarSecuencia))
            {
                printf("no ingreso ningun caracter de secuencia.\n");
                pthread_join(id, NULL);
                return FIN_DE_RONDA_ACTUAL;
            }
        }

        if(recursos->temporizador.timeout || cantidadDeCaracteresDeSecuenciaIngresados == cantidadDeCaracteresDeSecuencia)
        {
            recursos->temporizador.detenerTemporizador = 1;
            break; //Fin ronda
        }
    }

    pthread_join(id, NULL);///FUERZO AL HILO MAIN a que el hilo del temporizador termine para proseguir con la ejecucion del codigo
    if(recursos->temporizador.timeout)
    {
        recursos->temporizador.coordenadas.posicionDeTextoFinal.X = recursos->temporizador.coordenadas.posicionDeOrigen.X;
        recursos->temporizador.coordenadas.posicionDeTextoFinal.Y = recursos->temporizador.coordenadas.posicionDeOrigen.Y + cantidadDeCaracteresDeSecuenciaIngresados + 2;
        SetConsoleCursorPosition(recursos->temporizador.hConsole, recursos->temporizador.coordenadas.posicionDeTextoFinal);
    }

    printf("\n");
    if(recursos->temporizador.timeout)
    {
        printf("No completaste la secuencia. Se acabo el tiempo.\n");
    }else if(cantidadDeCaracteresDeSecuenciaIngresados == cantidadDeCaracteresDeSecuencia)
        {
            printf("Secuencia ingresada exitosamente!\n");
        }
        else
        {
            printf("Uso de vida.\n");
        }

//    ronda->puntosObtenidos= 1000;   ///DEJAR ESTO ASI PORQUE PERMITE VISUALIZAR CANTIDAD DE VECES QUE SE EJECUTO ESTA FUNCION
//    ronda->vidasUsadas = -1;         ///eso no importa, todavia no uso vidas, es para no olvidarme de en algun momento inicializarlo
//    *cantidadDeVidasDelJugador = 1;    ///FIN DE JUEGO
    return FIN_DE_RONDA_ACTUAL;///FIN DE RONDA ACTUAL
}
