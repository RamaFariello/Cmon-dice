#include "funciones.h"

void mostrarJugador(const void* dato)
{
    printf("[ID: %u]%s\n",
           ((tJugador*)dato)->id,
           ((tJugador*)dato)->nya
          );
}

void mostrarCaracteresValidos()
{
    printf("\nCaracteres validos para ingreso en secuencias: %s.\n", CARACTERES_VALIDOS_A_INGRESAR_PARA_SECUENCIA);
    printf("\tX: uso de vida.\n");
    printf("\tV: color verde.\n");
    printf("\tA: color amarillo.\n");
    printf("\tR: color rojo.\n");
    printf("\tN: color naranja.\n\n");
}

void liberarListaDeSecuenciasIngresadasPorRonda(void* vRecursos, void* vRonda, int* retornoCodigoDeError)
{
    tRonda* ronda = (tRonda*)vRonda;

    vaciarListaSimple(&(ronda->secuenciaIngresada));
}

void liberarListasDeCadaJugador(void* vRecursos, void* vJugador, int* retornoCodigoDeError)
{
    tJugador* jugador = (tJugador*)vJugador;

    mapEnListaSimple(&(jugador->rondasJugadas), NULL, NULL, liberarListaDeSecuenciasIngresadasPorRonda);
    vaciarListaSimple(&(jugador->rondasJugadas));
    vaciarListaSimple(&(jugador->secuenciaAsignada));
}

int jugar(tRecursos* recursos)
{
    int retornoCodigoDeError;

    crearListaSimple(&(recursos->listaDeJugadores));

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
        mostrarCaracteresValidos();
        system("pause");
        system("cls");

        printf("Lista de jugadores en el orden que deben jugar:\n");
        mostrarListaSimpleEnOrden(&(recursos->listaDeJugadores), mostrarJugador);
        system("pause");
        system("cls");

        if(OK != (retornoCodigoDeError = iniciarJuego(recursos)))
        {
            fprintf(stderr, "No se pudo jugar, intente nuevamente.\n");
            mapEnListaSimple(&(recursos->listaDeJugadores), recursos, NULL, liberarListasDeCadaJugador);
            vaciarListaSimple(&(recursos->listaDeJugadores));
            return retornoCodigoDeError;
        }

        if(OK != (retornoCodigoDeError = generarInforme(recursos, construccionNombreArchivoTxtInforme)))
        {
            fprintf(stderr, "No pude grabar archivo de informe.\n");
            mapEnListaSimple(&(recursos->listaDeJugadores), recursos, NULL, liberarListasDeCadaJugador);
            vaciarListaSimple(&(recursos->listaDeJugadores));
            return retornoCodigoDeError;
        }
    }

    mapEnListaSimple(&(recursos->listaDeJugadores), recursos, NULL, liberarListasDeCadaJugador);
    vaciarListaSimple(&(recursos->listaDeJugadores));

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


