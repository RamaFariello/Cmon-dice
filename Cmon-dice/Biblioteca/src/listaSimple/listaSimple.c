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
        pl = &((*pl)->sig);
    }
    printf("\n");
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

void* mapEnListaSimple(const t_lista* pl, void* recursos, int* retornoCodigoDeError, void (*accion)(void* recursos, void* dato, int* retornoCodigoDeError))
{
    t_nodo* inicioLista = *pl;
    int usoCodigoDeError = 0;

    if(NULL != retornoCodigoDeError)
    {
        usoCodigoDeError = 1;
        *retornoCodigoDeError = OK;
    }

    while(*pl)
    {
        if(usoCodigoDeError && OK != *retornoCodigoDeError)
        {
            fprintf(stderr, "Fallo aplicando accion en map.\n");
            return NULL;
        }
        accion(recursos, (*pl)->dato, retornoCodigoDeError);

        pl = &((*pl)->sig);
    }

    return inicioLista;
}

int verificarIgualdadEnCantidadDeElementosYContenidoEnListaSimple(t_lista* lista1, t_lista* lista2, int(*comparar)(const void* a, const void* b))
{
    int cmp;

    if(!*lista1 || !*lista2)
    {
        return LISTAS_VACIAS;
    }

    while(*lista1 && *lista2 && !(cmp = comparar((*lista1)->dato, (*lista2)->dato)))
    {
        lista1 = &((*lista1)->sig);
        lista2 = &((*lista2)->sig);
    }

    if((*lista1 || *lista2) || cmp)
    {
        return NO_SON_IGUALES;
    }

    return SON_IGUALES;
}

int sacarUltimoEnListaSimple(t_lista* pl, void* dato, unsigned tam)
{
    t_nodo* nodoAEliminar;

    if(!*pl)
    {
        return NO_HAY_ELEMENTOS;
    }

    while((*pl)->sig)
    {
        pl = &((*pl)->sig);
    }

    nodoAEliminar = *pl;
    memcpy(dato, nodoAEliminar->dato, MENOR(nodoAEliminar->tam, tam));

    *pl = nodoAEliminar->sig;

    free(nodoAEliminar->dato);
    free(nodoAEliminar);

    return OK;
}

///imprimirNNodosEnListaSimple(rondaFinalizada->secuenciaAsignada, *(rondaFinalizada->archivo), ronda->cantidadDeCaracteresDeSecuencia, imprimirSecuencia);
void imprimirNNodosEnListaSimple(t_lista* pl, FILE* pf, int cantidadDeElementosAImprimir, int* retornoCodigoDeError, void(*imprimir)(void* pf, void* dato, int* retornoCodigoDeError))
{
    if(cantidadDeElementosAImprimir < 0)
    {
        fprintf(stderr, "Cantidad de elementos a mostrar negativa.\n");
        return;
    }

    while(*pl && cantidadDeElementosAImprimir)
    {
        imprimir(pf, (*pl)->dato, retornoCodigoDeError);
        cantidadDeElementosAImprimir--;
        pl = &((*pl)->sig);
    }
}

t_nodo** buscarMenor (t_lista *pl, int (*comparar)(const void* a, const void* b))
{
    t_nodo** menor = pl;

    while(*pl)
    {
        pl = &((*pl)->sig);

        if((*pl) && comparar((*pl)->dato,(*menor)->dato)<0)
        {
            menor = pl;
        }
    }
    return menor;
}

void ordenarLista (t_lista *pl, int (*comparar)(const void* a, const void* b))
{
    t_nodo** menor;
    t_nodo* aux;

    while(*pl)
    {
        menor = buscarMenor(pl,comparar);
        if(comparar((*menor)->dato,(*pl)->dato) != 0)
        {
            aux = *pl;
            *pl = *menor;
            *menor = aux;

            aux = (*pl)->sig;
            (*pl)->sig = (*menor)->sig;
            (*menor)->sig = aux;
        }
        pl = &((*pl)->sig);
    }

}

void* filtrarPorClaveEnListaSimple(t_lista* pl, const void* clave, void* recursos, int* retornoCodigoDeError, int(*comparar)(const void* a, const void* b), void (*accion)(void* recursos, void* dato, int* retornoCodigoDeError))
{
    t_nodo* inicioLista = *pl;

    while(*pl)
    {
        if(!comparar((*pl)->dato, clave))
        {
            accion(recursos, (*pl)->dato, retornoCodigoDeError);
        }
        pl = &((*pl)->sig);
    }

    return inicioLista;
}






