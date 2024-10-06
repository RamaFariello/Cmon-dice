#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arbol.h"
#define OK 1
#define NO_DEBO_CONVERTIR_LINEA -2147483647
#define RETORNO_LETRA 1
#define RETORNO_NUMERO 2

#define DESACTIVAR_AYUDA_AL_USUARIO 0
#define ACTIVAR_AYUDA_AL_USUARIO 1

#define SALIDA 'B'

#define ES_INGRESO_VALIDO 1
#define TIPO_DE_DATO_INGRESADO_INVALIDO 0
#define NO_PUDE_CARGAR_ARBOL -1
#define OPCION_INVALIDA -1

#define ES_NUMERO(X) ((X) >= '0' && (X) <= '9')
#define ES_LETRA(X) (((X) >= 'A' && (X) <= 'Z') || ((X) >= 'a' && (X) <= 'z'))
#define A_MAYUS(X) (((X) >= 'a' && (X) <= 'z') ? ((X) - ('a'-'A')) : (X))
#define A_CARACTER(X) ((X) + '0')

#define CORCHETE_DE_APERTURA '['
#define CORCHETE_DE_CIERRE ']'

#define TAM_OPCION_INGRESADA 11
#define MAX_TAM_TEXTO 100
#define TAM_COND_SALIDA 2
#define MAX_CANT_CARACTERES_ACEPTADOS 9
/**¿Porque es 11? porque con 9 digitos no me puedo exceder del rango maximo de tipo de dato int
permitiendo 1 más(10) detecto si se ingreso una cadena numerica mucho más larga que el rango valido

Rango de los enteros de 32 bits:
-2147483648
a
2147483647[10 digitos]
*/

int comparaCadenasMenu(const void* a, const void* b);
void mostrarCadenaMenuMenu(const void* data);

char* validaOpcionRetornandoInicioValorClasificandoRetorno(const char* cadena, unsigned* tipoDeRetorno);
int clasificaTextoMenu(const char* cadena, char* retorno);
int validaTipoDeDatoIngresado(const char* opcionIngresada);
void mostrarMenu(char textoMenu[][MAX_TAM_TEXTO], unsigned cantidadDeRegistros);
int ingresoDeOpcion(const tArbol* arbolDeOpciones, char textoMenu[][MAX_TAM_TEXTO], unsigned cantidadDeRegistros);
int cargarMenuAArbolBinarioDeBusqueda(tArbol* arbolDeOpciones, char textoMenu[][MAX_TAM_TEXTO], unsigned cantidadDeRegistros);
void menu(char textoMenu[][MAX_TAM_TEXTO], unsigned cantidadDeRegistros, void(*switchSegunCaso)(int opcion, void* parametroParaSwitchSegunCaso), void* parametroParaSwitchSegunCaso, int ayudaAlUsuario);


#endif // MENU_H_INCLUDED
