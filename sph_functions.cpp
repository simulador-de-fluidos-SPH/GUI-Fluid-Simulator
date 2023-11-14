#include "ui_mainwindow.h"

#include "sph_functions.h"
#include "ui_functions.h"

using namespace std;
using namespace Eigen;

Vector2d norMagnitude(Vector2d f, float rho){ // Recibe una fuerza y la densidad y la convierte
    return f * rho;
}

// Función para inicializar las particulas
void InitSPH()
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
                newParticle(x + jitter, y, globalUi->Fluid_Menu->currentIndex());
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
void ComputeDensityPressure(void)
{
    #pragma omp parallel for
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
                pi.rho += pi.MASS * POLY6 * pow(HSQ - r2, 3.f); /* Se cálcula la densidad del punto en el que se
encuentra la particula, POLY6 normaliza la densidad*/
                // printf("\n\nMASS: %f\nPOLY6: %f \nPOW: %f \nrho: %f", MASS, POLY6, pow(HSQ - r2, 3.f), pi.rho);
        }
    }
    pi.p = pi.GAS_CONST * (pi.rho - pi.REST_DENS); /* Se cálcula la presión de la particula despues de sumar
        el efecto de todas las particulas pj sobre pi, esta es una magnitud escalar*/
}
}

// Esta función calcula las fuerzas que actúan sobre cada partícula.
void ComputeForces()
{
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

            // -------------------------- Cálculo de fuerzas -------------------------- //
            if (r < H) // Se pregunta si la norma r es menor al radio del kernel para saber si se tienen que calcular o no
            {
                // compute pressure force contribution
                fpress += -rij.normalized() * pi.MASS * (pi.p + pj.p) / (2.f * pj.rho) * SPIKY_GRAD * pow(H - r, 3.f); /*
-rij.normalized() es el vector unitario en dirección opuesta a rij ** fpress es la fuerza a la que se ve sometida una particula
al estar en contacto con otra, siendo esta una fuerza de repulsión ** */
                // compute viscosity force contribution
                fvisc += pi.VISC * pi.MASS * (pj.v - pi.v) / pj.rho * VISC_LAP * (H - r); /* Fuerza ejercida sobre pi por pj junto con otras variables fijas.
se resta **la diferencia de velocidad entre pj y pi para determinar la magnitud y dirección de fvisc** */
            }
        }
        Vector2d fgrav = G * pi.MASS / pi.rho; // La fuerza de gravedad por la masa partido entre la desnsidad de particulas el punto de la particula pi
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
