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
QPoint mousePos; // Posición del mouse en movimiento

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

    this->resize(1000 - globalUi->dockWidget->width(), this->height()); // Se cambia el tamaño del widdget

    InitSPH(); // Se crean las parituclas de la simulación
}

// En esta función se pintan las cosas de la ventana
void OpenGLSimulation::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Borra los pixeles de la pantalla estableciendolos a el color definido en glClearColor

    // Se pregunta si la simulación está activa para calcular nuevas posiciones
    if(simulationActive){
        if(concentricForceActive){
            concentricForce(); // Se ejecuta los cálculos de la simulación teniendo en cuenta la fuerza concéntrica
        } else {
            // Se llaman las funciones de cálculo de nueva posición
            ComputeDensityPressure();
            ComputeForces();
            Integrate();
        }
        particlesBackup.push_back(particles); // se añade el esta de particles al backup
        // cout << particlesBackup.size() * 96 * particles.size() / 1000000.0f << "MB" << endl;

    }

    glEnable(GL_POINT_SMOOTH); // Le da forma circular a los puntos

    // glPointSize(0.02f * this->width()); // Ajusta el tamaño de los puntos a el tamaño del openGLWidget
    glPointSize(H); // Ajusta el tamaño de los puntos a H

    glBegin(GL_POINTS); // Se inicializa la creación de puntos GL_POINTS

    // Ciclo for que renderiza las particulas
    for (auto &p : particles) // Recorre las particulas del vector particles
    {
        glColor3f(p.red, p.green, p.blue); // le da a opengl el color de los puntos
        if(particlePointer == &p){

            glColor3f( 0.90039f, 0.90039f, 0.14000f ); // Define el color de los puntos
            glVertex2f(p.x(0), p.x(1)); // Se muestra el vector normalizado en coordenadas norx nory
        } else {
            glVertex2f(p.x(0), p.x(1)); // Se muestra el vector normalizado en coordenadas norx nory
        }
    }
    glEnd();

    // Renderización de la linea de lanzamiento de particula si la opción está activada
    if(launchPressed){
        glLineWidth(7.0f);
        glBegin(GL_LINES);
        glColor3f(0.1f, 0.1f, 0.1f);
        glVertex3f(launchInitPos(0), launchInitPos(1), 1.0f);
        glVertex3f(mouseMovePosition(0), mouseMovePosition(1), 1.0f);
        glEnd();
    }

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
    mouseEvent = e; // Puntero global que apunta al evento del ratón
    if (e->button() == Qt::LeftButton) // Verifica que se hizo clic con el botón izquierdo del ratón
    {
        int hs = herramientaSeleccionada;

        // Se comprueba que botón fue presionado para saber que función ejecutar
        if(hs == 1){
            // cout << "Herramienta seleccionada = 1" << endl;
            #if defined(Q_OS_LINUX)
                    QPointF positionElement = e->localPos();
            #elif defined(Q_OS_WINDOWS)
                    QPointF positionElement = e->position();
            #endif

            float x = positionElement.x();
            float y = (height() - positionElement.y());
            Vector2d currentPosition(x,y);
            launchInitPos = currentPosition; // Posición incicial de click
            mouseMovePosition = currentPosition; // posición inicial de movimiento
            launchPressed = true;
        }
        else if(hs == 2){
            concentricForceActive = true;
            //globalUi->concentricForce->setText(QString::number(concentricForceMag / proxRho / 100, 'f', 2));
        } else if(hs == 3) {
            deleteParticle(e);
            deletePressed = true;
        } else {
            particlePointerSetter(e);
        }

        /*
        FUNCIONES PARA IMPLEMENTAR (índice)
        0: Función para seleccionar particula (v) [Por defecto botón izquierdo]
        1: Función para crear particula (v) [por defecto botón derecho]
        2: Función de Fuerza concéntrica (v)
        3: Función de eliminar particula (v)
        4: Función de lanzar particula (v)
        */

    } else if (e->button() == Qt::RightButton)
    { // Funciones para lanzar una partícula desde cualquier dirección
        mPressed = true;
        newParticle(e->x(), height() - e->y(), globalUi->Fluid_Menu->currentIndex());
        mousePressPos = e->pos(); // Almacena la posición de presionado
    }
    mousePos = e->pos();
}

void OpenGLSimulation::mouseMoveEvent(QMouseEvent *e)
{
    mouseEvent = e;
    mousePos = e->pos();
    if(launchPressed){

        #if defined(Q_OS_LINUX)
                QPointF positionElement = e->localPos();
        #elif defined(Q_OS_WINDOWS)
                QPointF positionElement = e->position();
        #endif
        float x = positionElement.x();
        float y = (height() - positionElement.y());
        Vector2d currentPosition(x,y);
        mouseMovePosition = currentPosition;

        Vector2d velocity = (mouseMovePosition - launchInitPos);
        float mag = (velocity * velocity.norm() * -1).norm() * proxRho / 100;

        globalUi->throwSpeed->setText(QString::number(mag, 'f', 2)); // Se asigna un valor a la casilla de velocidad de lanzamiento

        // Calcula el ángulo en radianes
        float angleRad = std::atan2(velocity.y(), velocity.x());

        // Convierte el ángulo a grados
        float angleDeg = angleRad * (180.0 / M_PI);

        angleDeg = fmod(angleDeg + 360, 360);

        globalUi->throwSpeed_Direction->setText(QString::number(angleDeg, 'f', 1));

    }
    if (mPressed)
    {
        newParticle(e->x(), height() - e->y(), globalUi->Fluid_Menu->currentIndex());
    }
    if(deletePressed){
       deleteParticle(e);
    }
}

void OpenGLSimulation::mouseReleaseEvent(QMouseEvent *e)
{
    mouseEvent = e;
    mPressed = false;
    concentricForceActive = false; // Se desactiva concenctricForce si estaba activada
    deletePressed = false;
    if(launchPressed){

       newParticle(launchInitPos(0), launchInitPos(1), globalUi->Fluid_Menu->currentIndex());

       Vector2d velocity = (mouseMovePosition - launchInitPos);
       velocity = velocity * velocity.norm() * -1;

       particles.back().v = velocity;
       particlePointer = &particles.back();
       particleSelected = true;
       globalUi->throwSpeed->setText(QString::number(0, 'f', 2));
       globalUi->throwSpeed_Direction->setText(QString::number(0, 'f', 1));

       launchPressed = false;
    }
}

// Función para seleccionar una particula y devuelve la dirección de memoria de la particula seleccionada como dirección void no Particle
void* OpenGLSimulation::selectParticle(QMouseEvent *e){

#if defined(Q_OS_LINUX)
    QPointF positionElement = e->localPos();
#elif defined(Q_OS_WINDOWS)
    QPointF positionElement = e->position();
#endif

    Particle* localPointer = nullptr;

    float minDiff = H; // Minima diferencia para que la selección sea valida
    float positionx = positionElement.x();
    float positiony = (height() - positionElement.y());

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
            particleSelected = false;
            particlePointer = nullptr;
            break;
        }
    }
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

void OpenGLSimulation::concentricForce(){

    Vector2d pos(mousePos.x(), height() - mousePos.y());


    /* Se llaman las tres funciones de la simulación, compute forces se sobre escribe para que acepte una fuerza externa
        en una particula en particular */
    ComputeDensityPressure();
    #pragma omp parallel for
    for (auto &pi : particles) // primer iterador pi
    {
        Vector2d fpress(0.f, 0.f);
        Vector2d fvisc(0.f, 0.f);
        for (auto &pj : particles) // segundo iterador pj
        {
            if (&pi == &pj) // Si ambos punteros apuntan a la misma particula iterar sobre el siguiente &pj
            {
                continue;
            }

            Vector2d rij = pj.x - pi.x; // Vector que relaciona la posición entre pi y pj
            float r = rij.norm(); // Norma de rij

            if (r < H)
            {
                fpress += -rij.normalized() * pi.MASS * (pi.p + pj.p) / (2.f * pj.rho) * SPIKY_GRAD * pow(H - r, 3.f);
            }
        }
        Vector2d fgrav = G * pi.MASS / pi.rho;

        pi.f = fpress + fvisc + fgrav;

        // Comprueba si la particula está suficientemente cerca del punto de click para ser afectada, de ser así, se añade la fuerza externa al cálculo
        if ((pi.x - pos).norm() <= 3 * H)
        {
            Vector2d externalForce;
            float angulo;

            // Calcular la diferencia en coordenadas x e y
            float dx = pos.x() - pi.x.x();
            float dy = pos.y() - pi.x.y();

            // Calcular el ángulo utilizando atan2
            angulo = atan2(dy, dx);

            // Aplicar la fuerza externa
            externalForce = Vector2d(cos(angulo), sin(angulo)) * globalUi->concentricForceMag->value() / proxRho * -1;

            pi.f += externalForce;
        }

        // Se normalizan las fuerzas que se mostrarán en pantalla
        if(&pi == particlePointer){ // Si la particula iterada es la particula seleccionada guardar las fuerzas
            particlePVelocity = norMagnitude(pi.v, pi.rho);
            particlePForce = norMagnitude(pi.f, pi.rho);
            particlePFgrav = norMagnitude(fgrav, pi.rho);
            particlePFpress = norMagnitude(fpress, pi.rho);
            particlePFvisc = norMagnitude(fvisc, pi.rho);
        }
    }
    Integrate();
}


