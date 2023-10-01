#include "openglsimulation.h"

OpenGLSimulation::OpenGLSimulation(QWidget *parent)
    :QOpenGLWidget { parent }
{

}

// Inicialisa el opengl
void OpenGLSimulation::initializeGL()
{
    float r, g, b, a = normaliza_0_1(255.0f, 1.0f, 255.0f); // Se declaran r, g, b y alfa como 1

    qColorToRGB(Qt::black, r, g, b); // A la función qColor se le pasa el color negro de QColor y se modifica r, g, b
    initializeOpenGLFunctions();
    glClearColor(r, g, b, a);
    glEnable(GL_DEPTH_TEST); // Prueba de profundidad
    glEnable(GL_LIGHT0); // Se activa la luz
    glEnable(GL_LIGHTING); // Se activa iluminación
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL); // Se activa glColorMaterial
}


// En esta función se pintan las cosas de la ventana
void OpenGLSimulation::paintGL()
{
    float r, g, b;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLineWidth(5.0f);
    // EJE X
    glBegin(GL_LINES);
    qColorToRGB(Qt::red, r, g, b);
    glColor3f(r, g, b); // Define color
    glVertex2f(-1.0f, 0.0f); // Vertice
    glVertex2f(1.0f, 0.0f); // Vertice
    glEnd();

    // EJE Y
    glBegin(GL_LINES);
    qColorToRGB(Qt::blue, r, g, b);
    glColor3f(r, g, b); // Define color
    glVertex2f(0.0f, -1.0f); // Vertice
    glVertex2f(0.0f, 1.0f); // Vertice
    glEnd();

    // Se inicia la sección de pintado
    glBegin(GL_TRIANGLES);
    qColorToRGB(Qt::cyan, r, g, b);
    glColor3f(r, g, b); // Define color
    glVertex2f(0.0f, 1.0f);
    glVertex2f(-0.5f, 0.0f);
    glVertex2f(0.5f, -0.2f);
    glEnd();

    glBegin(GL_LINE_LOOP); // GL_POLYGON
    qColorToRGB(Qt::red, r, g, b);
    glColor3f(r, g, b); // Define color
    glVertex2f(0.0f, -0.3f);
    glVertex2f(-0.5f, -0.4f);
    glVertex2f(-0.7f, -0.5f);
    glVertex2f(0.7f, -0.9f);
    glEnd();

    // glEnable(GL_POINT_SMOOTH);
    glPointSize(30.0f);
    glBegin(GL_POINTS);
    qColorToRGB(Qt::yellow, r, g, b);
    glColor3f(r, g, b); // Define color
    glVertex2f(0.2f, 0.9f);
    glEnd();

    // glTranslatef(0.5, 0.5, 0.5); // Se hacen desplazamientos de la cámara

}


// Esta función crea la ventana
void OpenGLSimulation::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); // Carga la proyección
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // Carga la matriz identidad
}

// Con esta función se convierte el color de qcolor a rgb
void OpenGLSimulation::qColorToRGB(const QColor &C, float &r, float &g, float &b) const
{
    // Se modifican las variables globales r, g, b a partir de los valores de QColor
    r = normaliza_0_1(C.red(), 1.0f, 255.0f);
    g = normaliza_0_1(C.green(), 1.0f, 255.0f);
    b = normaliza_0_1(C.blue(), 1.0f, 255.0f);
}

float OpenGLSimulation::normaliza_0_1(float val, float min, float max) const
{
    return (val - min) / (max - min);
}

