#include "openglsimulation.h"

OpenGLSimulation::OpenGLSimulation(QWidget *parent)
    :QOpenGLWidget { parent }
{

}

using namespace std;
using namespace Eigen;

// "Particle-Based Fluid Simulation for Interactive Applications" by Müller et al.
// solver parameters
const static Vector2d G(0.f, -10.f);   // external (gravitational) forces
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
const static float EPS = H; // boundary epsilon
const static float BOUND_DAMPING = -0.5f;

// particle data structure
// stores position, velocity, and force for integration
// stores density (rho) and pressure values for SPH
struct Particle
{
    Particle(float _x, float _y) : x(_x, _y), v(0.f, 0.f), f(0.f, 0.f), rho(0), p(0.f) {}
    Vector2d x, v, f;
    float rho, p;
};

// solver data
static vector<Particle> particles;

// interaction
const static int MAX_PARTICLES = 2500;
const static int DAM_PARTICLES = 500;
const static int BLOCK_PARTICLES = 250;

// rendering projection parameters
const static int WINDOW_WIDTH = 800;
const static int WINDOW_HEIGHT = 600;
const static double VIEW_WIDTH = 1.5 * 800.f;
const static double VIEW_HEIGHT = 1.5 * 600.f;


// función para inicializar las particulas
void InitSPH(void)
{
    cout << "initializing dam break with " << DAM_PARTICLES << " particles" << endl;

    // Crear un generador de números aleatorios utilizando la semilla actual del reloj
    random_device rd;
    mt19937 gen(rd());

    uniform_real_distribution<float> dist(0.0f, 1.0f);

    for (float y = EPS; y < VIEW_HEIGHT - EPS * 2.f; y += H)
    {
        for (float x = VIEW_WIDTH / 4; x <= VIEW_WIDTH / 2; x += H)
        {
            if (particles.size() < DAM_PARTICLES)
            {
                cout << particles.size(); // Linea de prueba
                // Generar un valor aleatorio en el rango [0.0, 1.0]
                float jitter = dist(gen);
                particles.push_back(Particle(x + jitter, y));
            }
            else
            {
                return;
            }
        }
    }
}

void Integrate(void)
{
    //	#pragma omp parallel for
    for (auto &p : particles)
    {
        // forward Euler integration
        p.v += DT * p.f / p.rho;
        p.x += DT * p.v;

        // enforce boundary conditions
        if (p.x(0) - EPS < 0.f)
        {
            p.v(0) *= BOUND_DAMPING;
            p.x(0) = EPS;
        }
        if (p.x(0) + EPS > VIEW_WIDTH)
        {
            p.v(0) *= BOUND_DAMPING;
            p.x(0) = VIEW_WIDTH - EPS;
        }
        if (p.x(1) - EPS < 0.f)
        {
            p.v(1) *= BOUND_DAMPING;
            p.x(1) = EPS;
        }
        if (p.x(1) + EPS > VIEW_HEIGHT)
        {
            p.v(1) *= BOUND_DAMPING;
            p.x(1) = VIEW_HEIGHT - EPS;
        }
    }
}

// Esta función calcula la densidad y la presión para cada partícula en base las particulas de su alrededor.
void ComputeDensityPressure(void)
{
    //	#pragma omp parallel for
    for (auto &pi : particles)
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
void ComputeForces(void)
{
    //	#pragma omp parallel for
    for (auto &pi : particles)
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
        Vector2d fgrav = G * MASS / pi.rho;
        pi.f = fpress + fvisc + fgrav;
    }
}

void OpenGLSimulation::Update(void) const
{
    ComputeDensityPressure();
    ComputeForces();
    Integrate();

    //glutPostRedisplay();

}

/*
void InitGL(void)
{
    glClearColor(0.9f, 0.9f, 0.9f, 1);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(H / 2.f);
    glMatrixMode(GL_PROJECTION);
}
*/

// Inicializa el opengl
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

    glBegin(GL_POINTS); // Se inicializa la creación de puntos GL_POINTS
    for (auto &p : particles)
    {
        glVertex2f(p.x(0), p.x(1));
    }
    glEnd();

//    SwapBuffers();

    // glEnable(GL_POINT_SMOOTH);
    glPointSize(30.0f);
    glBegin(GL_POINTS);
    qColorToRGB(Qt::yellow, r, g, b);
    glColor3f(r, g, b); // Define color
    glVertex2f(0.2f, 0.9f);
    glEnd();

    update();

    glFlush();


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

