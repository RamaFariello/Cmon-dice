#include <stdio.h>///***PASADO***
#include <stdlib.h>///***PASADO***
#include <windows.h>//AJUSTAR CONSOLA
#include <curl/curl.h>///***PASADO***
#include "./Biblioteca/include/listaSimple/listaSimple.h"///***PASADO***
#include "./Biblioteca/include/menu/menu.h"///***PASADO***
#include "./Biblioteca/include/generico.h"///***PASADO***


/**UX-UI JUEGO*/
#define JUGAR 'A'
#define TAM_NyA 100 ///***PASADO***
#define ES_LETRA(X) (((X) >= 'A' && (X) <= 'Z') || ((X) >= 'a' && (X) <= 'z'))
#define ES_BLANCO(X) (' ' == (X))
#define NOMBRE_INVALIDO 0
#define OK 1
#define NO_HAY_JUGADORES 0


#define FACIL 'F'
#define MEDIO 'M'
#define DIFICIL 'D'

#define CARACTERES_VALIDOS_A_INGRESAR_PARA_SECUENCIA "X-V-A-R-N"
/*FIN UX-UI*/

#define CANT_BYTES_MEMORIA_RESERVADA 20001

#define TAM_URL 1000

#define NO_PUDE_HACER_REALLOC 0///***PASADO***
#define NO_PUDE_RESERVAR_MEMORIA -32767 ///***PASADO***
#define ERROR_INICIAR_ESTRUCTURA_CURL -1///***PASADO***
#define ERROR_SOLICITUD_HTTPS -2///***PASADO***

#define CERTIFICADO_SITIO_SEGURO "random-org.pem"///***PASADO***
///CURL
#define RESPUESTA_SERVIDOR_OK_MIN 200///***PASADO***
#define RESPUESTA_SERVIDOR_OK_MAX 299///***PASADO***

#define RESPUESTA_SERVIDOR_ERROR_CLIENTE_MIN 400///***PASADO***
#define RESPUESTA_SERVIDOR_ERROR_CLIENTE_MAX 499///***PASADO***

#define RESPUESTA_SERVIDOR_ERROR_SERVIDOR_MIN 500///***PASADO***
#define RESPUESTA_SERVIDOR_ERROR_SERVIDOR_MAX 599///***PASADO***

#define RESPUESTA_SERVIDOR_INFORMATIVA_MIN 100///***PASADO***
#define RESPUESTA_SERVIDOR_INFORMATIVA_MAX 199///***PASADO***

#define RESPUESTA_SERVIDOR_REDIRECCION_MIN 300///***PASADO***
#define RESPUESTA_SERVIDOR_REDIRECCION_MAX 399///***PASADO***

typedef struct
{
    void* buffer;
    size_t cantBytesCopiados;
    size_t cantBytesReservados;
} tReconstruccionDato;///***PASADO***

typedef struct
{
    unsigned tiempoDeVisualizacionSecuenciaCorrecta;
    unsigned tiempoRespuestaPorRonda;
    unsigned cantidadDeVidas;
} tConfiguracion;///***PASADO***

typedef struct
{
    unsigned id;///puede haber 2 jugadores con el mismo nombre y apellido
    char nya[TAM_NyA];
    unsigned cantidadDeVidas;
    t_lista rondasJugadas;
    t_lista secuenciaFinalRespondida;///COLA PARA GRABAR, PILA PARA USO DE VIDAS
} tJugador;///***PASADO***

#define CANTIDAD_DE_NIVELES 3
typedef struct
{
    tConfiguracion configuraciones[CANTIDAD_DE_NIVELES];
    unsigned indiceDeNivelDeConfiguracionElegida;
    t_lista listaDeJugadores;
    unsigned cantidadDeJugadores;
    tReconstruccionDato datoRespuestaAPI;
    char* cadenaConIndices;
} tRecursos;///***PASADO***

///***********************************************************CURL***********************************************************/
size_t datosObtenidosDeRespuestaURL(const char* bufferParaDatosRecibidos, size_t itemSize, size_t nItems, void* sDato)
{
    size_t cantidadDeBytesRecibidos = itemSize * nItems;
    void* returnRealloc;
    //+1 para el \0
    while(
        (cantidadDeBytesRecibidos + 1) > ((tReconstruccionDato*)sDato)->cantBytesReservados - ((tReconstruccionDato*)sDato)->cantBytesCopiados
    )
    {
        (((tReconstruccionDato*)sDato)->cantBytesReservados) += CANT_BYTES_MEMORIA_RESERVADA;
        returnRealloc = realloc(((tReconstruccionDato*)sDato)->buffer, ((tReconstruccionDato*)sDato)->cantBytesReservados);
        if(NULL == returnRealloc)
        {
            fprintf(stderr, "No pude hacer realloc.\n");
            return NO_PUDE_HACER_REALLOC;
        }
        ((tReconstruccionDato*)sDato)->buffer = returnRealloc;
    }

    memcpy(((tReconstruccionDato*)sDato)->buffer + ((tReconstruccionDato*)sDato)->cantBytesCopiados, bufferParaDatosRecibidos, cantidadDeBytesRecibidos);
    (((tReconstruccionDato*)sDato)->cantBytesCopiados) += cantidadDeBytesRecibidos;
    ((char*)(((tReconstruccionDato*)sDato)->buffer))[((tReconstruccionDato*)sDato)->cantBytesCopiados] = '\0';

    return cantidadDeBytesRecibidos;
}

int inicializacionEstructuraCURL(CURL** curl)
{
    *curl = curl_easy_init();
    if(NULL == *curl)
    {
        fprintf(stderr, "No pude inicializar estructura CURL.\n");
        return ERROR_INICIAR_ESTRUCTURA_CURL;
    }

    return OK;
}

void configuracionEstructuraCURL(CURL* curl, const char* URL, void* dato)
{
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, CERTIFICADO_SITIO_SEGURO);
    curl_easy_setopt(curl, CURLOPT_URL, URL);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, dato);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, datosObtenidosDeRespuestaURL);
}

int ejecutarSolicitudHTTPS(CURL* curl)
{
    CURLcode resultadoDeSolicitudHTTPS;
    long codigoDeRepuestaHTTPS = 0;

    resultadoDeSolicitudHTTPS = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &codigoDeRepuestaHTTPS);
    if(CURLE_OK != resultadoDeSolicitudHTTPS)
    {
        fprintf(stderr, "Fallo en la solicitud HTTPS: %s.\n", curl_easy_strerror(resultadoDeSolicitudHTTPS));
    }

    return codigoDeRepuestaHTTPS;
}

/*****************************/

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
            insertarAlFinalEnListaSimple(listaDeJugadores, &jugador, sizeof(tJugador));
        }
        system("cls");
    }
    while(ingresoNombres);

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
    }
    while(INDICE_INVALIDO == indiceNivelDeDificultad);
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
            fprintf(stderr, "Error de grabacion de archivo de configuraciones: LETRA INICIAL INCORRECTA.\n");
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
            fprintf(stderr, "Error de grabacion de archivo de configuraciones: VALORES FUERA DE RANGO.\n");
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

#define CANT_RONDAS_PROMEDIO_JUGADAS 15
#define ERROR_BUFFER_RECONSTRUCCION_DATO 0

#define URL_BASE "https://www.random.org/integers/"
#define QUERY_PARAMS_FORMATO "?min=0&max=3&col=1&base=10&format=plain&rnd=new"
#define QUERY_PARAM_CANTIDAD_DE_ELEMENTOS "&num="

void construccionURL(char* URL, unsigned tam, unsigned ce)//doy la flexibilidad de usar cantidad de elementos o no
{
    snprintf(URL, tam, "%s%s%s%u", URL_BASE, QUERY_PARAMS_FORMATO, QUERY_PARAM_CANTIDAD_DE_ELEMENTOS, ce * CANT_RONDAS_PROMEDIO_JUGADAS);
}

int consumoAPI(tReconstruccionDato* dato, unsigned cantidadDeJugadores, void (*construccionURL)(char* URL, unsigned tam, unsigned ce))
{
    CURL* curl;
    char URL[TAM_URL];
    int retornoCodigoDeError;

    if(NULL == dato->buffer)
    {
        fprintf(stderr, "No hay buffer para almacenar respuesta de API.\n");
        return ERROR_BUFFER_RECONSTRUCCION_DATO;
    }

    construccionURL(URL, sizeof(URL),cantidadDeJugadores);

    if(ERROR_INICIAR_ESTRUCTURA_CURL == inicializacionEstructuraCURL(&curl))
    {
        return ERROR_INICIAR_ESTRUCTURA_CURL;
    }

    *((char*)(dato->buffer)) = '\0';
    dato->cantBytesReservados = CANT_BYTES_MEMORIA_RESERVADA;
    dato->cantBytesCopiados = 0;

    configuracionEstructuraCURL(curl, URL, dato);
    retornoCodigoDeError = ejecutarSolicitudHTTPS(curl);
    if(!(retornoCodigoDeError >= RESPUESTA_SERVIDOR_OK_MIN && retornoCodigoDeError <= RESPUESTA_SERVIDOR_OK_MAX))
    {
        fprintf(stderr, "Codigo de error: %d. %s.\n", retornoCodigoDeError,
                retornoCodigoDeError >= RESPUESTA_SERVIDOR_ERROR_CLIENTE_MIN && retornoCodigoDeError <= RESPUESTA_SERVIDOR_ERROR_CLIENTE_MAX ? "Error del Cliente" :
                retornoCodigoDeError >= RESPUESTA_SERVIDOR_ERROR_SERVIDOR_MIN && retornoCodigoDeError <= RESPUESTA_SERVIDOR_ERROR_SERVIDOR_MAX ? "Error del Servidor" :
                retornoCodigoDeError >= RESPUESTA_SERVIDOR_INFORMATIVA_MIN && retornoCodigoDeError <= RESPUESTA_SERVIDOR_INFORMATIVA_MAX ? "Mensaje informativo del Servidor" :
                retornoCodigoDeError >= RESPUESTA_SERVIDOR_REDIRECCION_MIN && retornoCodigoDeError <= RESPUESTA_SERVIDOR_REDIRECCION_MAX ? "Redireccion" :
                "Ocurrio un error en el codigo de CURL"
                );

        curl_easy_cleanup(curl);
        return retornoCodigoDeError;
    }

    fprintf(stdout, "%s\n",(char*)(dato->buffer));

    curl_easy_cleanup(curl);

    return OK;
}

void imprimirResultados(FILE* pf, tRecursos* recursos)
{
    fprintf(pf, "\nIMPLEMENTAR FORMATO DE INFORME EN LA FUNCION imprimirResultado\n"
            );
}

void construccionNombreArchivoTxtInforme(char* NOMBRE_ARCHIVO_TXT_INFORME, unsigned tam, struct tm* fechaYHora)
{
        snprintf(NOMBRE_ARCHIVO_TXT_INFORME, tam,
            "informe-juego_%4d-%02d-%02d-%02d-%02d.txt",
            fechaYHora->tm_year + 1900,
            fechaYHora->tm_mon + 1,
            fechaYHora->tm_mday,
            fechaYHora->tm_hour,
            fechaYHora->tm_min
        );
}

#define TAM_NOMBRE_ARCHIVO_INFORME 100
#define NO_PUDE_ABRIR_ARCHIVO_TXT_INFORME 0

int generarInforme(tRecursos* recursos, void (*construccionNombreArchivoTxtInforme)(char* NOMBRE_ARCHIVO_TXT_INFORME, unsigned tam, struct tm* fechaYHora))
{
    FILE* aInforme;
    char NOMBRE_ARCHIVO_TXT_INFORME[TAM_NOMBRE_ARCHIVO_INFORME];
    time_t tiempoTranscurrido;
    struct tm* fechaYHora;

    imprimirResultados(stdout, recursos);

    tiempoTranscurrido = time(NULL);
    fechaYHora = localtime(&tiempoTranscurrido);

    construccionNombreArchivoTxtInforme(NOMBRE_ARCHIVO_TXT_INFORME, sizeof(NOMBRE_ARCHIVO_TXT_INFORME), fechaYHora);

    if(!abrirArchivo(&aInforme, NOMBRE_ARCHIVO_TXT_INFORME, "wt"))
    {
        return NO_PUDE_ABRIR_ARCHIVO_TXT_INFORME;
    }

    imprimirResultados(aInforme, recursos);

    fclose(aInforme);

    return OK;
}

#define USO_DE_VIDA 'X'
#define COLOR_VERDE 'V'
#define COLOR_AMARILLO 'A'
#define COLOR_ROJO 'R'
#define COLOR_NARANJA 'N'
#define A_NUMERO(X) ((X) - '0')

#define RESPUESTA_SERVIDOR_ERROR_FORMATO 422

char obtenerCaracterDeSecuencia(const char* cadenaConIndices, const char* caracteresDeSecuencia)
{
    return '\0' == *cadenaConIndices ? CARACTER_DE_SALIDA : caracteresDeSecuencia[A_NUMERO(*cadenaConIndices)];
}

int validaFormatoRespuestaAPI(const char* respuesta)
{
    if(0 != strlen(respuesta) % 2)//SIEMPRE ES 1 CARACTER y 1 \n => la cantidad recibida es PAR(NO INCLUYO EL \0 en la validacion, se asume que esta-> lo pone mi funcion de write callback)
    {
        fprintf(stderr, "Respuesta de API en formato erroneo.\n");
        return RESPUESTA_SERVIDOR_ERROR_FORMATO;
    }
    return OK;
}

int generarRondas(tRecursos* recursos)
{
    char caracteresDeSecuencia[] = {COLOR_VERDE, COLOR_AMARILLO, COLOR_ROJO, COLOR_NARANJA};
    int retornoCodigoError;

    if(
        OK != (retornoCodigoError = consumoAPI(&(recursos->datoRespuestaAPI), recursos->cantidadDeJugadores, construccionURL)) ||
        OK != (retornoCodigoError = validaFormatoRespuestaAPI((recursos->datoRespuestaAPI).buffer))
    )
    {
        fprintf(stderr, "No pude generar rondas.\n");
        return retornoCodigoError;
    }

    recursos->cadenaConIndices = (recursos->datoRespuestaAPI).buffer;
    while('\0' != *(recursos->cadenaConIndices))
    {
        printf("LETRA OBTENIDA: %c\n", obtenerCaracterDeSecuencia(recursos->cadenaConIndices, caracteresDeSecuencia));
        (recursos->cadenaConIndices) += 2;///porque 2?, 1 por el caracter que ya utilice y 1 por el \n
    }

    return OK;
}

int iniciarJuego(tRecursos* recursos)
{
    int retornoCodigoError;

    if(OK != (retornoCodigoError = generarRondas(recursos)))
    {
        return retornoCodigoError;
    }

    return OK;
}

int jugar(tRecursos* recursos)
{
    int retornoCodigoError;

    (recursos->datoRespuestaAPI).buffer = NULL;//si no hago esto, va a romper al final porque hice free de basura

    if(!ingresoDeNombresAListaSimple(&(recursos->listaDeJugadores), &(recursos->cantidadDeJugadores)))
    {
        printf("No se ingresaron jugadores.\n");
    }
    else
    {
        ingresoDeNivel(&(recursos->indiceDeNivelDeConfiguracionElegida));
        mezclarListaSimpleAleatoriamente(&(recursos->listaDeJugadores), recursos->cantidadDeJugadores);

        system("cls");
        mostrarConfiguracionElegida(recursos->configuraciones, recursos->indiceDeNivelDeConfiguracionElegida);
        printf("\nCaracteres validos para ingreso en secuencias: %s.\n", CARACTERES_VALIDOS_A_INGRESAR_PARA_SECUENCIA);
        printf("\tX: uso de vida.\n");
        printf("\tV: color verde.\n");
        printf("\tA: color amarillo.\n");
        printf("\tR: color rojo.\n");
        printf("\tN: color naranja.\n\n");
        system("pause");
        system("cls");
        printf("Lista de jugadores en el orden que deben jugar:\n");
        mostrarListaSimpleEnOrden(&(recursos->listaDeJugadores), mostrarJugador);
        system("pause");
        system("cls");

        printf("\n**************************ARRANCA EL JUEGO**************************\n");

        (recursos->datoRespuestaAPI).buffer = malloc(CANT_BYTES_MEMORIA_RESERVADA);
        if(NULL == (recursos->datoRespuestaAPI).buffer)
        {
            fprintf(stderr, "No pude reservar memoria para buffer de respuesta de API.\n");
            vaciarListaSimple(&(recursos->listaDeJugadores));
            return NO_PUDE_RESERVAR_MEMORIA;
        }

        if(OK != (retornoCodigoError = iniciarJuego(recursos)))
        {
            fprintf(stderr, "No pude iniciar el juego.\n");
            vaciarListaSimple(&(recursos->listaDeJugadores));
            free((recursos->datoRespuestaAPI).buffer);
            return retornoCodigoError;
        }

        ///TODO:
        ///PASAR ESTO AL OTRO PROYECTO
        ///CODEAR LOGICA DE RONDAS
        //ANALIZAR USO DE ENUM
        //char letrasDeSecuencia[] = {COLOR_VERDE, COLOR_AMARILLO, COLOR_ROJO, COLOR_NARANJA};
        //CALCULAR PUNTOS POR JUGADOR POR RONDA
        //IR CALCULANDO EL TOTAL DE PUNTOS PARA NO VOLVER A RECORRER
        //COLA DE GANADORES[cuanto termina de jugar el primero, lo acolo, luego, comparo con el primero de la cola]-> solo guardar ID, NOMBRE Y PUNTOS que consiguio para ganr
        ///CODEAR FORMATO DEL INFORME

        if(OK != (retornoCodigoError = generarInforme(recursos, construccionNombreArchivoTxtInforme)))
        {
            fprintf(stderr, "No pude grabar archivo de informe.\n");
            return retornoCodigoError;
        }
    }
    //RESETEO TODOS LOS PARAMETROS PARA VOLVER A JUGAR
    vaciarListaSimple(&(recursos->listaDeJugadores));
    free(((recursos)->datoRespuestaAPI).buffer);
    return OK;
}

void switchTextoMenu(int opcion, void* recursos)
{
    switch(opcion)
    {
        case JUGAR:
            jugar(recursos);
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
    }
    else
    {
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
    tRecursos recursos;
    FILE* aConfiguracion;
    char textoMenuPrincipal[][MAX_TAM_TEXTO] =
    {
        "Cmon-dice:",
        "[A] Jugar.",
        "[B] Salida."
    };
    unsigned cantidadDeRegistros = sizeof(textoMenuPrincipal) / MAX_TAM_TEXTO;

///*********************************TAM CONSOLA*********************************
    ajustarTamConsola();
//    obtenerTamanoConsola();
//    system("pause");
///*********************************TAM CONSOLA*********************************

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

    menu(textoMenuPrincipal, cantidadDeRegistros, switchTextoMenu, &recursos, DESACTIVAR_AYUDA_AL_USUARIO);

    vaciarListaSimple(&recursos.listaDeJugadores);
    fclose(aConfiguracion);

    return 0;
}
