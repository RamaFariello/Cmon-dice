#include "funciones.h"

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

    ///*********************************IMPLEMENTAR TAM CONSOLA FIJA*********************************

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
