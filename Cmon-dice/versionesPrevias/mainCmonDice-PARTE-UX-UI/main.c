#include <stdio.h>
#include <stdlib.h>
#include "./Biblioteca/include/listaSimple/listaSimple.h"
#include "./Biblioteca/include/menu/menu.h"
#include "./Biblioteca/include/generico.h"
#include <windows.h>

#define JUGAR 'A'
#define TAM_NyA 100
#define ES_LETRA(X) (((X) >= 'A' && (X) <= 'Z') || ((X) >= 'a' && (X) <= 'z'))
#define ES_BLANCO(X) (' ' == (X))
#define NOMBRE_INVALIDO 0
#define OK 1
#define NO_HAY_JUGADORES 0

#define TAM_NyA 100

#define FACIL 'F'
#define MEDIO 'M'
#define DIFICIL 'D'

#define CARACTERES_VALIDOS_A_INGRESAR_PARA_SECUENCIA "X-V-A-R-N"

typedef struct
{
    unsigned tiempoDeVisualizacionSecuenciaCorrecta;
    unsigned tiempoRespuestaPorRonda;
    unsigned cantidadDeVidas;
}tConfiguracion;

typedef struct
{
    unsigned id;///puede haber 2 jugadores con el mismo nombre y apellido
    char nya[TAM_NyA];
    unsigned cantidadDeVidas;
    t_lista rondasJugadas;
    t_lista secuenciaFinalRespondida;///COLA PARA GRABAR, PILA PARA USO DE VIDAS
}tJugador;

#define CANTIDAD_DE_NIVELES 3
typedef struct
{
    tConfiguracion configuraciones[CANTIDAD_DE_NIVELES];
    unsigned indiceDeNivelDeConfiguracionElegida;
    t_lista listaDeJugadores;
    unsigned cantidadDeJugadores;
}tRecursosMenu;

void mostrarJugador(const void* dato)
{
    printf("[ID: %u]%s\n",
           ((tJugador*)dato)->id,
           ((tJugador*)dato)->nya
           );
}

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

#define CARACTER_DE_SALIDA 'X'
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
        }else if(!validoIngresoDeNombre(jugador.nya))
            {
                printf("\nIngreso de nombre invalido, intente nuevamente.\n");
                system("pause");
            }
            else
            {
                (jugador.id)++;///SOLO SI ES UN NOMBRE VALIDO LE ASIGNO ID
                (*cantidadDeJugadores) = jugador.id;
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

#define INDICE_INVALIDO -1
#define INDICE_NIVEL_FACIL 0
#define INDICE_NIVEL_MEDIO 1
#define INDICE_NIVEL_DIFICIL 2

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
    }else if(MEDIO == caracter)
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
        printf("Ingrese nivel de dificiltad[FACIL(F) - MEDIO(M) - DIFICIL(D)]:\t");
        fflush(stdin);
        scanf("%c", &nivelDeDificultadIngresado);
        nivelDeDificultadIngresado = A_MAYUS(nivelDeDificultadIngresado);

        if(INDICE_INVALIDO == (indiceNivelDeDificultad = defineIndiceDeNivelSegunCaracter(nivelDeDificultadIngresado)))
        {
            printf("\nIngreso invalido, intente nuevamente.\n");
            system("pause");
        }
        system("cls");
    }while(INDICE_INVALIDO == indiceNivelDeDificultad);
    printf("Nivel de dificultad ingresado: %c\n", nivelDeDificultadIngresado);

    *indiceDeNivelDeConfiguracionElegida = indiceNivelDeDificultad;
}

#define TAM_BUFFER_CARGA_CONFIGURACIONES 11///cantidad maxima de caracteres posibles contando \0
#define ARCHIVO_TXT_DE_CONFIGURACION_CON_ERRORES 0
#define MIN_CANT_VIDAS 0
#define MAX_CANT_VIDAS 5
#define MIN_TIEMPO_JUEGO_POR_RONDA 0
#define MAX_TIEMPO_JUEGO_POR_RONDA 20

int cargarConfiguraciones(FILE* aConfiguracion, tConfiguracion* configuraciones)
{
    char buffer[TAM_BUFFER_CARGA_CONFIGURACIONES];
    unsigned indice;

    while(fgets(buffer, TAM_BUFFER_CARGA_CONFIGURACIONES, aConfiguracion))
    {
        if(INDICE_INVALIDO == (indice = defineIndiceDeNivelSegunCaracter(*buffer)))
        {
            fprintf(stderr, "Error de grabacion de archivo de configuraciones: LETRA INICIAL INCORRECTA.");
            return ARCHIVO_TXT_DE_CONFIGURACION_CON_ERRORES;
        }

        sscanf(buffer, "%*c|%u|%u|%u",
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
            fprintf(stderr, "Error de grabacion de archivo de configuraciones: VALORES FUERA DE RANGO.");
            return ARCHIVO_TXT_DE_CONFIGURACION_CON_ERRORES;
        }
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

#define USO_DE_VIDA 'X'
#define COLOR_VERDE 'V'
#define COLOR_AMARILLO 'A'
#define COLOR_ROJO 'R'
#define COLOR_NARANJA 'N'

void jugar(void* recursos)
{
    if(!ingresoDeNombresAListaSimple(&(((tRecursosMenu*)recursos)->listaDeJugadores), &(((tRecursosMenu*)recursos)->cantidadDeJugadores)))
    {
        printf("No se ingresaron jugadores.\n");
    }
    else
    {
        ingresoDeNivel(&(((tRecursosMenu*)recursos)->indiceDeNivelDeConfiguracionElegida));
        mezclarListaSimpleAleatoriamente(&(((tRecursosMenu*)recursos)->listaDeJugadores), ((tRecursosMenu*)recursos)->cantidadDeJugadores);

        system("cls");
        mostrarConfiguracionElegida(((tRecursosMenu*)recursos)->configuraciones,((tRecursosMenu*)recursos)->indiceDeNivelDeConfiguracionElegida);
        printf("\nCaracteres validos para ingreso en secuencias: %s\n", CARACTERES_VALIDOS_A_INGRESAR_PARA_SECUENCIA);
        printf("\tX: uso de vida.\n");
        printf("\tV: color verde.\n");
        printf("\tA: color amarillo.\n");
        printf("\tR: color rojo.\n");
        printf("\tN: color naranja.\n\n");
        system("pause");
        system("cls");
        printf("Lista de jugadores en el orden que deben jugar:\n");
        mostrarListaSimpleEnOrden(&(((tRecursosMenu*)recursos)->listaDeJugadores), mostrarJugador);
        system("pause");
        system("cls");
        printf("\n**************************ARRANCA EL JUEGO**************************\n");
        ///TODO:
            ///PASAR ESTO AL OTRO PROYECTO
        ///CODEAR LOGICA DE RONDAS
            //ANALIZAR USO DE ENUM
            //char letrasDeSecuencia[] = {COLOR_VERDE, COLOR_AMARILLO, COLOR_ROJO, COLOR_NARANJA};
            //CALCULAR PUNTOS POR JUGADOR POR RONDA
            //IR CALCULANDO EL TOTAL DE PUNTOS PARA NO VOLVER A RECORRER
            //COLA DE GANADORES[cuanto termina de jugar el primero, lo acolo, luego, comparo con el primero de la cola]-> solo guardar ID, NOMBRE Y PUNTOS que consiguio para ganr
        ///CODEAR FORMATO DEL INFORME
    }
    //RESETEO TODOS LOS PARAMETROS PARA VOLVER A JUGAR
    vaciarListaSimple(&(((tRecursosMenu*)recursos)->listaDeJugadores));
}

void switchTextoMenu(int opcion, void* recursosMenu)
{
    switch(opcion)
    {
        case JUGAR:
            jugar(recursosMenu);
        break;
    }
}

///TAM CONSOLA: seria conveniente que por un tema de UX/UI siempre se fije el tam de la consola para ver los textos con el formato seteado.
void obtenerTamanoConsola()
{
    // Obtener el handle de la consola
    HANDLE hConsola = GetStdHandle(STD_OUTPUT_HANDLE);

    // Estructura para obtener la información de la consola
    CONSOLE_SCREEN_BUFFER_INFO infoConsola;

    // Obtener información actual de la consola
    if(GetConsoleScreenBufferInfo(hConsola, &infoConsola))
    {
        // Tamaño del buffer de la consola
        COORD tamBuffer = infoConsola.dwSize;

        // Tamaño de la ventana de la consola
        SMALL_RECT ventana = infoConsola.srWindow;

        printf("Tamaño del buffer: %d x %d (ancho x alto)\n", tamBuffer.X, tamBuffer.Y);
        printf("Tamaño de la ventana: %d x %d (ancho x alto)\n", ventana.Right - ventana.Left + 1, ventana.Bottom - ventana.Top + 1);
    } else {
        printf("No se pudo obtener la información de la consola.\n");
    }
}

void ajustarTamConsola()
{
    // Obtener el handle de la consola
    HANDLE hConsola = GetStdHandle(STD_OUTPUT_HANDLE);

    // Definir el tamaño del buffer (120 columnas x 9001 filas)
    COORD tamBuffer;
    tamBuffer.X = 120;   // Ancho del buffer
    tamBuffer.Y = 9001;  // Alto del buffer

    // Establecer el tamaño del buffer
    SetConsoleScreenBufferSize(hConsola, tamBuffer);

    // Definir el tamaño de la ventana (120x30 caracteres)
    SMALL_RECT ventana;
    ventana.Left = 0;
    ventana.Top = 0;
    ventana.Right = 120 - 1;  // Ancho de la ventana
    ventana.Bottom = 30 - 1;  // Alto de la ventana

    // Ajustar el tamaño de la ventana de la consola
    SetConsoleWindowInfo(hConsola, TRUE, &ventana);
}

#define NOMBRE_ARCHIVO_TXT_CONFIGURACION "config.txt"
#define NO_PUDE_ABRIR_ARCHIVO_TXT_CONFIGURACION -3

int main()
{
    tRecursosMenu recursos;
    FILE* aConfiguracion;
    char textoMenuPrincipal[][MAX_TAM_TEXTO] =
    {
      "Cmon-dice:",
      "[A] Jugar.",
      "[B] Salida."
    };
    unsigned cantidadDeRegistros = sizeof(textoMenuPrincipal) / MAX_TAM_TEXTO;

    crearListaSimple(&recursos.listaDeJugadores);

    if(!abrirArchivo(&aConfiguracion, NOMBRE_ARCHIVO_TXT_CONFIGURACION, "rt"))
    {
        return NO_PUDE_ABRIR_ARCHIVO_TXT_CONFIGURACION;
    }
    if(!cargarConfiguraciones(aConfiguracion, recursos.configuraciones))
    {
        vaciarListaSimple(&recursos.listaDeJugadores);
        fclose(aConfiguracion);
        return ARCHIVO_TXT_DE_CONFIGURACION_CON_ERRORES;
    }
///*********************************TAM CONSOLA*********************************
    ajustarTamConsola();
//    obtenerTamanoConsola();
//    system("pause");
///*********************************TAM CONSOLA*********************************
    menu(textoMenuPrincipal, cantidadDeRegistros, switchTextoMenu, &recursos, DESACTIVAR_AYUDA_AL_USUARIO);

    vaciarListaSimple(&recursos.listaDeJugadores);
    fclose(aConfiguracion);

    return 0;
}
