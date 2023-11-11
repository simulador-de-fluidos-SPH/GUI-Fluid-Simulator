#include "openglsimulation.h"
#include "ui_mainwindow.h"
#include "particleproperties.h"
#include "globalVariables.h"
#include "sph_functions.h"
#include "ui_functions.h"

OpenGLSimulation::OpenGLSimulation(QWidget *parent)
    :QOpenGLWidget { parent }
{
    setMouseTracking(true); // Se habilita la seguimiento del ratón
}
using namespace std;
using namespace Eigen;

// -------------------------- Variables globales que solo serán usadas en este widget -------------------------- //

int fps = 0; // Contador de fps
time_t timer = time(0);

//Variables para rastrear movimiento del mouse
bool mPressed = false; //para saber si el mouse esta presionado o no
QPoint mousePressPos; //Posicion de donde se presionó el mouse
QPoint mouseReleasePos; //Posicion de donde se soltó el mouse

// Inicializa el opengl
void OpenGLSimulation::initializeGL()
{
    // Colores del fondo de la pantalla
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;

    initializeOpenGLFunctions(); // Prepara el progrmaa para ejecutar funciones de openGL
    glClearColor(r, g, b, a); // Establece el color de fondo de la ventana
    glEnable(GL_DEPTH_TEST); // Función para determinar si un obejto se dibuja delante o detras de otro
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); // Configura como se aplica el color a los objetos
    glEnable(GL_COLOR_MATERIAL); // Habilita la capacidad de controlar los colores

    InitSPH(); // Se crean las parituclas de la simulación
}

// En esta función se pintan las cosas de la ventana
void OpenGLSimulation::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Borra los pixeles de la pantalla estableciendolos a el color definido en glClearColor

    // Se llaman las funciones de cálculo de nueva posición
    ComputeDensityPressure();
    ComputeForces();
    Integrate();

    glEnable(GL_POINT_SMOOTH); // Le da forma circular a los puntos

    // glPointSize(0.02f * this->width()); // Ajusta el tamaño de los puntos a el tamaño del openGLWidget
    glPointSize(H); // Ajusta el tamaño de los puntos a H

    glBegin(GL_POINTS); // Se inicializa la creación de puntos GL_POINTS

    // Ciclo for que renderiza las particulas
    for (auto &p : particles) // Recorre las particulas del vector particles
    {
        glColor3f(p.red, p.green, p.blue); // le da a opengl el color de los puntos
        if(particlePointer == &p){

            glColor3f(.6f, .4f, .3f); // Define el color de los puntos
            glVertex2f(p.x(0), p.x(1)); // Se muestra el vector normalizado en coordenadas norx nory
        } else {
            glVertex2f(p.x(0), p.x(1)); // Se muestra el vector normalizado en coordenadas norx nory
        }
    }
    glEnd();

    glBegin(GL_POINTS);
    glColor3f(.8, .8, .3); // Define el color de los puntos
    glEnd();

    modUiData(); // Modifica los datos de fuerza y velocidad de ui
    if(time(0) > timer){ // Verifica si ha pasado un segundo
        globalUi->FPS_Shower->setText(QString::fromStdString(to_string(fps))); // Modifica el label de ui por la cantidad de fps
        timer = time(0);  // Resetea el tiempo al tiempo actual
        fps = 0;
    } else {
        fps++; // Cuenta un nuevo fps
    }

    // un update() es equivalente a un fotograma
    // update() Reinicia la función paintGL(), pausar basta con evitar la ejecución de update()
    update();

}

// Esta función crea la ventana
void OpenGLSimulation::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h); // Se establece una ventana esquina inferior izquierda en (0, 0) con ancho y alto del widget
    glMatrixMode(GL_PROJECTION); // Carga la proyección
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // Carga la matriz identidad

    // ################# Bounding interaction implementation #################
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
    VIEW_WIDTH =  static_cast<double>(w);
    VIEW_HEIGHT =  static_cast<double>(h);

    glOrtho(0.0, VIEW_WIDTH, 0.0, VIEW_HEIGHT, -1.0, 1.0); // Configura la cámara para que apunte y tenga el tamaño del openGLWidget

}

// -------------------------------------------- TOOLS -------------------------------------------- //

// Función que se ejecutará cada que el mouse es presionado - Esto hay que adecuarlo a los botones
void OpenGLSimulation::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) // Verifica que se hizo clic con el botón izquierdo del ratón
    {
        int hs = herramientaSeleccionada;

        // Operadores ternarios para saber que opción tiene seleccionada el usuario y ejecutar una función
        hs == 3 ? deleteParticle(e) : particlePointerSetter(e);

        /*
        FUNCIONES PARA IMPLEMENTAR (índice)
        0: Función para seleccionar particula (v) [Por defecto]
        1: Función para crear particula (x)
        2: Función de splash (x)
        3: Función de eliminar particula (v)
        4: Función de explosión (x)
        */


    } else if (e->button() == Qt::RightButton)
    { // Funciones para lanzar una partícula desde cualquier dirección
        mPressed = true;
        mousePressPos = e->pos(); // Almacena la posición de presionado
    }
}

void OpenGLSimulation::mouseMoveEvent(QMouseEvent *e)
{
    if (mPressed)
    {
        // Calcula la distancia entre la posición actual y la posición de presionado
        int deltaX = e->x() - mousePressPos.x();
        int deltaY = e->y() - mousePressPos.y();

        // Calcula la velocidad en función de la distancia y un factor de escala
        float scale = 0.1f; // Ajusta este valor según la velocidad deseada
        float velx = static_cast<float>(deltaX) * scale;
        float vely = static_cast<float>(deltaY) * scale;

        // Crea una nueva partícula con la posición inicial en el punto de clic
        newParticle(e->x(), height() - e->y(), globalUi->Fluid_Menu->currentIndex());
        // Asigna la velocidad a la última partícula en el vector
        if (!particles.empty()) {
            particles.back().v = Vector2d(velx, vely);
        }
    }
}

void OpenGLSimulation::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
    {
        mPressed = false;
    }
}

// Función para seleccionar una particula y devuelve la dirección de memoria de la particula seleccionada como dirección void no Particle
void* OpenGLSimulation::selectParticle(QMouseEvent *e){

    // ################# Arch / Windows #################
#if defined(Q_OS_LINUX)
    QPointF positionElement = e->localPos();
#elif defined(Q_OS_WINDOWS)
    QPointF positionElement = e->position();
#endif

    Particle* localPointer = nullptr;

    float minDiff = H; // Minima diferencia para que la selección sea valida
    float positionx = positionElement.x() * WINDOW_WIDTH / width(); // Se encuentra y normaliza la coordenada x
    float positiony = (height() - positionElement.y()) * VIEW_HEIGHT / height(); // Se encuentra y normaliza la coordenada y

    Vector2d pos(positionx, positiony); // Se define un vector con lo antes encontrado

    for (auto &p : particles){

        if((p.x - pos).norm() <= H/2 && (p.x - pos).norm() < minDiff) // Pregunra si la norma del vector entr pos y particle es menor a el radio del kernel
        {
            minDiff = (p.x - pos).norm();
            localPointer = &p;
        }

    }

    return localPointer;
}

// Función para borrar una particula
void OpenGLSimulation::deleteParticle(QMouseEvent *e){
    Particle* localPointer = (Particle*)selectParticle(e); // Se declara un puntero que almacena la posición de la particula seleccionada

    for (auto it = particles.begin(); it != particles.end(); ++it) { // ciclo que recorre el vector de particulas
        if (&(*it) == localPointer) {
            particles.erase(it); // Elimina la partícula cuando la encuentra
            if(localPointer == particlePointer){ // En caso de que la particula eliminada sea la seleccionada globalmente el puntero se volverá null
                particleSelected = false;
                particlePointer = nullptr;
            }
            break;
        }
    }
}

// Función para asignarle a la variable global particlePointer la particula tocada o null si no fue tocada una particula
void OpenGLSimulation::particlePointerSetter(QMouseEvent *e){

    Particle* localPointer = (Particle*)selectParticle(e);

    if(localPointer == nullptr){
        particleSelected = false;
    } else {
        particleSelected = true;
    }

    particlePointer = localPointer; // Asigna la dirección de la particula seleccionada a la variable global
}

/*void OpenGLSimulation::launchParticle(QMouseEvent *e)
{
#if defined(Q_OS_LINUX)
    QPointF positionElement = e->localPos();
#elif defined(Q_OS_WINDOWS)
    QPointF positionElement = e->position();
#endif

    float positionX = positionElement.x() * WINDOW_WIDTH / width();
    float positionY = (height() - positionElement.y()) * VIEW_HEIGHT / height();

}*/
