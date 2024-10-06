#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curl.h"

#include "./Biblioteca/include/listaSimple/listaSimple.h"
#include "./Biblioteca/include/menu/menu.h"
#include "./Biblioteca/include/generico.h"

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
#define NO_PUDE_ABRIR_ARCHIVO_TXT_CONFIGURACION -1  //NO LO PUDE ABRIR
#define ARCHIVO_TXT_DE_CONFIGURACION_CON_ERRORES -2 //LO PUDE ABRIR, PERO TENIA ERRORES DE FORMATO

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

///generarRondas
#define COLOR_VERDE 'V'
#define COLOR_AMARILLO 'A'
#define COLOR_ROJO 'R'
#define COLOR_NARANJA 'N'

///jugar
#define CARACTERES_VALIDOS_A_INGRESAR_PARA_SECUENCIA "X-V-A-R-N"

///switchTextoMenu
#define JUGAR 'A'

#define OK 1

typedef struct
{
    unsigned id;
    char nya[TAM_NyA];
    unsigned cantidadDeVidas;
    t_lista rondasJugadas;
    t_lista secuenciaFinalRespondida;///COLA PARA GRABAR, PILA PARA USO DE VIDAS
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
    tReconstruccionDato datoRespuestaAPI;
    char* cadenaConIndices;
}tRecursos;

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

void imprimirResultados(FILE* pf, tRecursos* recursos);
void construccionNombreArchivoTxtInforme(char* NOMBRE_ARCHIVO_TXT_INFORME, unsigned tam, struct tm* fechaYHora);
int generarInforme(tRecursos* recursos, void (*construccionNombreArchivoTxtInforme)(char* NOMBRE_ARCHIVO_TXT_INFORME, unsigned tam, struct tm* fechaYHora));

char obtenerCaracterDeSecuencia(const char* cadenaConIndices, const char* caracteresDeSecuencia);
int generarRondas(tRecursos* recursos);
int iniciarJuego(tRecursos* recursos);
int jugar(tRecursos* recursos);

void switchTextoMenu(int opcion, void* recursos);

#endif // FUNCIONES_H_INCLUDED
