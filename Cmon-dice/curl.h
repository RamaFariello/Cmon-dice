#ifndef CURL_H_INCLUDED
#define CURL_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

///MEMORIA
#define NO_PUDE_HACER_REALLOC -32767
#define NO_PUDE_RESERVAR_MEMORIA -32768

///CURL
#define ERROR_INICIAR_ESTRUCTURA_CURL 99

//RESPONSE DEL SERVER
#define RESPUESTA_SERVIDOR_OK_MIN 200
#define RESPUESTA_SERVIDOR_OK_MAX 299

#define RESPUESTA_SERVIDOR_ERROR_CLIENTE_MIN 400
#define RESPUESTA_SERVIDOR_ERROR_CLIENTE_MAX 499

#define RESPUESTA_SERVIDOR_ERROR_SERVIDOR_MIN 500
#define RESPUESTA_SERVIDOR_ERROR_SERVIDOR_MAX 599

#define RESPUESTA_SERVIDOR_INFORMATIVA_MIN 100
#define RESPUESTA_SERVIDOR_INFORMATIVA_MAX 199

#define RESPUESTA_SERVIDOR_REDIRECCION_MIN 300
#define RESPUESTA_SERVIDOR_REDIRECCION_MAX 399

#define RESPUESTA_SERVIDOR_CON_ERROR_DE_FORMATO 422 //Unprocessable Entity: error de formato, lo recibi mal o me lo enviaron mal

#define CERTIFICADO_SITIO_SEGURO "random-org.pem" //archivo de certificado .pem

#define OK 1

typedef struct
{
    char* buffer;
    size_t cantBytesCopiados;
    size_t cantBytesReservados;
    size_t cantBytesFijosAReservar;
} tReconstruccionDato;

size_t datosObtenidosDeRespuestaURL(const char* bufferParaDatosRecibidos, size_t itemSize, size_t nItems, void* sDato);

int inicializacionEstructuraCURL(CURL** curl);
void configuracionEstructuraCURL(CURL* curl, const char* URL, void* dato);
int ejecutarSolicitudHTTPS(CURL* curl);

#endif // CURL_H_INCLUDED
