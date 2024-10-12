#include "funciones.h"

/**
MODIFIQUE ESTA FUNCION: ahora puedo pedir la cantidad de letras que quiera y automaticamente se va a consumir la API cuando sea necesario.
Solamente es obligatorio setear: recursos->cantidadDeIndicesDeCaracteresDeSecuenciaRestantes = 0;

Como testear esto:
Ingresar SOLO 1 JUGADOR.
Modificar a gusto la variable ce y observar la cantidad de veces que se consume la API.

FREE de NULL no hace nada, pero FREE de basura rompe todo.
Evito posibles memory leaks
*/

int iniciarJuego(tRecursos* recursos)
{
    int retornoCodigoDeError;
    int i;///HARDCODEADO PARA TESTEAR
    char letra;///HARDCODEADO PARA TESTEAR
    int ce = recursos->cantidadDeJugadores * CANT_RONDAS_PROMEDIO_JUGADAS + 10;///HARDCODEADO PARA TESTEAR[sume 10 para simular caso que me paso de cantidad traida]

    recursos->datoRespuestaAPI.buffer = NULL;  ///Porque es necesario esto?, un jugador pudo haber terminado su turno y sobraron letras. Nunca va a poder liberar la memoria.
    recursos->cantidadDeIndicesDeCaracteresDeSecuenciaRestantes = 0;///ESTA LINEA ES FUNDAMENTAL

    printf("\n[IMPORTANTE]Las funciones:\n-iniciarJuego\n-pedirLetraAleatoria\n\nESTAN HARDCODEADAS con comentarios para analizar comportamiento de los cambios que hizo Agustin[BORRAR LUEGO DE PROBAR].\n\n");    //AB: HARDCODEADO PARA TESTEAR
    system("pause");//AB: HARDCODEADO PARA TESTEAR
    system("cls");//AB: HARDCODEADO PARA TESTEAR

    for(i = 0; i < ce; i++)//AB: HARDCODEADO PARA TESTEAR
    {
        if(OK != (retornoCodigoDeError = pedirLetraAleatoria(recursos, &letra)))    ///NO BORRAR
        {
            fprintf(stderr, "No pude iniciar juego.\n"); ///NO BORRAR
            free(recursos->datoRespuestaAPI.buffer); ///NO BORRAR
            return retornoCodigoDeError;    ///NO BORRAR
        }
    }

    //mapEnListaSimple(&(recursos->listaDeJugadores), recursos, inicializarJugador);

    printf("\n-------------------------------------\n");    //AB: HARDCODEADO PARA TESTEAR
    printf("HICE FREE porque termine el juego.\n");
    printf("\n-------------------------------------\n");
    system("pause");
    system("cls");

    free(recursos->datoRespuestaAPI.buffer);///NO BORRAR
    return retornoCodigoDeError;///NO BORRAR
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
