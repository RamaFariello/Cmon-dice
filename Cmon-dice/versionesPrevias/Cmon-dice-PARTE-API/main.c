#include "funciones.h"

int main()
{
    CURL* curl;
    tReconstruccionDato dato;
    char URL[TAM_URL];
    unsigned cantidadDeNumeros = 1000;///12 JUGADORES 10 A 15  RONDAS PROMEDIO -> 10 * 15
//    sprintf(URL, "https://www.random.org/integers/?min=0&max=3&col=1&base=10&format=plain&rnd=new&num=%u", cantidadDeNumeros);
    sprintf(URL, "https://www.random.org/integers/?min=999999999999&max=999999999999&col=1&base=10&format=plain&rnd=new&num=%u", cantidadDeNumeros);

    ///INGRESO DE NOMBRES Y COSAS POR EL ESTILO

    dato.buffer = malloc(CANT_BYTES_MEMORIA_RESERVADA);
    if(NULL == dato.buffer)
    {
        fprintf(stderr,"No pude reservar memoria.");
        return NO_PUDE_RESERVAR_MEMORIA;
    }

    if(ERROR_INICIAR_ESTRUCTURA_CURL == inicializacionEstructuraCURL(&curl))
    {
        free(dato.buffer);
        return ERROR_INICIAR_ESTRUCTURA_CURL;
    }

    *((char*)(dato.buffer)) = '\0';
    dato.cantBytesReservados = CANT_BYTES_MEMORIA_RESERVADA;
    dato.cantBytesCopiados = 0;

    configuracionEstructuraCURL(curl, URL, &dato);
    if(ERROR_SOLICITUD_HTTPS == ejecutarSolicitudHTTPS(curl))
    {
        curl_easy_cleanup(curl);
        free(dato.buffer);
        return ERROR_SOLICITUD_HTTPS;
    }

    fprintf(stdout, "%s",(char*)(dato.buffer));

    curl_easy_cleanup(curl);
    free(dato.buffer);
    return 0;
}
