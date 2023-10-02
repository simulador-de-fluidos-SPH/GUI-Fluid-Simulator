#ifndef OPENGLSIMULATION_H
#define OPENGLSIMULATION_H

#include <QColor>
#include <QtOpenGLWidgets>
#include <QOpenGLFunctions>

#include <iostream>
#include <Eigen/Dense>
#include <vector>
#include <random>

#include <GL/glu.h>

class OpenGLSimulation : public QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenGLSimulation(QWidget *parent = nullptr);

    // Se definen cabezeras de funciones

protected:
    // Se redefinen las funcioines por defecto de qt
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
private:

    void Update(void) const;

    void qColorToRGB(const QColor &C, float &r, float &g, float &b) const;
    float normaliza_0_1(float val, float min, float max) const;
};

#endif // OPENGLSIMULATION_H
