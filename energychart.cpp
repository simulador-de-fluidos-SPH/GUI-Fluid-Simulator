#include "energychart.h"

EnergyChart::EnergyChart(QWidget *parent)
    :QOpenGLWidget { parent }
{

}

using namespace std;
using namespace Eigen;

void EnergyChart::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
}


// En esta función se pintan las cosas de la ventana
void EnergyChart::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(particleSelected){
        energyCalc();
        chartRender();
    } else {
        barsBase();
    }

    update();

}

// Esta función crea la ventana
void EnergyChart::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); // Carga la proyección
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // Carga la matriz identidad
    glOrtho(0.0, 1.0, 0.0, 1.0, 10, -10);
}

// Función que cálcula las energías de particleSelected
void EnergyChart::energyCalc(){
    float magVelocidad = particlePointer->v.norm();
    kineticE = 0.5 * particlePointer->MASS * (magVelocidad * magVelocidad) / 15700;

    potentialE = particlePointer->MASS * (abs(G(1)) + abs(G(0))) * particlePointer->x(1);

    mechanicalE = kineticE + potentialE;
    /*printf("%.1f ", mechanicalE);
    printf("%.1f ", kineticE);
    printf("%.1f\n", potentialE);*/
}

// Renderiza los vectores del monitor
void EnergyChart::chartRender(){ // Recibe un elemento tipo Particle
    glLineWidth(2.0f);  // Ancho del delineado

    float kineticBar = kineticE / mechanicalE;
    float potentialBar = potentialE / mechanicalE;
    float mechanincalBar = 1.0f;
    float chartLimit = 10000;

    if(mechanicalE < chartLimit){
        kineticBar = kineticE / chartLimit;
        potentialBar = potentialE / chartLimit;
        mechanincalBar = mechanicalE / chartLimit;
    } else {
        kineticBar = kineticE / mechanicalE;
        potentialBar = potentialE / mechanicalE;
    }


    // Kinetic
    glBegin(GL_POLYGON);
    glColor3f(0.251f, 0.878f, 0.816f);
    glVertex2f(0.15f, 0.01f);
    glVertex2f(0.25f, 0.01f);
    glVertex2f(0.25f, kineticBar - 0.01f);
    glVertex2f(0.15f, kineticBar - 0.01f);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Activa el modo de renderizado de líneas

    // Kinetic - Borde
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.0f, 0.0f);  // Color del borde (negro en este caso)
    glVertex2f(0.15f, 0.01f);
    glVertex2f(0.25f, 0.01f);
    glVertex2f(0.25f, kineticBar - 0.01f);
    glVertex2f(0.15f, kineticBar - 0.01f);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Vuelve al modo de renderizado normal



    // Potential
    glBegin(GL_POLYGON);
    glColor3f(0.11765f, 0.78431f, 0.17647f);
    glVertex2f(0.45f, 0.01f);
    glVertex2f(0.55f, 0.01f);
    glVertex2f(0.55f, potentialBar - 0.01f);
    glVertex2f(0.45f, potentialBar - 0.01f);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Activa el modo de renderizado de líneas

    // Potential - Borde
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.0f, 0.0f);  // Color del borde (negro en este caso)
    glVertex2f(0.45f, 0.01f);
    glVertex2f(0.55f, 0.01f);
    glVertex2f(0.55f, potentialBar - 0.01f);
    glVertex2f(0.45f, potentialBar - 0.01f);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Vuelve al modo de renderizado normal

    // Mechanical
    glBegin(GL_POLYGON);
    glColor3f(0.70588f, 0.70588f, 0.0f);
    glVertex2f(0.75f, 0.01f);
    glVertex2f(0.85f, 0.01f);
    glVertex2f(0.85f, mechanincalBar  - 0.01f);
    glVertex2f(0.75f, mechanincalBar  - 0.01f);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Activa el modo de renderizado de líneas

    // Mechanical - Borde
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.0f, 0.0f);  // Color del borde (negro en este caso)
    glVertex2f(0.75f, 0.01f);
    glVertex2f(0.85f, 0.01f);
    glVertex2f(0.85f, mechanincalBar - 0.01f);
    glVertex2f(0.75f, mechanincalBar - 0.01f);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Vuelve al modo de renderizado normal
}

// Genera la figura del plano cartesiano
void EnergyChart::barsBase(){
    glLineWidth(2.0f);  // Ancho del delineado

    // Kinetic
    glBegin(GL_POLYGON);
    glColor3f(0.21569f, 0.50980f, 0.84314f);
    glVertex2f(0.15f, 0.01f);
    glVertex2f(0.25f, 0.01f);
    glVertex2f(0.25f, 0.01f);
    glVertex2f(0.15f, 0.01f);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Activa el modo de renderizado de líneas

    // Kinetic - Borde
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.0f, 0.0f);  // Color del borde (negro en este caso)
    glVertex2f(0.15f, 0.01f);
    glVertex2f(0.25f, 0.01f);
    glVertex2f(0.25f, 0.01f);
    glVertex2f(0.15f, 0.01f);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Vuelve al modo de renderizado normal

    // Potential
    glBegin(GL_POLYGON);
    glColor3f(0.11765f, 0.78431f, 0.17647f);
    glVertex2f(0.45f, 0.01f);
    glVertex2f(0.55f, 0.01f);
    glVertex2f(0.55f, 0.01f);
    glVertex2f(0.45f, 0.01f);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Activa el modo de renderizado de líneas

    // Potential - Borde
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.0f, 0.0f);  // Color del borde (negro en este caso)
    glVertex2f(0.45f, 0.01f);
    glVertex2f(0.55f, 0.01f);
    glVertex2f(0.55f, 0.01f);
    glVertex2f(0.45f, 0.01f);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Vuelve al modo de renderizado normal

    // Mechanical
    glBegin(GL_POLYGON);
    glColor3f(0.70588f, 0.70588f, 0.0f);
    glVertex2f(0.75f, 0.01f);
    glVertex2f(0.85f, 0.01f);
    glVertex2f(0.85f, 0.01f);
    glVertex2f(0.75f, 0.01f);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Activa el modo de renderizado de líneas

    // Mechanical - Borde
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.0f, 0.0f);  // Color del borde (negro en este caso)
    glVertex2f(0.75f, 0.01f);
    glVertex2f(0.85f, 0.01f);
    glVertex2f(0.85f, 0.01f);
    glVertex2f(0.75f, 0.01f);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Vuelve al modo de renderizado normal
}




