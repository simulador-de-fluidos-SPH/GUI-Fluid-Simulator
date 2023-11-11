#ifndef OPENGLSIMULATION_H
#define OPENGLSIMULATION_H

#include <QOpenGLFunctions>

//######################## Arch / Windows ########################
#if defined(Q_OS_LINUX)
#include <QOpenGLWidget>
#elif defined(Q_OS_WINDOWS)
#include <QtOpenGLWidgets>
#endif

#include <QDebug>
#include <QMouseEvent>
#include <QPointF>
#include <QString>

#include <Eigen/Dense>
#include <vector>
#include <ctime>

using namespace Eigen;

class OpenGLSimulation : public QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLSimulation(QWidget *parent = nullptr);

protected:
    // Se redefinen las funciones por defecto de qt
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
private:

    // Funciones de heramientas
    void* selectParticle(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void particlePointerSetter(QMouseEvent *e);
    void deleteParticle(QMouseEvent *e);
    void launchParticle(QMouseEvent *e);

};

#endif // OPENGLSIMULATION_H
