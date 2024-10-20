#include "funciones.h"

void mostrarCaracter(const void* dato)
{
    printf("%c\n", *(char*)dato);
}

//void temporizador(int segundos)
//{
//    double tiempoLimite = (double)segundos; // Tiempo límite en segundos
//
//    // Capturar el tiempo de inicio
//    clock_t tiempoInicio = clock();
//
//    while (1)
//    {
//
//        // Calcular el tiempo transcurrido
//        clock_t tiempoActual = clock();
//        double tiempoTranscurrido = (double)(tiempoActual - tiempoInicio) / CLOCKS_PER_SEC;
//
//        // Verificar si han pasado los segundos
//        if (tiempoTranscurrido >= tiempoLimite)
//        {
//            break;
//        }
//
//        // Agregar un pequeño retraso para evitar consumir demasiados recursos
//        Sleep(100); // 100 milisegundos
//    }
//
//    //si salgo del while es que se agotó el tiempo
//    printf("Tiempo agotado.\n");
//}

//int respuestaJugador(tRecursos* recursos, tJugador* jugador, unsigned tiempoVisualizacion, unsigned tiempoRespuesta)
//{
//    char caracterIngresado[2];
//    t_lista secuenciaIngresada;
//    crearListaSimple(&secuenciaIngresada);
//
//    ///muestro la secuencia por tiempo configurado.
//    printf("\nSecuencia asignada:");
//    mostrarListaSimpleEnOrdenFormatoEspecial(&(jugador->secuenciaAsignada), mostrarCaracter);
//    temporizador(tiempoVisualizacion);
//    system("cls");
//
//
////    /// 4. El jugador ingresa su respuesta (magia del juego)
///// VER DE PONER EL LÍMITE DEL TIEMPO
//    fflush(stdin);
//    fgets(caracterIngresado,2,stdin);
//    do
//    {
//        insertarAlFinalEnListaSimple(&secuenciaIngresada, &caracterIngresado, sizeof(char));
//    }
//    while(strstr(caracterIngresado,CARACTERES_VALIDOS_A_INGRESAR_PARA_SECUENCIA));
//
////    /** COSAS A TENER EN CUENTA:
////        - USO DE VIDAS: resetean el tiempo, pueden finalizar la jugada (si se acaban),
////         hay que retroceder en la lista de respuesta según la cantidad de vidas usadas.
////        - TIEMPO: si se acaba el tiempo se pierde una vida y se vuelve a mostrar la secuencia.
//    // RECORDAR: se suma 1 segundo al tiempo de respuesta por cada respuesta correcta (o por cada ronda superada).
////     */
////
////    /// 4.1 El jugador sigue en juego
//
////
////    /// 4.2 El jugador PERDIÓ --> Se quedó sin vida.
//
///// 4.3 obligatorio: se guarda la respuesta mandada en jugador.respuestaFinal (no importa si acertó o no).
////    //se tiene que guardar la última respuesta que hizo que el jugador perdiera + puntos ganados (técnicamente 0).
//    vaciarListaSimple(&secuenciaIngresada);
//    return OK;
//}

/// generarRondas es por CADA RONDA del jugador.
int generaRondas(tRecursos* recursos, tJugador* jugador, int cantidadDeVidasSegunConfiguracion, int* retornoCodigoDeError)
{
    char letra;
    int puntosTotalesAcumulados = 0;
    unsigned cantidadDeCaracteresDeSecuencia = 1;

    while(cantidadDeVidasSegunConfiguracion - recursos->cantidadDeVidasUsadasTotales >= 0) //mientras tenga vidas.
    {
        if(OK != (*retornoCodigoDeError = pedirLetraAleatoria(recursos, &letra))) //si no recibo una letra válida
        {
            fprintf(stderr, "No pude obtener letra aleatoria para formar secuencia.\n");
            return *retornoCodigoDeError;
        }
        //si recibo una letra válida: inserto en la secuencia asignada
        insertarAlFinalEnListaSimple(&(jugador->secuenciaAsignada), &letra, sizeof(char));

        mostrarSecuenciaAsignada(recursos, jugador, recursos->configuraciones[recursos->indiceDeNivelDeConfiguracionElegida].tiempoDeVisualizacionSecuenciaCorrecta);
        ingresoDeSecuencia(recursos, jugador, cantidadDeCaracteresDeSecuencia,
                           recursos->configuraciones[recursos->indiceDeNivelDeConfiguracionElegida].tiempoRespuestaPorRonda
                           );
        system("pause");
        system("cls");

        cantidadDeCaracteresDeSecuencia++;
        ///HARDCODEADO - BORRAR DESPUÉS: para que no siga en bucle infinito porque todavía no está la lógica de juego terminada
        recursos->cantidadDeVidasUsadasTotales++;
    }
    jugador->puntosTotales = puntosTotalesAcumulados; //posiblemente haya que borrar la variable puntosTotalesAcumulados...
    return OK;
}

/// jugarRondas es UNA VEZ por jugador.
void jugarRondas(void* vRecursos, void* vJugador, int* retornoCodigoDeError) //ex funcion inicializarJugador[esta funcion es la que se ejecuta en el map]
{
    tRecursos* recursos = (tRecursos*)vRecursos;
    tJugador* jugador = (tJugador*)vJugador;
    recursos->cantidadDeVidasUsadasTotales = 0;

    recursos->ronda.puntosObtenidos = 0;
    recursos->ronda.vidasUsadas = 0;

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


//int generarRondasViejo(tRecursos* recursos)
//{
//    /// 1. inicializar variables y listas
//    //crearLista(respuesta);
//
//    //ronda.vidasUsadas = 0;
//    //tiempo = recursos.configuracion[INDICE].tiempo;
//    //ronda.puntosObtenidos = 0;
//    char letra; //para poder guardarla en la secuencia;
//    int retornoCodigoDeError;
//
//    /// 2. Pedir la secuencia
//    if(OK != (retornoCodigoDeError = pedirLetraAleatoria(recursos, &letra)))/// ver lógica para volver a consumir API
//    {
//        return retornoCodigoDeError;
//    }
//
//    /// 3.guardar y mostrar la secuencia
//    //insertarAlFinalDeLista(jugador.secuencia, &letra);
//    // mostrarSecuenciaPorTiempo();
//
//    /// 4. El jugador ingresa su respuesta (magia del juego)
//
//    //ingresarRespuesta();
//    /** COSAS A TENER EN CUENTA:
//        - USO DE VIDAS: resetean el tiempo, pueden finalizar la jugada (si se acaban),
//         hay que retroceder en la lista de respuesta según la cantidad de vidas usadas.
//        - TIEMPO: si se acaba el tiempo se pierde una vida y se vuelve a mostrar la secuencia.
//     */
//
//    /// 4.1 El jugador sigue en juego
//
//    /// 4.2 El jugador PERDIÓ --> Se quedó sin vida.
//    //se tiene que guardar la última respuesta que hizo que el jugador perdiera + puntos ganados (técnicamente 0).
//
//    /// 5.Liberar recursos temporales
//    return retornoCodigoDeError;
//}

