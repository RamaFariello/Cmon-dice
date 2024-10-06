#ifndef GENERICO_H_INCLUDED
#define GENERICO_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NO_PUDE_ABRIR_ARCHIVO_GENERICO 0
#define OK 1

int abrirArchivo( FILE** pf, const char* nombreArchivo, const char* modo );
void mostrarColeccion( const void* dato, int tam, int ce, void (*mostrar)( const void* dato ) );
void grabar( FILE* pf, const void* dato, void (*grabarSegunEstrategia)( FILE* pf, const void* dato ) );

#endif // GENERICO_H_INCLUDED
