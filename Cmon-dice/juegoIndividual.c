#include "funciones.h"

int iniciarJuego(tRecursos* recursos)
{
    int retornoCodigoDeError;
    //mapEnListaSimple(&(recursos->listaDeJugadores), recursos, inicializarJugador);
    return OK;
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
