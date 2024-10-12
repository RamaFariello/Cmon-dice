#include "funciones.h"

void mostrarCaracter(const void* dato)  ///HARDCODEADO
{
    printf("%c", *(char*)dato);
}

void juegarRondas(void* vRecursos, void* vJugador, int* retornoCodigoDeError) //ex funcion inicializarJugador[esta funcion es la que se ejecuta en el map]
{
    tRecursos* recursos = (tRecursos*)vRecursos;
    tJugador* jugador = (tJugador*)vJugador;
    int puntosTotalesAcumulados;
    int cantidadDeVidasSegunNivelDeConfiguracionElegido;
    char letra;///HARDCODEADO

    puntosTotalesAcumulados = 0;
    cantidadDeVidasSegunNivelDeConfiguracionElegido = (recursos->configuraciones)[recursos->indiceDeNivelDeConfiguracionElegida].cantidadDeVidas;
    recursos->cantidadDeVidasUsadasTotales = 0;

    recursos->ronda.puntosObtenidos = 0;
    recursos->ronda.vidasUsadas = 0;

    printf("Simulando juego:");                                                                         ///HARDCODEADO
    printf("\nEn este momento esta jugando:\n");
    mostrarJugador(jugador);
    while(cantidadDeVidasSegunNivelDeConfiguracionElegido - recursos->cantidadDeVidasUsadasTotales >= 0)
    {
        if(OK != (*retornoCodigoDeError = pedirLetraAleatoria(recursos, &letra)))
        {
            fprintf(stderr, "No pude obtener letra aleatoria para formar secuencia.\n");
            break;
        }
        insertarAlFinalEnListaSimple(&(jugador->secuenciaAsignada), &letra, sizeof(char));

        puntosTotalesAcumulados = 1000;             ///HARDCODEADO para ver algo, aca va la cantidad real acumulada
        recursos->cantidadDeVidasUsadasTotales++;   ///HARDCODEADO PARA NO ESTAR EN BUCLE INFINITO: lo modifico segun lo que el jugador haga en las rondas
    }

    if(OK == *retornoCodigoDeError)                                                                         ///HARDCODEADO
    {
        printf("\nSecuencia asignada:");                                                                    ///HARDCODEADO
        mostrarListaSimpleEnOrdenFormatoEspecial(&(jugador->secuenciaAsignada), mostrarCaracter);           ///HARDCODEADO
        printf("Cantidad de puntos totales acumulados por el jugador: %d\n", puntosTotalesAcumulados);      ///HARDCODEADO
        jugador->puntosTotales = puntosTotalesAcumulados;                                                   ///HARDCODEADO
    }

    printf("\n");                                                                                       ///HARDCODEADO
    system("pause");                                                                                    ///HARDCODEADO
    system("cls");                                                                                      ///HARDCODEADO
}

int iniciarJuego(tRecursos* recursos)
{
    int retornoCodigoDeError;

    recursos->datoRespuestaAPI.buffer = NULL;//para hacer free de NULL y no de basura en caso de errores
    recursos->cantidadDeIndicesDeCaracteresDeSecuenciaRestantes = 0;

    mapEnListaSimple(&(recursos->listaDeJugadores), recursos, &retornoCodigoDeError, juegarRondas);

    if(OK != retornoCodigoDeError)
    {
        fprintf(stderr, "Juego suspendido.\n");
    }

    liberarRecursosParaConsumoDeAPI(recursos);
    return retornoCodigoDeError;
}

//int inicializarJugador(tRecursos* recursos)
//{
//    /// Asignar parámetros al jugador
//    // jugador.cantidadVidas = recursos->configuración[INDICE_DIFICULTAD].cantidadDeVidas
//    // jugador.puntosTotales = 0;
//    // crearLista(secuencia);
//
//    while(cantidadVidas >= 0)
//    {
//        //juegarRonda(recursos);
//    }
//
//}
//
//int jugarRonda(tRecursos* recursos)
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

