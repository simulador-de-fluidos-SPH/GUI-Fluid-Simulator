// Este código maneja el monitor de particula de el dockWidget

#include "particlemonitor.h"

particleMonitor::particleMonitor(QWidget *parent)
    :QOpenGLWidget { parent }
{

}

using namespace std;
using namespace Eigen;

// La estructura particle está definida en particlemonitor.h

extern bool particleSelected; // Se sabe si hay una particula seleccionada o no
extern Particle* particlePointer; // Pide la dirección de memoria de la particula seleccionada

// opngl init
void particleMonitor::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
}


// En esta función se pintan las cosas de la ventana
void particleMonitor::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    plano(); // Se crea el plano

    glEnable(GL_POINT_SMOOTH);
    glPointSize(8.0f);
    glBegin(GL_POINTS);
    glColor3f(18.0f/255.0f, 215.0f/255.0f, 23.0f/255.0f);
    glVertex3f(0.0f, 0.0f, 0.1f);
    glEnd();

    vectorRender(particlePointer); // Se mostrarán los vectores si el puntero apunta a una particula

    update();

}

// Esta función crea la ventana
void particleMonitor::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); // Carga la proyección
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // Carga la matriz identidad
    // glOrtho(0.0, VIEW_WIDTH, 0.0, VIEW_HEIGHT, -1.0, 1.0);
}

// Renderiza los vectores del monitor
void particleMonitor::vectorRender(Particle* particlePointer){ // Recibe un elemento tipo Particle
    if(particleSelected){ // Se consulta si la particula está seleccionada
       Particle p = *particlePointer;

        // Imagen del vector velocidad
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glColor3f(0.1f, 0.2f, 0.8f); // Define color
        glVertex2f(0.0f, 0.0f); //
        glVertex2f(p.v(0)/1000, p.v(1)/1000); //
        glEnd();

       printf("\n\nv: (%f, %f) \nf: (%f, %f)", p.v(0), p.v(1), p.f(0), p.f(1));
    }

}

// Genera la figura del plano cartesiano
void particleMonitor::plano(){
    glLineWidth(1.4f);
    glColor3f(0, 0, 0); // Define color
    // EJE X
    glBegin(GL_LINES);
    glVertex3f(-1.0f, 0.0f, 0.2f);
    glVertex3f(1.0f, 0.0f, 0.2f);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(1.0f, 0.0f, 0.2f);
    glVertex3f(0.9f, 0.1f, 0.2f);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(1.0f, 0.0f, 0.2f);
    glVertex3f(0.9f, -0.1f, 0.2f);
    glEnd();

    // EJE Y
    glBegin(GL_LINES);
    glVertex3f(0.0f, -1.0f, 0.2f);
    glVertex3f(0.0f, 1.0f, 0.2f);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(0.0f, 1.0f, 0.2f);
    glVertex3f(0.1f, 0.9f, 0.2f);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(0.0f, 1.0f, 0.2f);
    glVertex3f(-0.1f, 0.9f, 0.2f);
    glEnd();
}
