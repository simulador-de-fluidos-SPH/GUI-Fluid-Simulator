#ifndef ENERGYCHART_H
#define ENERGYCHART_H

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
#include <cmath>

using namespace Eigen;

class EnergyChart: public QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT

public:
    EnergyChart(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    void energyCalc();
    void chartRender();
    void barsBase();
};


#endif // ENERGYCHART_H
