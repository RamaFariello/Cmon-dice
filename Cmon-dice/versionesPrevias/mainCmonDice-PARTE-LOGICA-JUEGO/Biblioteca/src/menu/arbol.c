 #include "../../include/menu/arbol.h"

void crearArbol(tArbol* a)
{
    *a =NULL;
}

int insertarEnArbol(tArbol* a, const void* d,unsigned tam, int (*comparar)(const void*a,const void* b))
{
    tNodoa* nuevo;
    int cmp;

    if(*a == NULL)
    {
        nuevo = (tNodoa*)malloc(sizeof(tNodoa));
        if(!nuevo)
        {
            return NO_PUDE_INSERTAR;
        }
        nuevo->d = malloc(tam);
        if(!(nuevo->d))
        {
            free(nuevo);
            return NO_PUDE_INSERTAR;
        }
        nuevo->tam = tam;
        nuevo->der=NULL;
        nuevo->izq =NULL;
        memcpy(nuevo->d,d,tam);
        *a = nuevo;
        return OK;
    }
    cmp = comparar((*a)->d,d);

    if(cmp>0)
    {
        return insertarEnArbol(&((*a)->izq),d,tam,comparar);
    }else if(cmp<0)
        {
            return insertarEnArbol(&((*a)->der),d,tam,comparar);
        }
        else
        {
            return OK;//hubo duplicados pero no me interesa, sigo insertando
        }
}

void recorrerPreOrden( const tArbol* a, void (*accion)(const void*a))
{
    if(*a)
    {
        accion((*a)->d); //raiz
        recorrerPreOrden(&((*a)->izq),accion);
        recorrerPreOrden(&((*a)->der),accion);
    }
}

void recorrerOrden( const tArbol* a, void (*accion)(const void*a))
{
    if(*a)
    {
        recorrerOrden(&((*a)->izq),accion);
        accion((*a)->d);//raiz
        recorrerOrden(&((*a)->der),accion);
    }
}

void recorrerPostOrden( const tArbol* a, void (*accion)(const void*a))
{
    if(*a)
    {
        recorrerPostOrden(&((*a)->izq),accion);
        recorrerPostOrden(&((*a)->der),accion);
        accion((*a)->d);//raiz
    }

}

void mostrarArbol(const tArbol* a, void(*mostrarDato)(const void* d))
{
    int contador = 0;
    mostrarArbolR(a, contador, mostrarDato);
}

void mostrarArbolR(const tArbol* a,int contador,void(*mostrarDato)(const void* d))
{
    //int i;
    if(*a == NULL)
    {
        return;
    }

    mostrarArbolR(&((*a)->der), contador + 1, mostrarDato);

    printf("%*c", contador * 5 ,' ');
    /*for(i=0; i<contador;i++)
    {
        printf("\t");
    }*/
    mostrarDato((*a)->d);
    printf("\n");
    mostrarArbolR(&((*a)->izq), contador + 1, mostrarDato);
}

int contadorNodos(const tArbol* a)
{
    return *a? contadorNodos(&((*a)->izq)) +  contadorNodos(&((*a)->der)) + 1: 0;
}

int alturaArbol(const tArbol* a)
{
    return *a? MAYOR(alturaArbol(&((*a)->izq)), alturaArbol(&((*a)->der))) + 1 : 0;
}

int esCompletoANivel(const tArbol* a, int n)
{
    if(!(*a))
    {
        return ARBOL_VACIO;
    }
    if(n== 0)
    {
        return OK;
    }
    return esCompletoANivel(&((*a)->izq),n-1) && esCompletoANivel(&((*a)->der),n-1);
}

int esCompleto(const tArbol *a)
{
    return esCompletoANivel(a,alturaArbol(a)-1);
}

int esBalanceado(const tArbol *a)
{
    return esCompletoANivel(a,alturaArbol(a)-2);
}

void vaciarArbol(tArbol* a)
{
    while(NULL != *a)
    {
        eliminarRaiz(a);
    }
}

int eliminarRaiz(tArbol* a)
{
    tNodoa **remp;
    tNodoa *elim;

    if(!(*a)) //arbol vacio no puedo eliminar nada, devuelvo 0
    {
        return ARBOL_VACIO;
    }
    free((*a)->d);

    if(!((*a)->izq) && !((*a)->der)) //arbol con solo un elemento, lo elimino, devuelvo 1
    {
        free(*a);
        *a = NULL;
        return OK;
    }

    remp = alturaArbol(&(*a)->izq)>alturaArbol(&(*a)->der)? mayorNodo(&(*a)->izq) : menorNodo(&(*a)->der);
    elim = *remp;

    (*a)->d = elim->d;
    (*a)->tam = elim->tam;
    *remp = elim->izq? elim->izq : elim->der;

    free(elim);
    return OK;
}

tNodoa** mayorNodo(const tArbol* a)
{

    while((*a) && (*a)->der)
    {
        a = &((*a)->der);
    }
    return (tNodoa**)a;
}

tNodoa** menorNodo(const tArbol* a)
{

    while((*a) && (*a)->izq)
    {
        a = &((*a)->izq);
    }
    return (tNodoa**)a;
}

int eliminarNodoPorClave(tArbol *a, void* dato, int(*comparar)(const void* a, const void* b))
{
    if(!(*a))
    {
        return ARBOL_VACIO;
    }
    if(comparar(dato,(*a)->d) == 0)
    {
        eliminarRaiz(a);
        return OK;
    }
    if(comparar(dato,(*a)->d)<0)
    {
        return eliminarNodoPorClave(&(*a)->izq,dato,comparar);
    }

    return eliminarNodoPorClave(&(*a)->der,dato,comparar);
}

tNodoa* buscarPorClave(tArbol *a, void* dato, int(*comparar)(const void* a,const void* b))
{
    if(!(*a) || comparar(dato,(*a)->d) == 0)
    {
        return *a;
    }
    if(comparar(dato,(*a)->d)<0)
    {
        return buscarPorClave(&(*a)->izq,dato,comparar);
    }

    return buscarPorClave(&(*a)->der,dato,comparar);
}
