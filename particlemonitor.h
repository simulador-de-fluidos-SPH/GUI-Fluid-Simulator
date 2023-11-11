#ifndef PARTICLEMONITOR_H
#define PARTICLEMONITOR_H

// Estos son los headers del monitor de la particula seleccionada

#include <QOpenGLFunctions>
#include "globalVariables.h"

//######################## Arch / Windows ########################
#if defined(Q_OS_LINUX)
#include <QOpenGLWidget>
#elif defined(Q_OS_WINDOWS)
#include <QtOpenGLWidgets>
#endif

#include <iostream>
#include <Eigen/Dense>
#include <vector>

using namespace Eigen;

class particleMonitor: public QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT

public:
    particleMonitor(QWidget *parent = nullptr);

    // Se definen cabezeras de funciones

protected:
    // Se redefinen las funciones por defecto de qt
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    void plano();
    void vectorRender(Particle* p);
};

#endif // PARTICLEMONITOR_H
