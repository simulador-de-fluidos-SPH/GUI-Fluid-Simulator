// Este código maneja el monitor de particula de el dockWidget

#include "particlemonitor.h"

particleMonitor::particleMonitor(QWidget *parent)
    :QOpenGLWidget { parent }
{

}

using namespace std;
using namespace Eigen;

Vector2d arrEndX(pmz(1)*0.9, pmz(1)*0.1); // arrEndx: arrows End Delimitará donde se acaban de dibujar las flechas para el eje x
Vector2d arrEndY(pmz(0)*0.1, pmz(0)*0.9); // arrEndy: arrows End Delimitará donde se acaban de dibujar las flechas para el eje y

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



    vectorRender(); // Se mostrarán los vectores si el puntero apunta a una particula

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
    glOrtho(-pmz(0), pmz(0), -pmz(1), pmz(1), 10, -10); // Se ajusta a los valores de particle monitor zoom pmz
}

// Renderiza los vectores del monitor
void particleMonitor::vectorRender(){ // Recibe un elemento tipo Particle
    if(particleSelected){ // Se consulta si la particula está seleccionada
       // Particle p = *particlePointer;

        glEnable(GL_POINT_SMOOTH);
        glPointSize(11.0f);
        glBegin(GL_POINTS);
        glColor3f(18.0f/255.0f, 215.0f/255.0f, 23.0f/255.0f);
        glVertex3f(0.0f, 0.0f, 0.1f);
        glEnd();

        // Imagen del vector fuerza de gravedad
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glColor3f(0.8f, 0.2f, 0.1f); // Rojo
        glVertex2f(0.0f, 0.0f);
        glVertex2f(particlePFgrav(0), particlePFgrav(1)); // Vector por densidad
        glEnd();

        // Imagen del vector fuerza de viscocidad
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glColor3f(0.8f, 0.4f, 0.8f); // Morado
        glVertex2f(0.0f, 0.0f);
        glVertex2f(particlePFvisc(0), particlePFvisc(1)); // Vector por densidad
        glEnd();

        // Imagen del vector fuerza de presión
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glColor3f(0.251f, 0.878f, 0.816f); // Turquesa
        glVertex2f(0.0f, 0.0f);
        glVertex2f(particlePFpress(0), particlePFpress(1));  // Vector por densidad
        glEnd();

        // Imagen del vector fuerza total
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glColor3f(0.1f, 0.8f, 0.2f); // Verde
        glVertex2f(0.0f, 0.0f);
        glVertex2f(particlePForce(0), particlePForce(1)); // Vector por densidad
        glEnd();

        // Imagen del vector velocidad
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glColor3f(0.1f, 0.2f, 0.8f); // Azul
        glVertex2f(0.0f, 0.0f);
        glVertex2f(particlePVelocity(0)*3, particlePVelocity(1)*3); // Vector por densidad
        glEnd();


        //printf("\n\nv: (%f, %f) \nf: (%f, %f)\nrho: %f", p.v(0) * p.rho, p.v(1) * p.rho, p.f(0) * p.rho, p.f(1) * p.rho, p.rho);
        //printf("\n\nG: (%f, %f) \nrho: %f", particlePointerFgrav(0), particlePointerFgrav(1), p.rho);
    }

}

// Genera la figura del plano cartesiano
void particleMonitor::plano(){
    glLineWidth(1.4f);
    glColor3f(0, 0, 0); // Define color
    // EJE X
    glBegin(GL_LINES);
    glVertex3f(-pmz(0), 0.0f, 0.2f);
    glVertex3f(pmz(0), 0.0f, 0.2f);
    glEnd();

    // Flechas positivas de X
    glBegin(GL_LINES);
    glVertex3f(pmz(0), 0.0f, 0.2f);
    glVertex3f(arrEndX(0),arrEndX(1), 0.2f);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(pmz(0), 0.0f, 0.2f);
    glVertex3f(arrEndX(0), -arrEndX(1), 0.2f);
    glEnd();

    // EJE Y
    glBegin(GL_LINES);
    glVertex3f(0.0f, -pmz(1), 0.2f);
    glVertex3f(0.0f, pmz(1), 0.2f);
    glEnd();

    // Flechas positivas de Y
    glBegin(GL_LINES);
    glVertex3f(0.0f, pmz(1), 0.2f);
    glVertex3f(arrEndY(0), arrEndY(1), 0.2f);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(0.0f, pmz(1), 0.2f);
    glVertex3f(-arrEndY(0), arrEndY(1), 0.2f);
    glEnd();
}
