#ifndef OPENGLSIMULATION_H
#define OPENGLSIMULATION_H

#include <QColor>

//######################## Arch / Windows ########################
#if defined(Q_OS_LINUX)
#include <QOpenGLWidget>
#elif defined(Q_OS_WINDOWS)
#include <QtOpenGLWidgets>
#endif

#include <QOpenGLFunctions>
#include <QDebug>
#include <QMouseEvent>
#include <QPointF>

#include <iostream>
#include <Eigen/Dense>
#include <vector>
#include <random>
#include <omp.h>

class OpenGLSimulation : public QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLSimulation(QWidget *parent = nullptr);

    // Se definen cabezeras de funciones

protected:
    // Se redefinen las funciones por defecto de qt
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
private:

    // Simulación
    void InitSPH();
    void ComputeForces();
    void ComputeDensityPressure();
    void Integrate();

    // Funciones de heramientas
    void* selectParticle(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void particlePointerSetter(QMouseEvent *e);
    void deleteParticle(QMouseEvent *e);

    void qColorToRGB(const QColor &C, float &r, float &g, float &b) const;
    float normaliza_0_1(float val, float min, float max) const;
};

#endif // OPENGLSIMULATION_H
