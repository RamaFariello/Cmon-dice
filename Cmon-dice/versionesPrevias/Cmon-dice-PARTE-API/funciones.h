#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "./Biblioteca/include/listaSimple/listaSimple.h"
#include "./Biblioteca/include/menu/menu.h"
#include "./Biblioteca/include/generico.h"

///CANT_BYTES_MEMORIA_RESERVADA = num * 2 + 1 para incluir el \n y el \0 [seguridad del \0 y eficiencia de no andar haciendo realloc]
#define CANT_BYTES_MEMORIA_RESERVADA 20001
//#define CANT_BYTES_MEMORIA_RESERVADA 21

#define TAM_URL 1000

#define NO_PUDE_HACER_REALLOC 0
#define NO_PUDE_RESERVAR_MEMORIA 1
#define ERROR_INICIAR_ESTRUCTURA_CURL -1
#define ERROR_SOLICITUD_HTTPS -2
#define OK 1

///Esta API manda los numeros con \n y le pone el \0 al final
///RESPUESTAS DE LA API -> 1\n2\n3\n0\n1\n3\n2\n
//#define URL "https://www.random.org/integers/?num=10000&min=0&max=3&col=1&base=10&format=plain&rnd=new" ///RESPUESTAS DE LA API -> 1\n2\n3\n0\n1\n3\n2\n
#define CERTIFICADO_SITIO_SEGURO "random-org.pem"
///CURL
typedef struct
{
    void* buffer;
    size_t cantBytesCopiados;
    size_t cantBytesReservados;
}tReconstruccionDato;

#define TAM_NyA 100
typedef struct
{
    int id;///puede haber 2 jugadores con el mismo nombre y apellido
    char nya[TAM_NyA];
    unsigned cantidadDeVidas;
    t_lista rondasJugadas;
    t_lista secuenciaFinalRespondida;///COLA PARA GRABAR, PILA PARA USO DE VIDAS
}tJugador;

typedef struct
{
    t_lista secuenciaCorrectaPorRonda;
    unsigned cantidadDeVidasUsadas;
    unsigned cantidadDePuntosPorRonda;
}tRonda;

#define TAM_NIVEL_DE_DIFICULTAD 8///LA PALABRA MÁS LARGA ES DIFICIL + \0
typedef struct
{
    char nivelDeDificultad[TAM_NIVEL_DE_DIFICULTAD];
    unsigned maximoTiempoDeVisualizacionSecuenciaCorrecta;
    unsigned maximoTiempoRespuestaPorRonda;
    unsigned cantidadMaximaDeVidas;
}tConfiguracion;

///CURL
size_t datosObtenidosDeRespuestaURL(const char* bufferParaDatosRecibidos, size_t itemSize, size_t nItems, void* sDato);
int inicializacionEstructuraCURL(CURL** curl);
void configuracionEstructuraCURL(CURL* curl, const char* URL,void* dato);
int ejecutarSolicitudHTTPS(CURL* curl);

#endif // FUNCIONES_H_INCLUDED
