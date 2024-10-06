#include "funciones.h"

size_t datosObtenidosDeRespuestaURL(const char* bufferParaDatosRecibidos, size_t itemSize, size_t nItems, void* sDato)
{
    size_t cantidadDeBytesRecibidos = itemSize * nItems;
    void* returnRealloc;
    //+1 para el \0
    while(
       (cantidadDeBytesRecibidos + 1) > ((tReconstruccionDato*)sDato)->cantBytesReservados - ((tReconstruccionDato*)sDato)->cantBytesCopiados
       )
    {
        system("pause");
        (((tReconstruccionDato*)sDato)->cantBytesReservados) += CANT_BYTES_MEMORIA_RESERVADA;
        returnRealloc = realloc(((tReconstruccionDato*)sDato)->buffer, ((tReconstruccionDato*)sDato)->cantBytesReservados);
        if(NULL == returnRealloc)
        {
            fprintf(stderr, "No pude hacer realloc.");
            return NO_PUDE_HACER_REALLOC;
        }
        ((tReconstruccionDato*)sDato)->buffer = returnRealloc;
    }

    memcpy(((tReconstruccionDato*)sDato)->buffer + ((tReconstruccionDato*)sDato)->cantBytesCopiados , bufferParaDatosRecibidos, cantidadDeBytesRecibidos);
    (((tReconstruccionDato*)sDato)->cantBytesCopiados) += cantidadDeBytesRecibidos;
    ((char*)(((tReconstruccionDato*)sDato)->buffer))[((tReconstruccionDato*)sDato)->cantBytesCopiados] = '\0';

    return cantidadDeBytesRecibidos;
}

int inicializacionEstructuraCURL(CURL** curl)
{
    *curl = curl_easy_init();
    if(NULL == *curl)
    {
        fprintf(stderr, "Fallo init.\n");
        return ERROR_INICIAR_ESTRUCTURA_CURL;
    }

    return OK;
}

void configuracionEstructuraCURL(CURL* curl, const char* URL,void* dato)
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

    if(CURLE_OK != (resultadoDeSolicitudHTTPS = curl_easy_perform(curl)))
    {
        fprintf(stderr, "Fallo la solicitud HTTPS: %s.\n", curl_easy_strerror(resultadoDeSolicitudHTTPS));

        return ERROR_SOLICITUD_HTTPS;
    }
    return OK;
}
