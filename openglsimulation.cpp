#include "openglsimulation.h"

OpenGLSimulation::OpenGLSimulation(QWidget *parent)
    :QOpenGLWidget { parent }
{
    setMouseTracking(true); // Se habilita la seguimiento del ratón
}

using namespace std;
using namespace Eigen;

// "Particle-Based Fluid Simulation for Interactive Applications" by Müller et al.
// solver parameters
const static Vector2d G(0.f, -9.8f);   // external (gravitational) forces
const static float REST_DENS = 300.f;  // rest density
const static float GAS_CONST = 2000.f; // const for equation of state
const static float H = 16.f;		   // kernel radius
const static float HSQ = H * H;		   // radius^2 for optimization
const static float MASS = 2.5f;		   // assume all particles have the same mass
const static float VISC = 200.f;	   // viscosity constant
const static float DT = 0.0007f;	   // integration timestep

// smoothing kernels defined in Müller and their gradients
// adapted to 2D per "SPH Based Shallow Water Simulation" by Solenthaler et al.
const static float POLY6 = 4.f / (M_PI * pow(H, 8.f));
const static float SPIKY_GRAD = -10.f / (M_PI * pow(H, 5.f));
const static float VISC_LAP = 40.f / (M_PI * pow(H, 5.f));

// simulation parameters
const static float EPS = H / 2; // Será la distancia de colición de la particula con los bordes
const static float BOUND_DAMPING = -0.5f; // variable que modificará la dirección de velocidad cuando la particula toca un borde, al ser menor que 1 esta disminuye en magnitud

    struct Particle
    {
        Particle(float _x, float _y) : x(_x, _y), v(0.f, 0.f), f(0.f, 0.f), rho(0), p(0.f) {} // particle data structure
        Vector2d x, v, f; // stores position, velocity, and force for integration
        float rho, p; // stores density (rho) and pressure values for SPH
    };

// solver data
static vector<Particle> particles; // Se daclara un vector de tipo Particle llamado particles

Particle* particlePointer = nullptr; // Puntero que almacenará la dirección de memoria de una particula
bool particleSelected = false; // Variable que sabe si hay una particula seleccionada o no

// interaction
const static int MAX_PARTICLES = 2500; // Particulas máximas
const static int DAM_PARTICLES = 100; // Particulas generadas
const static int BLOCK_PARTICLES = 250; // Particulas generables por click

// rendering projection parameters
int WINDOW_WIDTH = 750;
int WINDOW_HEIGHT = 400;
double VIEW_WIDTH = 750;
double VIEW_HEIGHT = 400;


// Función para inicializar las particulas
void OpenGLSimulation::InitSPH()
{
    qDebug() << "Initializing simulation with" << DAM_PARTICLES << "particles\n";

    // Crear un generador de números aleatorios utilizando la semilla actual del reloj
    random_device rd;
    mt19937 gen(rd());

    uniform_real_distribution<float> dist(0.0f, 1.0f); // Establece el rango de generación de los números aleatorios

    // Ambos for loop sirven para controlar donde aparecen las particulas en el plano
    for (float y = EPS; y < VIEW_HEIGHT - EPS * 2.f; y += H) // Posición del conjunto de particulas en y
    {
        for (float x = VIEW_WIDTH / 4; x <= VIEW_WIDTH / 2; x += H) // Posición del conjunto de particulas en x
        {
            if (particles.size() < DAM_PARTICLES) // Verifica si la cantidad de particulas es menor que DAM_PARTICLES
            {
                float jitter = dist(gen); // Generar un valor aleatorio en el rango [0.0, 1.0] para introducir una variación de posición
                particles.push_back(Particle(x + jitter, y)); // Se crea un nuevo Particle y se añade al vector particles (x + jitter introduce la variación en x)
            }
            else
            {
                return;
            }
        }
    }
}

void OpenGLSimulation::Integrate(void)
{
        #pragma omp parallel for
    for (auto &p : particles) // Recorre las particulas del vector particles
    {
        // forward Euler integration
        p.v += DT * p.f / p.rho;
        p.x += DT * p.v;

        // Condiciones de las particulas al llegar a los bordes
        if (p.x(0) - EPS < 0.f) // Colición cor borde izquierdo
        {
            p.v(0) *= BOUND_DAMPING; /* Las componentes (x, y) del vector v se multiplican por el escalar BOUND_DAMPING.
            Si BOUND_DAMPING = -0.5 y v = (x, y) entonces v*BOUND_DAMPING = (-0.5x, -0.5y) */
            p.x(0) = EPS; // Posición x = tamaño de EPS, toca el borde izquierdo
        }
        if (p.x(0) + EPS > VIEW_WIDTH) // Colición con borde derecho
        {
            p.v(0) *= BOUND_DAMPING; // Definición anterior
            p.x(0) = VIEW_WIDTH - EPS; // Posición x = ancho de la ventana menos EPS
        }
        if (p.x(1) - EPS < 0.f) // Colición con borde inferior
        {
            p.v(1) *= BOUND_DAMPING; // Definición anterior
            p.x(1) = EPS; // Posición y = tamaño de EPS, toca el borde inferior
        }
        if (p.x(1) + EPS > VIEW_HEIGHT) // Colición con borde superior
        {
            p.v(1) *= BOUND_DAMPING; // Definición anterior
            p.x(1) = VIEW_HEIGHT - EPS; // Posición y = altura de la ventana menos EPS
        }
    }
}

// Esta función calcula la densidad y la presión para cada partícula en base las particulas de su alrededor.
void OpenGLSimulation::ComputeDensityPressure(void)
{
        #pragma omp parallel for
    for (auto &pi : particles) // Recorre las particulas del vector particles
    {
        pi.rho = 0.f;
        for (auto &pj : particles)
        {
            Vector2d rij = pj.x - pi.x;
            float r2 = rij.squaredNorm();

            if (r2 < HSQ)
            {
                // this computation is symmetric
                pi.rho += MASS * POLY6 * pow(HSQ - r2, 3.f);
            }
        }
        pi.p = GAS_CONST * (pi.rho - REST_DENS);
    }
}

// Esta función calcula las fuerzas que actúan sobre cada partícula.
void OpenGLSimulation::ComputeForces(void)
{
        #pragma omp parallel for
    for (auto &pi : particles) // Recorre las particulas del vector particles
    {
        Vector2d fpress(0.f, 0.f);
        Vector2d fvisc(0.f, 0.f);
        for (auto &pj : particles)
        {
            if (&pi == &pj)
            {
                continue;
            }

            Vector2d rij = pj.x - pi.x;
            float r = rij.norm();

            if (r < H)
            {
                // compute pressure force contribution
                fpress += -rij.normalized() * MASS * (pi.p + pj.p) / (2.f * pj.rho) * SPIKY_GRAD * pow(H - r, 3.f);
                // compute viscosity force contribution
                fvisc += VISC * MASS * (pj.v - pi.v) / pj.rho * VISC_LAP * (H - r);
            }
        }
        Vector2d fgrav = G * MASS / pi.rho; // Fuerza de gravedad * masa partido entre densidad
        pi.f = fpress + fvisc + fgrav;
    }
}

// Inicializa el opengl
void OpenGLSimulation::initializeGL()
{
    float r, g, b, a = normaliza_0_1(255.0f, 1.0f, 255.0f); // Se declaran r, g, b y alfa como 1

    qColorToRGB(Qt::white, r, g, b); // A la función qColor se le pasa el color negro de QColor y se modifica r, g, b
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
    float r, g, b;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Borra los pixeles de la pantalla estableciendolos a el color definido en glClearColor

    // Se llaman las funciones de cálculo de nueva posición
    ComputeDensityPressure();
    ComputeForces();
    Integrate();

    glEnable(GL_POINT_SMOOTH); // Le da forma circular a los puntos

    // glPointSize(0.02f * this->width()); // Ajusta el tamaño de los puntos a el tamaño del openGLWidget
    glPointSize(H); // Ajusta el tamaño de los puntos a H

    glBegin(GL_POINTS); // Se inicializa la creación de puntos GL_POINTS
    qColorToRGB(Qt::blue, r, g, b); // Se convierte el color blue de QT a rgb
    glColor3f(r, g, b); // Define el color de los puntos

    // Ciclo for que renderiza las particulas
    for (auto &p : particles) // Recorre las particulas del vector particles
    {
        if(particlePointer == &p){
            qColorToRGB(Qt::yellow, r, g, b); // Se convierte el color blue de QT a rgb
            glColor3f(r, g, b); // Define el color de los puntos
            glVertex2f(p.x(0), p.x(1)); // Se muestra el vector normalizado en coordenadas norx nory
            qColorToRGB(Qt::blue, r, g, b); // Se convierte el color blue de QT a rgb
            glColor3f(r, g, b); // Define el color de los puntos
        } else {
            glVertex2f(p.x(0), p.x(1)); // Se muestra el vector normalizado en coordenadas norx nory
        }
        // qDebug() << "p.x(0): " << norX << "p.x(1): " << norY << "\n" << width() << " " << height() << "\n";
    }
    glEnd();

    glBegin(GL_POINTS);
    qColorToRGB(Qt::yellow, r, g, b); // Se convierte el color blue de QT a rgb
    glColor3f(r, g, b); // Define el color de los puntos
    glEnd();

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

    /* ################# Bounding interaction implementation #################
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
    VIEW_WIDTH =  static_cast<double>(w);
    VIEW_HEIGHT =  static_cast<double>(h);
    */

    glOrtho(0.0, VIEW_WIDTH, 0.0, VIEW_HEIGHT, -1.0, 1.0); // Configura la cámara para que apunte y tenga el tamaño del openGLWidget

}

// -------------------------------------------- TOOLS -------------------------------------------- //

// Función que se ejecutará cada que el mouse es presionado
void OpenGLSimulation::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) // Verifica que se hizo clic con el botón izquierdo del ratón
    {
        // --------- FUNCIONES DE AL CLICKER --------- //
        particlePointerSetter(e);

        //deleteParticle(e);
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

    particlePointer = localPointer;
}

// -------------------------------------------- COLORS -------------------------------------------- //

// Con esta función se convierte el color de qcolor a rgb
void OpenGLSimulation::qColorToRGB(const QColor &C, float &r, float &g, float &b) const
{
    // Se modifican las variables globales r, g, b a partir de los valores de QColor
    r = normaliza_0_1(C.red(), 1.0f, 255.0f);
    g = normaliza_0_1(C.green(), 1.0f, 255.0f);
    b = normaliza_0_1(C.blue(), 1.0f, 255.0f);
}

// Funciónn auxiliar para normalizar color
float OpenGLSimulation::normaliza_0_1(float val, float min, float max) const
{
    return (val - min) / (max - min);
}

