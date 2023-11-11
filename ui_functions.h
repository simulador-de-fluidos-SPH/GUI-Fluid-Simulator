#ifndef UI_FUNCTIONS_H
#define UI_FUNCTIONS_H

#include "ui_mainwindow.h"

// función que modifica las casillas de ui con los nuevos valores de los datos
void modUiData();

// Esta función crea una nueva particula en el vector particles con los datos del arreglo props en un indice dado
void newParticle(float _x, float _y, int index);

#endif // UI_FUNCTIONS_H
