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

    fflush(stdout); ///PARA QUE NO HAYA CONFLICTO CON TEXTO PREVIO
    SetConsoleCursorPosition(recursos->temporizador.hConsole, recursos->temporizador.coordenadas.posicionDelTemporizadorEnConsola);
    printf("%d seg   ", recursos->temporizador.tiempoRestanteParaTemporizador); // Muestra el temporizador

    //Bucle del temporizador
    while(recursos->temporizador.tiempoRestanteParaTemporizador > 0 && !recursos->temporizador.detenerTemporizador)
    {
        fflush(stdout); // Asegura que se muestre inmediatamente
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

    fflush(stdout);
}

void mostrarSecuenciaAsignada(tRecursos* recursos, tJugador* jugador, unsigned tiempoDeVisualizacion)
{
    pthread_t id;

    inicializacionDeRecursos(recursos, tiempoDeVisualizacion);
    configuracionesGraficas(recursos);
    printf("Secuencia asignada para esta ronda:\n");

    pthread_create(&id, NULL, accionParaThreadDeTemporizador, recursos);

    mostrarListaSimpleEnOrden(&(jugador->secuenciaAsignada), mostrarCaracter);

    pthread_join(id, NULL);
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

void ingresoCantidadDeVidas(int* cantidadDeVidasIngresadas, int cantidadDeCaracteresDeSecuenciaIngresados, int cantidadDeVidasRestantes)
{
    printf("\n[CANTIDAD DE VIDAS RESTANTES: %d]\n", cantidadDeVidasRestantes);
    printf("Ingrese la cantidad de vidas que desea usar:\t");
    do
    {
        fflush(stdin);
        printf("Ingrese cantidad de vidas:\t");
        scanf("%d", cantidadDeVidasIngresadas);

        if(*cantidadDeVidasIngresadas <= 0)
        {
            printf("\nIngreso de cantidad de vidas invalido, debe ser mayor a 0.\n");
        }else if(cantidadDeVidasRestantes < *cantidadDeVidasIngresadas)
            {
                printf("\nIngreso de cantidad de vidas invalido, la cantidad de vidas a usar es mayor a la cantidad de vidas restantes.\n");
            }
            else if(cantidadDeCaracteresDeSecuenciaIngresados < *cantidadDeVidasIngresadas)
                {
                    printf("\nIngreso de cantidad de vidas invalido, la cantidad de vidas ingresadas supera a la cantidad de caracteres de secuencia ingresados.\n");
                }
    }while(*cantidadDeVidasIngresadas <= 0 || cantidadDeVidasRestantes < *cantidadDeVidasIngresadas || cantidadDeCaracteresDeSecuenciaIngresados < *cantidadDeVidasIngresadas);
}

int usoDeVida(tRecursos* recursos, tJugador* jugador, int* cantidadDeCaracteresDeSecuenciaIngresados, int cantidadDeVidasSegunConfiguracion)
{
    int cantidadDeVidasIngresadas;
    char caracter;

    if(!(cantidadDeVidasSegunConfiguracion - recursos->cantidadDeVidasUsadasTotales > 0))
    {
        printf("\nYa no tiene mas vidas.\n");
        return FIN_DE_RONDA;
    }

    if(
       !*cantidadDeCaracteresDeSecuenciaIngresados &&
       (((recursos->configuraciones)[recursos->indiceDeNivelDeConfiguracionElegida]).cantidadDeVidas)
       )///TENGO VIDAS y NO INGRESE NADA, reinicio ronda
    {
        printf("Se le ha restado una vida. Le quedan: %d vida/s.\n", cantidadDeVidasSegunConfiguracion - recursos->cantidadDeVidasUsadasTotales);
        (recursos->ronda.vidasUsadas)++;
        return DEBO_REPETIR_RONDA;
    }

    ///TENIA VIDA, INGRESE MAS DE 1 CARACTER y TOQUE TECLA DE USO DE VIDAS o ingrese TODO y estaba mal
    ingresoCantidadDeVidas(&cantidadDeVidasIngresadas, *cantidadDeCaracteresDeSecuenciaIngresados, cantidadDeVidasSegunConfiguracion - recursos->cantidadDeVidasUsadasTotales);
    (*cantidadDeCaracteresDeSecuenciaIngresados) -= cantidadDeVidasIngresadas;
    (recursos->ronda.vidasUsadas) += cantidadDeVidasIngresadas;

    while(cantidadDeVidasIngresadas && sacarUltimoEnListaSimple(&(recursos->ronda.secuenciaIngresada), &caracter, sizeof(char)))
    {
        cantidadDeVidasIngresadas--;
    }

    return DEBO_REPETIR_RONDA;///SIN REINICIAR
}


///ACTUALIZAR recursos->ronda.puntosObtenidos
///ACTUALIZAR recursos->ronda.vidasUsadas
///Siempre que "devuelva" USO_DE_VIDA me tengo que quedar en esta funcion, solo tiene 2 posibles retornos: FIN_DE_RONDA o DEBO_REPETIR_RONDA
int ingresoDeSecuencia(tRecursos* recursos, tJugador* jugador, int cantidadDeVidasSegunConfiguracion, unsigned maximaCantidadDeCaracteresDeSecuencia, unsigned maximoTiempoParaIngresoDeRespuesta)
{
    pthread_t id; //ID del hilo
    char ch;
    int cantidadDeCaracteresDeSecuenciaIngresados = 0; //Contador de intentos

    inicializacionDeRecursos(recursos, maximoTiempoParaIngresoDeRespuesta);
    configuracionesGraficas(recursos);

    printf("Ingresa un caracter: %s.\n", CARACTERES_VALIDOS_A_INGRESAR_PARA_SECUENCIA);

    pthread_create(&id, NULL, accionParaThreadDeTemporizador, recursos);   //EJECUTA TEMPORIZADOR EN PARALELO
    while(1)//Bucle principal para ingresar la secuencia
    {
        //Bucle para esperar la entrada del usuario o el timeout
        while(!recursos->temporizador.timeout)
        {
            fflush(stdin);
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
                        insertarAlFinalEnListaSimple(&(recursos->ronda.secuenciaIngresada), &ch, sizeof(char));
                        cantidadDeCaracteresDeSecuenciaIngresados++; //Incrementar el contador de intentos
                        break;
                    }
                    else
                    {
                        recursos->temporizador.detenerTemporizador = 1;
                        pthread_join(id, NULL);
                        return usoDeVida(recursos, jugador, &cantidadDeCaracteresDeSecuenciaIngresados, cantidadDeVidasSegunConfiguracion);
                    }
                }
            }
        }

        if(recursos->temporizador.timeout || cantidadDeCaracteresDeSecuenciaIngresados == maximaCantidadDeCaracteresDeSecuencia)
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

    if(cantidadDeCaracteresDeSecuenciaIngresados == maximaCantidadDeCaracteresDeSecuencia)
    {
            if(SON_IGUALES == verificarIgualdadEnCantidadDeElementosYContenidoEnListaSimple(&(jugador->secuenciaAsignada), &(recursos->ronda.secuenciaIngresada), comparaCaracteres))
            {
                if(!(recursos->ronda.vidasUsadas))
                {
                    recursos->ronda.puntosObtenidos = 3;
                }
                else
                {
                    recursos->ronda.puntosObtenidos = 1;
                }
                return FIN_DE_RONDA;
            }
    }
    else if(!cantidadDeCaracteresDeSecuenciaIngresados)
        {
            printf("No ingreso nada. Se termino el tiempo.\n");
        }

    return usoDeVida(recursos, jugador, &cantidadDeCaracteresDeSecuenciaIngresados, cantidadDeVidasSegunConfiguracion);
}
