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
        recursos->temporizador.timeout = 1;//SE TERMINO EL TIEMPO
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
