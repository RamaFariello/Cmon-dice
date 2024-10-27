#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>     //Para _kbhit() y _getch()
#include <windows.h>   //Para Sleep y funciones de Windows
#include <pthread.h>   //Para usar hilos para temporizador en paralelo

#include "curl.h"

#include "./Biblioteca/include/listaSimple/listaSimple.h"
#include "./Biblioteca/include/menu/menu.h"
#include "./Biblioteca/include/generico.h"

#define REINICIAR_NIVEL -1
#define FIN_DE_JUEGO 0
#define INGRESO_SIN_MOSTRAR 1

#define FIN_DE_RONDA_ACTUAL -260611


///tJugador
#define TAM_NyA 100

///tRecursos
#define CANTIDAD_DE_NIVELES 3

///cargarConfiguraciones
#define NOMBRE_ARCHIVO_TXT_CONFIGURACION "config.txt"

#define TAM_BUFFER_CARGA_CONFIGURACIONES 11///cantidad maxima de caracteres posibles contando \0
#define MIN_CANT_VIDAS 0
#define MAX_CANT_VIDAS 5
#define MIN_TIEMPO_JUEGO_POR_RONDA 0
#define MAX_TIEMPO_JUEGO_POR_RONDA 20
///errores main y cargarConfiguraciones
#define NO_PUDE_ABRIR_ARCHIVO_TXT_CONFIGURACION 0  //NO LO PUDE ABRIR
#define ARCHIVO_TXT_DE_CONFIGURACION_CON_ERRORES 0 //LO PUDE ABRIR, PERO TENIA ERRORES DE FORMATO

///validoIngresoDeNombre
#define NOMBRE_INVALIDO 0
#define ES_BLANCO(X) (' ' == (X))

///ingresoDeNombresAListaSimple
#define CARACTER_DE_SALIDA 'X'
#define NO_HAY_JUGADORES 0

///ERROR defineIndiceDeNivelSegunCaracter e ingresoDeNivel
#define INDICE_INVALIDO -1

///mostrarConfiguracionElegida - defineIndiceDeNivelSegunCaracter
#define FACIL 'F'
#define MEDIO 'M'
#define DIFICIL 'D'
#define INDICE_NIVEL_FACIL 0
#define INDICE_NIVEL_MEDIO 1
#define INDICE_NIVEL_DIFICIL 2


///construccionURL
#define CANT_RONDAS_PROMEDIO_JUGADAS 15
///consumoAPI
#define INCLUIR_BARRA_N 2
#define CONTAR_BARRA_CERO 1

#define URL_BASE "https://www.random.org/integers/"
#define QUERY_PARAMS_FORMATO "?min=0&max=3&col=1&base=10&format=plain&rnd=new"
#define QUERY_PARAM_CANTIDAD_DE_ELEMENTOS "&num="

///consumoAPI
#define TAM_URL 1000

///generarInforme
#define TAM_NOMBRE_ARCHIVO_INFORME 100
#define NO_PUDE_ABRIR_ARCHIVO_TXT_INFORME -1

///obtenerCaracterDeSecuencia
#define A_NUMERO(X) ((X) - '0')

///pedirLetraAleatoria
#define COLOR_VERDE 'V'
#define COLOR_AMARILLO 'A'
#define COLOR_ROJO 'R'
#define COLOR_NARANJA 'N'
#define RANGO_MIN_DE_INDICE_VALIDO '0'
#define RANGO_MAX_DE_INDICE_VALIDO '3'
#define ES_RANGO_INDICE_VALIDO(X) (((X) >= (RANGO_MIN_DE_INDICE_VALIDO)) && ((X) <= (RANGO_MAX_DE_INDICE_VALIDO)))
#define NO_PUDE_ASIGNAR_CARACTER_DE_SECUENCIA -4

///jugar
#define CARACTERES_VALIDOS_A_INGRESAR_PARA_SECUENCIA "X-V-A-R-N"

///switchTextoMenu
#define JUGAR 'A'

#define OK 1

typedef struct
{
    COORD posicionDeOrigen;
    COORD posicionDeTextoLetraIngresada;
    COORD posicionDeTextoFinal;
    COORD posicionDelTemporizadorEnConsola;
}tCoordenadas;

typedef struct
{
    tCoordenadas coordenadas;
    HANDLE hConsole;
    int timeout;
    int tiempoRestanteParaTemporizador;
    int detenerTemporizador;
}tTemporizador;

typedef struct
{
    FILE** archivo;
    t_lista* secuenciaAsignada;
}tRondaFinalizadaInforme;

typedef struct
{
    t_lista secuenciaIngresada;
    unsigned cantidadDeCaracteresDeSecuencia;
    unsigned puntosObtenidos;
    unsigned vidasUsadas;
}tRonda;

typedef struct
{
    unsigned id;
    char nya[TAM_NyA];
    unsigned puntosTotales;
    t_lista rondasJugadas;  ///contenido de esta lista: NODOS de tipo tRonda
    t_lista secuenciaAsignada;
}tJugador;

typedef struct
{
    unsigned tiempoDeVisualizacionSecuenciaCorrecta;
    unsigned tiempoRespuestaPorRonda;
    unsigned cantidadDeVidas;
}tConfiguracion;

typedef struct
{
    tConfiguracion configuraciones[CANTIDAD_DE_NIVELES];
    unsigned indiceDeNivelDeConfiguracionElegida;

    t_lista listaDeJugadores;
    unsigned cantidadDeJugadores;
    unsigned mayorPuntajeTotal;

    tReconstruccionDato datoRespuestaAPI;
    char* cadenaDeIndicesTraidosDeAPI;
    unsigned cantidadDeIndicesDeCaracteresDeSecuenciaRestantes;

    tTemporizador temporizador;
}tRecursos;

///***********************
int comparaCaracteres(const void* a, const void* b);

void ocultarCursor(tRecursos* recursos);
void limpiarConsola(tRecursos* recursos);
void deshabilitarQuickEditMode();
void* accionParaThreadDeTemporizador(void* arg);
void configuracionesGraficas(tRecursos* recursos);
void inicializacionDeRecursos(tRecursos* recursos, unsigned maximoTiempoParaIngresoDeRespuesta);

void mostrarSecuenciaAsignada(tRecursos* recursos, tJugador* jugador, unsigned tiempoParaVisualizarSecuencia);
int ingresoDeSecuencia(tRecursos* recursos, tJugador* jugador, tRonda* ronda, int* cantidadDeVidasDelJugador, unsigned cantidadDeCaracteresDeSecuencia, unsigned tiempoParaIngresarSecuencia);
///***********************
void mostrarCaracter(const void* dato);

void mostrarConfiguracionElegida(tConfiguracion* configuracion, unsigned indiceDeNivelDeConfiguracionElegida);
int cargarConfiguraciones(FILE* aConfiguracion, tConfiguracion* configuraciones);

void mostrarJugador(const void* dato);
int validoIngresoDeNombre(const char* cadena);
int ingresoDeNombresAListaSimple(t_lista* listaDeJugadores, unsigned* cantidadDeJugadores);
int defineIndiceDeNivelSegunCaracter(char caracter);
void ingresoDeNivel(unsigned* indiceDeNivelDeConfiguracionElegida);

int validaFormatoRespuestaAPI(const char* respuesta);
void construccionURL(char* URL, unsigned tam, unsigned ce);
int consumoAPI(tReconstruccionDato* dato, unsigned cantidadDeJugadores, void (*construccionURL)(char* URL, unsigned tam, unsigned ce));
int inicializarRecursosParaConsumoDeAPI(tRecursos* recursos);
void liberarRecursosParaConsumoDeAPI(tRecursos* recursos);

void imprimirResultados(FILE* pf, tRecursos* recursos);
void construccionNombreArchivoTxtInforme(char* NOMBRE_ARCHIVO_TXT_INFORME, unsigned tam, struct tm* fechaYHora);
int generarInforme(tRecursos* recursos, void (*construccionNombreArchivoTxtInforme)(char* NOMBRE_ARCHIVO_TXT_INFORME, unsigned tam, struct tm* fechaYHora));

int convertirIndiceEnCaracterDeSecuencia(char caracterIndice, char* letra);
int obtenerCaracterDeSecuenciaAleatorio(tRecursos* recursos, char* letra);
int pedirLetraAleatoria(tRecursos* recursos, char* letra);
int iniciarJuego(tRecursos* recursos);

void mostrarCaracteresValidos();
void liberarListaDeSecuenciasIngresadasPorRonda(void* vRecursos, void* vRonda, int* retornoCodigoDeError);
void liberarListasDeCadaJugador(void* vRecursos, void* vJugador, int* retornoCodigoDeError);
int jugar(tRecursos* recursos);
void switchTextoMenu(int opcion, void* recursos);

#endif // FUNCIONES_H_INCLUDED
