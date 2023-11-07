#include "openglsimulation.h"
#include "ui_mainwindow.h"

OpenGLSimulation::OpenGLSimulation(QWidget *parent)
    :QOpenGLWidget { parent }
{
    setMouseTracking(true); // Se habilita la seguimiento del ratón
}

using namespace std;
using namespace Eigen;

// "Particle-Based Fluid Simulation for Interactive Applications" by Müller et al.
// solver parameters
const static Vector2d G(0.f, -10.0f);   // external (gravitational) forces
const static float REST_DENS = 300.f;  // rest density
const static float GAS_CONST = 2000.f; // const for equation of state
const static float H = 16.f;		   // kernel radius
const static float HSQ = H * H;		   // radius^2 for optimization
const static float MASS = 2.5f;		   // assume all particles have the same mass
const static float VISC = 200.f;	   // viscosity constant
const static float DT = 0.0007f;	   // integration timestep

// smoothing kernels defined in Müller and their gradients
// adapted to 2D per "SPH Based Shallow Water Simulation" by Solenthaler et al.
// H es el radio del kernel, la unidad resultante es la unidad del radio del kernel
const static float POLY6 = 4.f / (M_PI * pow(H, 8.f)); // 4/(π * H⁸): Estima la densidad del fluido en función de la distancia con otras particulas
const static float SPIKY_GRAD = -10.f / (M_PI * pow(H, 5.f)); // -10/(π * H⁵): Ayuda a calcular las fuerzas que actuan sobre una particula
const static float VISC_LAP = 40.f / (M_PI * pow(H, 5.f)); // 40/(π * H⁵): Se utiliza para calculos relacionados con la densidad del fluido

// simulation parameters
const static float EPS = H / 2; // Será la distancia de colición de la particula con los bordes
const static float BOUND_DAMPING = -0.5f; // variable que modificará la dirección de velocidad cuando la particula toca un borde, al ser menor que 1 esta disminuye en magnitud

    /*struct Particle
    {
        Particle(float _x, float _y) : x(_x, _y), v(0.f, 0.f), f(0.f, 0.f), rho(0), p(0.f) {} // particle data structure
        Vector2d x, v, f; // stores position, velocity, and force for integration
        float rho, p; // stores density (rho) and pressure values for SPH
    };*/

// solver data
static vector<Particle> particles; // Se daclara un vector de tipo Particle llamado particles


// ------------------------------ Variables Globales ------------------------------ //

extern Ui::MainWindow* globalUi; // Conección con la ui de mainwindow
extern int herramientaSeleccionada; // Herramienta seleccionada por el usuario

int fps = 0; // Contador de fps
time_t timer = time(0);

Particle* particlePointer = nullptr; // Puntero que almacenará la dirección de memoria de una particula
bool particleSelected = false; // Variable que sabe si hay una particula seleccionada o no

Vector2d particlePVelocity; // Velocidad la particula seleccionada
Vector2d particlePForce; // Fuerza total de la particula seleccionada
Vector2d particlePFgrav; // Fuerza de gravedad de la particula seleccionada
Vector2d particlePFvisc; // Fuerza a causa de la viscocidad de la particula seleccionada
Vector2d particlePFpress; // Fuerza a causa de la presión de la particula seleccionada

Vector2d pmz(30.f, 30.f); // pmz: particle monitor zoom

//Variables para rastrear movimiento del mouse
bool mPressed = false; //para saber si el mouse esta presionado o no
QPoint mousePressPos; //Posicion de donde se presionó el mouse
QPoint mouseReleasePos; //Posicion de donde se soltó el mouse

// interaction
const static int MAX_PARTICLES = 2500; // Particulas máximas
const static int DAM_PARTICLES = 10; // Particulas generadas
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
        //#pragma omp parallel for
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
        //#pragma omp parallel for
    for (auto &pi : particles) // primer iterador pi
    {
        pi.rho = 0.f; // presión de la particula = 0
        for (auto &pj : particles) // segundo iterador pj
        {
            Vector2d rij = pj.x - pi.x; // Se obtiene el vector entre las posiciones de cada particula, i y j son subindices respecto a su iterador
            float r2 = rij.squaredNorm(); // se calcula la magnitud cuadrada del vector (no se le saca raíz)


            if (r2 < HSQ) /* Se pregunnta si la distancia de las particulas es menor que el radio del kernel, es decir
si una particula está sobre otra  */
            {

                // this computation is symmetric
                pi.rho += MASS * POLY6 * pow(HSQ - r2, 3.f); /* Se cálcula la densidad del punto en el que se
encuentra la particula, POLY6 normaliza la densidad*/
                // printf("\n\nMASS: %f\nPOLY6: %f \nPOW: %f \nrho: %f", MASS, POLY6, pow(HSQ - r2, 3.f), pi.rho);
            }
        }
        pi.p = GAS_CONST * (pi.rho - REST_DENS); /* Se cálcula la presión de la particula despues de sumar
        el efecto de todas las particulas pj sobre pi, esta es una magnitud escalar*/
    }
}

// Esta función calcula las fuerzas que actúan sobre cada partícula.
void OpenGLSimulation::ComputeForces(void)
{
        //#pragma omp parallel for
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

            // -------------------------- Cálculo de fuerzas -------------------------- //
            if (r < H) // Se pregunta si la norma r es menor al radio del kernel para saber si se tienen que calcular o no
            {
                // compute pressure force contribution
                fpress += -rij.normalized() * MASS * (pi.p + pj.p) / (2.f * pj.rho) * SPIKY_GRAD * pow(H - r, 3.f); /*
-rij.normalized() es el vector unitario en dirección opuesta a rij ** fpress es la fuerza a la que se ve sometida una particula
al estar en contacto con otra, siendo esta una fuerza de repulsión ** */
                // compute viscosity force contribution
                fvisc += VISC * MASS * (pj.v - pi.v) / pj.rho * VISC_LAP * (H - r); /* Fuerza ejercida sobre pi por pj junto con otras variables fijas.
se resta **la diferencia de velocidad entre pj y pi para determinar la magnitud y dirección de fvisc** */
            }
        }
        Vector2d fgrav = G * MASS / pi.rho; // La fuerza de gravedad por la masa partido entre la desnsidad de particulas el punto de la particula pi
        pi.f = fpress + fvisc + fgrav; // Es la sumatoria de la fuerza de presión, viscocidad y gravedad

        // Se normalizan las fuerzas que se mostrarán en pantalla
        if(&pi == particlePointer){ // Si la particula iterada es la particula seleccionada guardar las fuerzas
            particlePVelocity = norMagnitude(pi.v, pi.rho);
            particlePForce = norMagnitude(pi.f, pi.rho);
            particlePFgrav = norMagnitude(fgrav, pi.rho);
            particlePFpress = norMagnitude(fpress, pi.rho);
            particlePFvisc = norMagnitude(fvisc, pi.rho);
        }
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
    }
    glEnd();

    glBegin(GL_POINTS);
    qColorToRGB(Qt::yellow, r, g, b); // Se convierte el color blue de QT a rgb
    glColor3f(r, g, b); // Define el color de los puntos
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
        particles.push_back(Particle(e->x(), height() - e->y()));
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

// -------------------------------------------- MAGNITUDES -------------------------------------------- //

Vector2d OpenGLSimulation::norMagnitude(Vector2d f, float rho){ // Recibe una fuerza y la densidad y la convierte
    return f * rho;
}

void OpenGLSimulation::modUiData(){ // Esta función modifica las casillas de ui con los nuevos valores de los datos
    globalUi->velocity_0->setText(QString::number(particlePVelocity(0), 'f', 2));
    globalUi->velocity_1->setText(QString::number(particlePVelocity(1), 'f', 2));

    globalUi->total_force_0->setText(QString::number(particlePForce(0), 'f', 2));
    globalUi->total_force_1->setText(QString::number(particlePForce(1), 'f', 2));

    globalUi->gravity_0->setText(QString::number(particlePFgrav(0), 'f', 2));
    globalUi->gravity_1->setText(QString::number(particlePFgrav(1), 'f', 2));

    globalUi->press_0->setText(QString::number(particlePFpress(0), 'f', 2));
    globalUi->press_1->setText(QString::number(particlePFpress(1), 'f', 2));

    globalUi->viscocity_0->setText(QString::number(particlePFvisc(0), 'f', 2));
    globalUi->viscocity_1->setText(QString::number(particlePFvisc(1), 'f', 2));

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

