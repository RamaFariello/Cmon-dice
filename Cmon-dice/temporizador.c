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
    }
    while(!(cantidadIngresada >= min && cantidadIngresada <= max));

    return cantidadIngresada;
}
/* if('X' == ch)
 {
     ///uso vida
     deboIngresarSecuencia=1;
 }else //Si el tiempo se acabo o si se completo toda la secuencia ocurriria lo mismo
     {
         ///verificar igualdad de secuencias
         //si -> gane la ronda
                     //deboIngresarSecuencia=0;
         //no -> USO VIDA
                 //SI USE LA VIDA deboIngresarSecuencia=1;
                 //SI NO, deboIngresarSecuencia=0;

     }*/

///NO TERMINE DE ESCRIBIR LA SECUENCIA
int usarVida(tRecursos* recursos, tJugador* jugador, tRonda* ronda, int* cantidadDeVidasDelJugador, int* cantidadDeCaracteresDeSecuenciaIngresados, unsigned cantidadDeCaracteresDeSecuencia, unsigned tiempoParaIngresarSecuencia, char ch)
{
    int min;///REVISAR!!!
    int max;
    int cantidadIngresada;
    char aux;

    if(SON_IGUALES == verificarIgualdadEnCantidadDeElementosYContenidoEnListaSimple(&(jugador->secuenciaAsignada), &(ronda->secuenciaIngresada), comparaCaracteres))
    {
        printf("\nGanaste flaco");
        if(ronda->vidasUsadas>0)
        {
            ronda->puntosObtenidos= 1;
        }
        else
        {
            ronda->puntosObtenidos= 3;
        }
        return FIN_DE_RONDA_ACTUAL;
    }

    if(!*cantidadDeVidasDelJugador)
    {
        printf("\nNo tiene mas vidas.\nFinalizo el juego, ");
        (*cantidadDeVidasDelJugador)--;
        return FIN_DE_JUEGO;
    }

    recursos->temporizador.tiempoRestanteParaTemporizador = tiempoParaIngresarSecuencia;
    printf("\nTiene %d cantidad de vida/s.\n", *cantidadDeVidasDelJugador);
    //caso 1 -> TIME OUT CON VIDA
    if(recursos->temporizador.timeout || (ch == 'X' && !cantidadDeCaracteresDeSecuenciaIngresados))
    {
        printf("\nTIME OUT O X SIN INGRESAR SECUENCIA.\nSe le ha restado una vida\n");
        (*cantidadDeVidasDelJugador)--;
        (ronda->vidasUsadas)++;
        return REINICIAR_NIVEL;
    }
    //caso 2-> APRETO X con caracteres ingresados o COMPLETO TODO PERO INCORRECTO
    if(ch == 'X' || cantidadDeCaracteresDeSecuencia == *cantidadDeCaracteresDeSecuenciaIngresados)
    {
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
        return INGRESO_SIN_MOSTRAR;

    }
    //caso 4-> NO TERMINO DE INGRESAR LA SECUENCIA
    return INGRESO_SIN_MOSTRAR;
}

///ESTA ES MI VERSION CRAZY
int ingresoDeSecuencia(tRecursos* recursos, tJugador* jugador, tRonda* ronda, int* cantidadDeVidasDelJugador, unsigned cantidadDeCaracteresDeSecuencia, unsigned tiempoParaIngresarSecuencia)
{
    pthread_t id;
    char ch = '\0';
    int cantidadDeCaracteresDeSecuenciaIngresados = 0;
    int deboIngresarSecuencia = 1;

    inicializacionDeRecursos(recursos, tiempoParaIngresarSecuencia);
    configuracionesGraficas(recursos);

    while(FIN_DE_RONDA_ACTUAL != deboIngresarSecuencia)
    {
        printf("Ingresa un caracter: %s.\n", CARACTERES_VALIDOS_A_INGRESAR_PARA_SECUENCIA);
        pthread_create(&id, NULL, accionParaThreadDeTemporizador, recursos);
        recursos->temporizador.timeout = 0;
        recursos->temporizador.tiempoRestanteParaTemporizador = tiempoParaIngresarSecuencia;
        while(!recursos->temporizador.timeout && cantidadDeCaracteresDeSecuenciaIngresados < cantidadDeCaracteresDeSecuencia && 'X' != ch)
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
                    printf("Letra de secuencia ingresada: %c", ch); //Mostrar el caracter ingresado

                    insertarAlFinalEnListaSimple(&(ronda->secuenciaIngresada), &ch, sizeof(char));
                    cantidadDeCaracteresDeSecuenciaIngresados++; //Incrementar el contador de intentos
                }

                if('X' == ch)
                {
                    printf("\nUso de vida.\n");
                }
            }
        }
        ///QUE PASO?, PORQUE SALI => MANEJARLO y darle valor a deboIngresarSecuencia para ver si necesito seguir ingresando o me voy
        pthread_join(id, NULL);
        if(recursos->temporizador.timeout)
        {
            recursos->temporizador.coordenadas.posicionDeTextoFinal.X = recursos->temporizador.coordenadas.posicionDeOrigen.X;
            recursos->temporizador.coordenadas.posicionDeTextoFinal.Y = recursos->temporizador.coordenadas.posicionDeOrigen.Y + cantidadDeCaracteresDeSecuenciaIngresados + 2;
            SetConsoleCursorPosition(recursos->temporizador.hConsole, recursos->temporizador.coordenadas.posicionDeTextoFinal);
        }

        deboIngresarSecuencia= usarVida(recursos, jugador, ronda, cantidadDeVidasDelJugador, &cantidadDeCaracteresDeSecuenciaIngresados, cantidadDeCaracteresDeSecuencia, tiempoParaIngresarSecuencia, ch);

        printf("\n");
        system("pause");
        system("cls");

        if(FIN_DE_RONDA_ACTUAL == deboIngresarSecuencia || FIN_DE_JUEGO == deboIngresarSecuencia)
        {
            return FIN_DE_RONDA_ACTUAL; //salgo del while
        }

//        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
///TEMPORIZADOR SE PAUSA SOLO, esta linea es solo si quiero pausarlo a mano
//        recursos->temporizador.detenerTemporizador = 1;
    }

    return FIN_DE_RONDA_ACTUAL;
}

