#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

/* En esta cabezera se nombran las variables para su uso en otros archivos,
en globalVariables.cpp se inicializan las variables*/

#include <QOpenGLFunctions>
#include <QMainWindow>
#include <Eigen/Dense>
#include <vector>

using namespace std;
using namespace Eigen;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Particle
{
    Particle(float _x, float _y, float _r, float _g, float _b,
             float _restDens, float _gasConst, float _mass, float _visc, float _dt)
        : x(_x, _y), v(0.f, 0.f), f(0.f, 0.f), rho(0), p(0.f), red(_r), green(_g), blue(_b),
        REST_DENS(_restDens), GAS_CONST(_gasConst), MASS(_mass), VISC(_visc), DT(_dt) {} // particle data structure

    Vector2d x, v, f; // stores position, velocity, and force for integration
    float rho, p;     // stores density (rho) and pressure values for SPH
    float red, green, blue; // particle colors

    float REST_DENS;
    float GAS_CONST;
    float MASS;
    float VISC;
    float DT;

};

// Definición de la estructura Data
struct Data {
    float red;
    float green;
    float blue;
    float REST_DENS;
    float GAS_CONST;
    float MASS;
    float VISC;
    float DT;
};


extern Data props[]; // Estructura que almacena los diferentes tipos de datos (definición en "particleproperties.cpp")

extern Ui::MainWindow* globalUi; // Conección con la ui de mainwindow
extern int herramientaSeleccionada; // Variable que alamcenará la herramienta seleccionada por el usuario
extern int* FM_Index; // Fluid menu index
extern bool simulationActive; // Booleano que almacena si la sumación está activa o no

extern float proxRho; // Valor aproximado de rho

extern bool particleSelected; // Se almacena si hay una particula seleccionada o no
extern Vector2d pmz; // pmz: particle monitor zoom

extern Particle* particlePointer; // Puntero que almacenará la dirección de memoria de una particula

extern vector<Particle> particles; // Se daclara un vector de tipo Particle llamado particles
extern vector<vector<Particle>> particlesBackup; // Vector de vectores particles para almacenar la imágen anterior en el render

extern long unsigned int stepIndex; // stepIndex es la cantidad de pasos que se devuelve el timeStepper

// --- Energy --- //
extern float kineticE;
extern float potentialE;
extern float mechanicalE;
// -------------- //

// --- TOOLS --- //
extern bool concentricForceActive; // Almacena si la función de fuerza concentrica está activada o no
extern float concentricForceMag; // Magnitud de fuerza concéntrica
extern QMouseEvent* mouseEvent; // Puntero que apunta a un evento de ratón
extern bool deletePressed;

extern bool launchPressed; // Almacena si la opción de launch fue presionada
extern Vector2d launchInitPos; // Posición donde se presiono el mouse con la opción de lanzar
extern Vector2d mouseMovePosition; // Posición del ratón al moverse
// ------------- //

extern float REST_DENS;  // rest density
extern float GAS_CONST;  // const for equation of state
extern float H;		     // kernel radius
extern float HSQ;		 // radius^2 for optimization
extern float MASS;		 // assume all particles have the same mass
extern float VISC;       // viscosity constant
extern float DT;	     // integration timestep

extern float r;  // red color
extern float g;  // green color
extern float b;  // blue color

extern float POLY6; // 4/(π * H⁸): Estima la densidad del fluido en función de la distancia con otras particulas
extern float SPIKY_GRAD; // -10/(π * H⁵): Ayuda a calcular las fuerzas que actuan sobre una particula
extern float VISC_LAP; // 40/(π * H⁵): Se utiliza para calculos relacionados con la densidad del fluido

// simulation parameters
extern float EPS; // Será la distancia de colición de la particula con los bordes
extern float BOUND_DAMPING; // variable que modificará la dirección de velocidad cuando la particula toca un borde, al ser menor que 1 esta disminuye en magnitud

// interaction
extern int MAX_PARTICLES; // Particulas máximas
extern long unsigned int DAM_PARTICLES; // Particulas generadas
extern int BLOCK_PARTICLES; // Particulas generables por click

// rendering projection parameters
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern double VIEW_WIDTH;
extern double VIEW_HEIGHT;

extern Vector2d G; // external (gravitational) forces
extern Vector2d particlePVelocity; // Velocidad la particula seleccionada
extern Vector2d particlePForce; // Fuerza total de la particula seleccionada
extern Vector2d particlePFgrav; // Fuerza de gravedad de la particula seleccionada
extern Vector2d particlePFvisc; // Fuerza a causa de la viscocidad de la particula seleccionada
extern Vector2d particlePFpress; // Fuerza a causa de la presión de la particula seleccionada

#endif // GLOBALVARIABLES_H
