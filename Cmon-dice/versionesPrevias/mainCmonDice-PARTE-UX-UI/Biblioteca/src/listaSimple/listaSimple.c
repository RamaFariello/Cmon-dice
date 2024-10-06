#include "../../include/listaSimple/listaSimple.h"

void crearListaSimple(t_lista* pl)
{
    *pl = NULL;
}

int insertarAlFinalEnListaSimple(t_lista* pl, const void* dato, unsigned tam)
{
    t_nodo* nuevoNodo;

    nuevoNodo = malloc(sizeof(t_nodo));
    if(NULL == nuevoNodo)
    {
        return NO_PUDE_INSERTAR;
    }
    nuevoNodo->dato = malloc(tam);
    if(NULL == nuevoNodo->dato)
    {
        free(nuevoNodo);
        return NO_PUDE_INSERTAR;
    }

    memcpy(nuevoNodo->dato, dato, tam);
    nuevoNodo->tam = tam;
    nuevoNodo->sig = NULL;

    while(*pl)
    {
        pl = &((*pl)->sig);
    }

    *pl = nuevoNodo;
    return OK;
}

int sacarPrimeroEnListaSimple(t_lista* pl, void* dato, unsigned tam)
{
    t_nodo* nodoAEliminar = *pl;

    if(NULL == *pl)
    {
        return NO_HAY_ELEMENTOS;
    }

    *pl = (*pl)->sig;
    memcpy(dato, nodoAEliminar->dato, MENOR(tam, nodoAEliminar->tam));

    free(nodoAEliminar->dato);
    free(nodoAEliminar);

    return OK;
}

void vaciarListaSimple(t_lista *pl)
{
    t_nodo* nodoAEliminar;
    while(*pl)
    {
        nodoAEliminar = *pl;
        *pl = nodoAEliminar->sig;
        free(nodoAEliminar->dato);
        free(nodoAEliminar);
    }
}

void mostrarListaSimpleEnOrden(const t_lista* pl, void (*mostrar)(const void* dato))
{
    printf("\n");
    while(*pl)
    {
        mostrar((*pl)->dato);
        printf("\n");
        pl = &((*pl)->sig);
    }
}

int listaSimpleVacia(const t_lista* pl)
{
    return NULL == *pl;
}

//int insertarOrdenadoSinDuplicados(t_lista* pl, const void* dato, unsigned tam, int (*comparar)(const void* a, const void* b))
//{
//    t_nodo* nuevoNodo;
//
//    while(*pl && comparar(dato, (*pl)->dato) > 0)
//    {
//        pl = &((*pl)->sig);
//    }
//
//    if(!*pl || comparar(dato, (*pl)->dato) < 0)
//    {
//        nuevoNodo = malloc(sizeof(t_nodo));
//        if(NULL == nuevoNodo)
//        {
//            return NO_PUDE_INSERTAR;
//        }
//
//        nuevoNodo->dato = malloc(tam);
//        if(NULL == nuevoNodo->dato)
//        {
//            free(nuevoNodo);
//            return NO_PUDE_INSERTAR;
//        }
//
//        memcpy(nuevoNodo->dato, dato, tam);
//        nuevoNodo->tam = tam;
//        nuevoNodo->sig = *pl;
//        *pl = nuevoNodo;
//    }
//
//    return OK;
//}

int insertarOrdenadoEnListaSimple(t_lista* pl, const void* dato, unsigned tam, int (*comparar)(const void* a, const void* b))
{
    t_nodo* nuevoNodo;

    nuevoNodo = malloc(sizeof(t_nodo));
    if(!nuevoNodo)
    {
        return NO_PUDE_INSERTAR;
    }

    nuevoNodo->dato = malloc(tam);
    if(!nuevoNodo->dato)
    {
        free(nuevoNodo);
        return NO_PUDE_INSERTAR;
    }

    memcpy(nuevoNodo->dato, dato, tam);
    nuevoNodo->tam = tam;

    if(!(*pl) || comparar(dato, (*pl)->dato) < 0)
    {
        nuevoNodo->sig = *pl;
        *pl = nuevoNodo;
        return OK;
    }

    while((*pl)->sig && comparar(dato, (*pl)->sig->dato) > 0)
    {
        pl = &(*pl)->sig;
    }

    nuevoNodo->sig = (*pl)->sig;
    (*pl)->sig = nuevoNodo;

    return OK;
}

void insertarArchivoBinarioEnListaSimple(FILE* pf, t_lista* pl, void* dato, unsigned tam, int (*comparar)(const void* a, const void* b))
{
    fread(dato, tam, 1, pf);
    while(!feof(pf))
    {
        insertarOrdenadoEnListaSimple(pl, dato, tam, comparar);
        fread(dato, tam, 1, pf);
    }
}

void swapDeNodos(t_nodo** nodo1, t_nodo** nodo2)
{
    t_nodo* aux;

    if(NULL != *nodo1 && NULL != *nodo2)
    {
        aux = *nodo1;
        *nodo1 = *nodo2;
        *nodo2 = aux;

        aux = (*nodo1)->sig;
        (*nodo1)->sig = (*nodo2)->sig;
        (*nodo2)->sig = aux;
    }
}

t_nodo** direccionDeSigNodoAnteriorAlAzar(t_nodo** base, unsigned cantidadDeElementos)
{
    unsigned cantidadDeAvance;

    srand(time(NULL));
    cantidadDeAvance = rand() % cantidadDeElementos;

    if(NULL == *base)
    {
        return NULL;
    }

    while(cantidadDeAvance > 0 && (*base)->sig)
    {
        cantidadDeAvance--;
        base = &((*base)->sig);
    }

    return base;
}

void mezclarListaSimpleAleatoriamente(t_lista* pl, unsigned cantidadDeElementos)
{
    t_nodo** retornoSigNodoAnteriorAlAzar;

    if(NULL == *pl || NULL == (*pl)->sig)//hay elementos o hay solo 1
    {
        return;
    }

    ///MEZCLAR EL PRIMERO: es necesario hacer esto para no perder la referencia del main y que siempre apunte al inicio de la lista
    retornoSigNodoAnteriorAlAzar = direccionDeSigNodoAnteriorAlAzar(&((*pl)->sig), cantidadDeElementos - 1);
    if(NULL != retornoSigNodoAnteriorAlAzar)
    {
        swapDeNodos(pl, retornoSigNodoAnteriorAlAzar);
    }
    pl = &((*pl)->sig);//pl seria 112

    ///MEZCLAR LOS SIGUIENTES NODOS
    while((*pl)->sig)
    {
        retornoSigNodoAnteriorAlAzar = direccionDeSigNodoAnteriorAlAzar(&((*pl)->sig), cantidadDeElementos - 2);
        if(NULL != retornoSigNodoAnteriorAlAzar)
        {
            swapDeNodos(pl, retornoSigNodoAnteriorAlAzar);
        }
        pl = &((*pl)->sig);
    }
}
