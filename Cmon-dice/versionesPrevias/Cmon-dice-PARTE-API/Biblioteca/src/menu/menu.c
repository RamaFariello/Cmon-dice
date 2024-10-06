#include "../../include/menu/menu.h"

int comparaCadenasMenu(const void* a, const void* b)
{
    return strcmpi(a, b);
}

void mostrarCadenaMenuMenu(const void* data)
{
    printf("%s", (char*)data);
}

char* validaOpcionRetornandoInicioValorClasificandoRetorno(const char* cadena, unsigned* tipoDeRetorno)
{
    char* inicioValor;
    unsigned caracteresContados = 0;

    if(CORCHETE_DE_APERTURA == *cadena)
    {
        cadena++;
        inicioValor = (char*)cadena;

        if(!ES_NUMERO(*inicioValor) && !ES_LETRA(*inicioValor))
        {
            return NULL;
        }

        if((ES_LETRA(*inicioValor) && CORCHETE_DE_CIERRE == *(inicioValor + 1)))
        {
            *tipoDeRetorno = RETORNO_LETRA;
            return inicioValor;
        }

        while('\0' != *cadena  && caracteresContados <= MAX_CANT_CARACTERES_ACEPTADOS )
        {
            if(CORCHETE_DE_CIERRE == *cadena)
            {
                *tipoDeRetorno = RETORNO_NUMERO;
                return inicioValor;
            }

            if(!ES_NUMERO(*cadena))
            {
                return NULL;
            }

            cadena++;
            caracteresContados++;
        }
    }

    return NULL;
}

int clasificaTextoMenu(const char* cadena, char* retorno)
{
    char* inicioValor;
    char* finValor;
    unsigned tipoDeRetorno = NO_DEBO_CONVERTIR_LINEA;

    if(
       (inicioValor = validaOpcionRetornandoInicioValorClasificandoRetorno(cadena, &tipoDeRetorno)) &&
       (finValor = strchr(inicioValor, CORCHETE_DE_CIERRE))
       )
    {
        switch(tipoDeRetorno)
        {
            case RETORNO_LETRA:
                    *retorno = *inicioValor;
                    *(retorno + 1) = '\0';
            break;
            case RETORNO_NUMERO:
                strncpy(retorno, inicioValor, (finValor - inicioValor));  //L.End
                retorno[finValor - inicioValor] = '\0';
            break;
        }
    }

    return tipoDeRetorno;
}

int validaTipoDeDatoIngresado(const char* opcionIngresada)
{
    unsigned cantidadDeCaracteresIngresados = strlen(opcionIngresada);

    if(ES_LETRA(*opcionIngresada) && 1 == cantidadDeCaracteresIngresados)
    {
        return OK;
    }

    if(cantidadDeCaracteresIngresados > MAX_CANT_CARACTERES_ACEPTADOS)
    {
        return TIPO_DE_DATO_INGRESADO_INVALIDO;
    }

    while('\0' != *opcionIngresada)
    {
        if(!ES_NUMERO(*opcionIngresada))
        {
            return TIPO_DE_DATO_INGRESADO_INVALIDO;
        }
        opcionIngresada++;
    }

    return OK;
}

void mostrarMenu(char textoMenu[][MAX_TAM_TEXTO], unsigned cantidadDeRegistros)
{
    int i;

    for( i = 0; i < cantidadDeRegistros; i++)
    {
        printf("%s\n", &textoMenu[i][0]);
    }
}

int ingresoDeOpcion(const tArbol* arbolDeOpciones, char textoMenu[][MAX_TAM_TEXTO], unsigned cantidadDeRegistros)
{
    char opcionIngresada[TAM_OPCION_INGRESADA];
    int ingresoInvalido;
    char* retorno;

    do
    {
        system("cls");
        mostrarMenu(textoMenu, cantidadDeRegistros);
        ingresoInvalido = ES_INGRESO_VALIDO;
        fflush(stdin);
        printf("\nIngrese la opcion:\t");
//        scanf("%10s", opcionIngresada);
        fgets(opcionIngresada, TAM_OPCION_INGRESADA - 1, stdin);

        if(NULL != (retorno = strrchr(opcionIngresada, '\n')))
        {
            *retorno = '\0';
        }

        if(
           TIPO_DE_DATO_INGRESADO_INVALIDO == validaTipoDeDatoIngresado(opcionIngresada) ||
           NULL == buscarPorClave((tArbol*)arbolDeOpciones, opcionIngresada, comparaCadenasMenu)
           )
        {
            printf("\nOpcion ingresada invalida, intente nuevamente.\n");
            ingresoInvalido = OPCION_INVALIDA;
            system("pause");
        }
    }while(OPCION_INVALIDA == ingresoInvalido);

    system("cls");
    if(ES_NUMERO(opcionIngresada[0]))
    {
        return atoi(opcionIngresada);
    }

    return A_MAYUS(opcionIngresada[0]);
}

int cargarMenuAArbolBinarioDeBusqueda(tArbol* arbolDeOpciones, char textoMenu[][MAX_TAM_TEXTO], unsigned cantidadDeRegistros)
{
    char retorno[MAX_TAM_TEXTO];
    int i;

    for(i = 0; i < cantidadDeRegistros; i++)
    {
        if(NO_DEBO_CONVERTIR_LINEA != clasificaTextoMenu(&textoMenu[i][0], retorno))
        {
            printf("Opcion cargada: %s\n",retorno);
            if(ES_LETRA(retorno[0]))
            {
                retorno[0] = A_MAYUS(retorno[0]);
            }

            if(OK != insertarEnArbol(arbolDeOpciones, retorno, strlen(retorno) + 1, comparaCadenasMenu))
            {
                return NO_PUDE_CARGAR_ARBOL;
            }
        }
    }
    return OK;
}

void menu(
            char textoMenu[][MAX_TAM_TEXTO], unsigned cantidadDeFilas,
            void(*switchSegunCaso)(int opcion, void* parametroParaSwitchSegunCaso), void* parametroParaSwitchSegunCaso,
            int ayudaAlUsuario
          )
{
    tArbol arbolDeOpciones;
    int opcion;
    char condicionDeSalida[TAM_COND_SALIDA];

    condicionDeSalida[0] = ES_LETRA(SALIDA)? SALIDA : A_CARACTER(SALIDA);
    condicionDeSalida[1] = '\0';

    if(ACTIVAR_AYUDA_AL_USUARIO == ayudaAlUsuario)
    {
        system("cls");
        printf("Como implementar el MENU Generico de manera adecuada:");
        printf("\n***************************************************************************************************\n");
        printf("El Usuario DEBE:\n");
        printf("\t-DECLARAR la matriz de caracteres textoMenu[contiene a las opciones].\n");
        printf("\t\tEjemplo:\n");
        printf("\t\t\tchar textoMenu[][MAX_TAM_TEXTO];\t//puede usar el nombre que prefiera\n");
        printf("\t\t-IMPORTANTE: utilice el MACRO \"MAX_TAM_TEXTO\" para definir TAM COLUMNA.\n\n");
        printf("\t-INICIALIZAR la matriz de caracteres textoMenu(MAXIMO 99 caracteres por linea).\n");
        printf("\tFormatos validos de inicializacion:\n");
        printf("\t\tEjemplos:\n");
        printf("\t\t\t\"texto\"\n");
        printf("\t\t\t\"[A]<texto_opcional>\" \t\tRANGO(A-Z a-z)\n");
        printf("\t\t\t\"[0]<texto_opcional>\" \t\tRANGO(0-999999999)\n\n");
        printf("\t\t-OPCIONAL: incluir texto al lado de los corchetes en las opciones.\n");
        printf("\t\t-IMPORTANTE: las opciones deben estar entre corchetes y sus valores deben estar\n");
        printf("\t\t en el rango de validez indicado, sino se tomaran como lineas de texto.\n\n");
        printf("\t-INCLUIR la OPCION DE SALIDA \"[%c]\" en la matriz de caracteres textoMenu.\n", ES_LETRA(SALIDA)? SALIDA : A_CARACTER(SALIDA));
        printf("\t-ASEGURAR que las opciones inicializadas en la matriz de caracteres textoMenu\n");
        printf("\t esten implementadas en el switch.\n");
        printf("\t-ASEGURAR enviar parametro adicional a funcion de switch si corresponde o NULL en su defecto.\n\n");
        printf("Para desactivar los mensajes de ayuda: usar el MACRO \"DESACTIVAR_AYUDA_AL_USUARIO\".\n");
        printf("***************************************************************************************************\n");
        printf("Escrito por: AGUSTIN BROCANI 2024\n\n");
        system("pause");
        system("cls");
    }

    crearArbol(&arbolDeOpciones);

    if(NO_PUDE_CARGAR_ARBOL == cargarMenuAArbolBinarioDeBusqueda(&arbolDeOpciones, textoMenu, cantidadDeFilas))
    {
        printf("NO PUDE CARGAR ARBOL - NO PUEDO CARGAR Menu\n");
    }else if(NULL == buscarPorClave(&arbolDeOpciones, &condicionDeSalida, comparaCadenasMenu))
        {
            system("cls");
            printf("No incluyo condicion de salida [0] en el texto del Menu\n");
        }else
            {
                if(ACTIVAR_AYUDA_AL_USUARIO == ayudaAlUsuario)
                {
                    printf("\n***************************************************************************************************\n");
                    printf("\tNO OLVIDE implementar todas las opciones cargadas en su switch\n");
                    printf("\tPODRIA TENER OPCIONES QUE NO HACEN NADA\n");
                    printf("\t-TENGA CRITERIO!!!\n");
                    printf("***************************************************************************************************\n");
                    printf("INICIAR MENU\n\n");
                    system("pause");
                }
//                mostrarArbol(&arbolDeOpciones, mostrarCadenaMenu);
                do
                {
                    opcion = ingresoDeOpcion(&arbolDeOpciones, textoMenu, cantidadDeFilas);
                    switchSegunCaso(opcion, parametroParaSwitchSegunCaso);
                    printf("\n\n");
                    if(SALIDA != opcion)
                        system("pause");
                }while(SALIDA != opcion);
                system("cls");
                printf("Saliendo...\n");
            }
    vaciarArbol(&arbolDeOpciones);
}
