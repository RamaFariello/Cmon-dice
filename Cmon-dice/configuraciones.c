#include "funciones.h"

int validoIngresoDeNombre(const char* cadena)
{
    if(!ES_LETRA(*cadena))
    {
        return NOMBRE_INVALIDO;
    }

    while('\0' != *cadena)
    {
        if((!ES_LETRA(*cadena) && !ES_BLANCO(*cadena)) ||
                ((ES_LETRA(*cadena) && '\0' != *(cadena + 1) && ES_BLANCO(*(cadena + 1)) && '\0' != *(cadena + 2) && !ES_LETRA(*(cadena + 2)))))
        {
            return NOMBRE_INVALIDO;
        }

        cadena++;
    }

    if(ES_BLANCO(*(cadena - 1)))
    {
        return NOMBRE_INVALIDO;
    }

    return OK;
}

int ingresoDeNombresAListaSimple(t_lista* listaDeJugadores, unsigned* cantidadDeJugadores)
{
    tJugador jugador;
    unsigned ingresoNombres = 1;
    char* retorno;

    jugador.id = 0;
    *cantidadDeJugadores = 0;

    printf("\t\tINGRESO DE NOMBRES DE LOS JUGADORES\n");
    printf(" _______________________________________________________________________\n");
    printf("|Reglas de formato de ingreso:                                          |\n");
    printf("|-Solo puede hacer ingreso de caracteres LETRA o ESPACIO para nombre.   |\n");
    printf("|-Solo puede utilizar un ESPACIO entre dos caracteres LETRA para nombre.|\n");
    printf("|-Al finalizar ingreso de nombre, presione ENTER.                       |\n");
    printf("|FINALIZAR INGRESO DE NOMBRES: ingrese 'X' y presione ENTER.            |\n");
    printf("|_______________________________________________________________________|\n\n");
    system("pause");
    system("cls");

    do
    {
        printf("[FINALIZAR INGRESO DE NOMBRE DE LOS JUGADORES: ingrese 'X' y presione ENTER]\n");
        printf("\nIngrese Nombre de Jugador con ID = %d:\t", jugador.id + 1);
        fflush(stdin);
        fgets(jugador.nya, TAM_NyA, stdin);

        if(NULL != (retorno = strrchr(jugador.nya, '\n')))
        {
            *retorno = '\0';
        }
        *(jugador.nya) = ES_LETRA(*(jugador.nya)) ? A_MAYUS(*(jugador.nya)) : *(jugador.nya);

        if(CARACTER_DE_SALIDA == *(jugador.nya) && '\0' == *(jugador.nya + 1))
        {
            ingresoNombres = 0;
        }
        else if(!validoIngresoDeNombre(jugador.nya))
        {
            printf("\nIngreso de nombre invalido, intente nuevamente.\n");
            system("pause");
        }
        else
        {
            (jugador.id)++;///SOLO SI ES UN NOMBRE VALIDO LE ASIGNO ID
            (*cantidadDeJugadores) = jugador.id;
            jugador.puntosTotales = 0;

            ///ACA VAN TODAS LAS INICIALIZACIONES NECESARIAS PARA LOS JUGADORES:
            crearListaSimple(&jugador.rondasJugadas);
            crearListaSimple(&jugador.secuenciaAsignada);

            insertarAlFinalEnListaSimple(listaDeJugadores, &jugador, sizeof(tJugador));
        }
        system("cls");
    }while(ingresoNombres);

    if(!*cantidadDeJugadores)
    {
        return NO_HAY_JUGADORES;
    }

    return OK;
}

void mostrarConfiguracionElegida(tConfiguracion* configuracion, unsigned indiceDeNivelDeConfiguracionElegida)
{
    printf("Configuracion seleccionada:\n");
    printf("\tNivel de dificultad: %s.\n",
           INDICE_NIVEL_FACIL == indiceDeNivelDeConfiguracionElegida ? "FACIL" : INDICE_NIVEL_MEDIO == indiceDeNivelDeConfiguracionElegida ? "MEDIO" : "DIFICIL"
          );
    printf("\tTiempo de visualizacion de secuencia: %u.\n", configuracion[indiceDeNivelDeConfiguracionElegida].tiempoDeVisualizacionSecuenciaCorrecta);
    printf("\tTiempo de respuesta: %u.\n", configuracion[indiceDeNivelDeConfiguracionElegida].tiempoRespuestaPorRonda);
    printf("\tCantidad de vidas: %u.\n", configuracion[indiceDeNivelDeConfiguracionElegida].cantidadDeVidas);
}

int indicesDeNivelesValidos(const int* nivelesCargados)
{
    int i;

    for(i = 0; i < CANTIDAD_DE_NIVELES; i++)
    {
        if(INDICE_INVALIDO == *nivelesCargados)
        {
            fprintf(stderr, "No cargo los 3 niveles requeridos.\n");
            return INDICE_INVALIDO;
        }
        nivelesCargados++;
    }
    return OK;
}

int cargarConfiguraciones(FILE* aConfiguracion, tConfiguracion* configuraciones, int* nivelesCargados)
{
    char buffer[TAM_BUFFER_CARGA_CONFIGURACIONES];
    unsigned indice;

    while(fgets(buffer, TAM_BUFFER_CARGA_CONFIGURACIONES, aConfiguracion))
    {
        if(INDICE_INVALIDO == (indice = defineIndiceDeNivelSegunCaracter(*buffer)))
        {
            fprintf(stderr, "Error de grabacion de archivo de configuraciones: LETRA INICIAL INCORRECTA.\n");
            return ARCHIVO_TXT_DE_CONFIGURACION_CON_ERRORES;
        }

        sscanf(buffer, "%*c|%d|%d|%d",
               &(configuraciones[indice].tiempoDeVisualizacionSecuenciaCorrecta),
               &(configuraciones[indice].tiempoRespuestaPorRonda),
               &(configuraciones[indice].cantidadDeVidas)
              );//%*c ignora la primer letra
        if(
            !(configuraciones[indice].tiempoDeVisualizacionSecuenciaCorrecta > MIN_TIEMPO_JUEGO_POR_RONDA && configuraciones[indice].tiempoDeVisualizacionSecuenciaCorrecta <= MAX_TIEMPO_JUEGO_POR_RONDA) ||
            !(configuraciones[indice].tiempoRespuestaPorRonda > MIN_TIEMPO_JUEGO_POR_RONDA && configuraciones[indice].tiempoRespuestaPorRonda <= MAX_TIEMPO_JUEGO_POR_RONDA) ||
            !(configuraciones[indice].cantidadDeVidas >= MIN_CANT_VIDAS && configuraciones[indice].cantidadDeVidas <= MAX_CANT_VIDAS)
        )
        {
            fprintf(stderr, "Error de grabacion de archivo de configuraciones: VALORES FUERA DE RANGO.\n");
            return ARCHIVO_TXT_DE_CONFIGURACION_CON_ERRORES;
        }
        nivelesCargados[indice] = indice;
    }
    return OK;
}

int validaArchivoDeConfiguraciones(FILE* aConfiguracion, tRecursos* recursos)
{
    int nivelesCargados[] = {INDICE_INVALIDO, INDICE_INVALIDO, INDICE_INVALIDO};

    if(ARCHIVO_TXT_DE_CONFIGURACION_CON_ERRORES == cargarConfiguraciones(aConfiguracion, recursos->configuraciones, nivelesCargados) || INDICE_INVALIDO == indicesDeNivelesValidos(nivelesCargados))
    {
        return ARCHIVO_TXT_DE_CONFIGURACION_CON_ERRORES;
    }

    return OK;
}

int defineIndiceDeNivelSegunCaracter(char caracter)
{
    if(!ES_LETRA(caracter))
    {
        return INDICE_INVALIDO;
    }

    caracter = A_MAYUS(caracter);
    if(FACIL == caracter)
    {
        return INDICE_NIVEL_FACIL;
    }
    else if(MEDIO == caracter)
        {
            return INDICE_NIVEL_MEDIO;
        }
        else if(DIFICIL == caracter)
            {
                return INDICE_NIVEL_DIFICIL;
            }

    return INDICE_INVALIDO;
}

void ingresoDeNivel(unsigned* indiceDeNivelDeConfiguracionElegida)//aca no soy tan estricto con lo que ingresa porque me interesa que elija uno
{
    int indiceNivelDeDificultad;
    char nivelDeDificultadIngresado;

    do
    {
        printf("Ingrese nivel de dificultad[FACIL(F) - MEDIO(M) - DIFICIL(D)]:\t");
        fflush(stdin);
        scanf("%c", &nivelDeDificultadIngresado);
        nivelDeDificultadIngresado = A_MAYUS(nivelDeDificultadIngresado);

        if(INDICE_INVALIDO == (indiceNivelDeDificultad = defineIndiceDeNivelSegunCaracter(nivelDeDificultadIngresado)))
        {
            printf("\nIngreso invalido, intente nuevamente.\n");
            system("pause");
        }
        system("cls");
    }
    while(INDICE_INVALIDO == indiceNivelDeDificultad);
    printf("Nivel de dificultad ingresado: %c\n", nivelDeDificultadIngresado);

    *indiceDeNivelDeConfiguracionElegida = indiceNivelDeDificultad;
}
