#ifndef LISTASIMPLE_H_INCLUDED
#define LISTASIMPLE_H_INCLUDED

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <time.h>
#define NO_PUDE_INSERTAR 0
#define NO_HAY_ELEMENTOS 0

#define LISTAS_VACIAS -1
#define NO_SON_IGUALES 0
#define SON_IGUALES 1

#define OK 1

#define MENOR(X,Y) (((X)>(Y))? (Y):(X))

typedef struct s_nodo
{
    struct s_nodo* sig;
    void* dato;
    unsigned tam;
}t_nodo;

typedef t_nodo* t_lista;


void crearListaSimple(t_lista* pl);
int insertarAlFinalEnListaSimple(t_lista* pl, const void* dato, unsigned tam);
int sacarPrimeroEnListaSimple(t_lista* pl, void* dato, unsigned tam);
void mostrarListaSimpleEnOrden(const t_lista* pl, void (*mostrar)(const void* dato));
void vaciarListaSimple(t_lista *pl);
int listaSimpleVacia(const t_lista* pl);
//int insertarOrdenadoSinDuplicados(t_lista* pl, const void* dato, unsigned tam, int (*comparar)(const void* a, const void* b));
int insertarOrdenadoEnListaSimple(t_lista* pl, const void* dato, unsigned tam, int (*comparar)(const void* a, const void* b));

void insertarArchivoBinarioEnListaSimple(FILE* pf, t_lista* pl, void* dato, unsigned tam, int (*comparar)(const void* a, const void* b));

void swapDeNodos(t_nodo** nodo1, t_nodo** nodo2);
t_nodo** direccionDeSigNodoAnteriorAlAzar(t_nodo** base, unsigned cantidadDeElementos);
void mezclarListaSimpleAleatoriamente(t_lista* pl, unsigned cantidadDeElementos);
void* mapEnListaSimple(const t_lista* pl, void* recursos, int* retornoCodigoDeError, void (*accion)(void* recursos, void* dato, int* retornoCodigoDeError));

int verificarIgualdadEnCantidadDeElementosYContenidoEnListaSimple(t_lista* lista1, t_lista* lista2, int(*comparar)(const void* a, const void* b));
int sacarUltimoEnListaSimple(t_lista* pl, void* dato, unsigned tam);
void imprimirNNodosEnListaSimple(t_lista* pl, FILE* pf, int cantidadDeElementosAImprimir, int* retornoCodigoDeError, void(*imprimir)(void* pf, void* dato, int* retornoCodigoDeError));
t_nodo** buscarMenor (t_lista *pl, int (*comparar)(const void* a, const void* b));
void ordenarLista (t_lista *pl, int (*comparar)(const void* a, const void* b));
void* filtrarPorClaveEnListaSimple(t_lista* pl, const void* clave, void* recursos, int* retornoCodigoDeError, int(*comparar)(const void* a, const void* b), void (*accion)(void* recursos, void* dato, int* retornoCodigoDeError));

#endif // LISTASIMPLE_H_INCLUDED
