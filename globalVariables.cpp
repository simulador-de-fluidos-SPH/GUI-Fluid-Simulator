#include "globalVariables.h"

using namespace Eigen;

// En este archivo se inicializan las variables globales



vector<Particle> particles;

Particle* particlePointer = nullptr;

Ui::MainWindow *globalUi;
int herramientaSeleccionada = 0;
bool particleSelected = false;
Vector2d pmz(30.f, 30.f);

float REST_DENS = 300.f;
float GAS_CONST = 2000.f;
float H = 16.f;
float HSQ = H * H;
float MASS = 2.5f;
float VISC = 200.f;
float DT = 0.0007f;

float r = 0.30980;
float g = 0.25882;
float b = 0.70980;

float EPS = H / 2;
float BOUND_DAMPING = -0.5f;

float POLY6 = 4.f / (M_PI * pow(H, 8.f));
float SPIKY_GRAD = -10.f / (M_PI * pow(H, 5.f));
float VISC_LAP = 40.f / (M_PI * pow(H, 5.f));

int MAX_PARTICLES = 2500;
long unsigned int DAM_PARTICLES = 10;
int BLOCK_PARTICLES = 250;

int WINDOW_WIDTH = 750;
int WINDOW_HEIGHT = 400;
double VIEW_WIDTH = 750;
double VIEW_HEIGHT = 400;

Vector2d G(0.f, -9.8f);
Vector2d particlePVelocity;
Vector2d particlePForce;
Vector2d particlePFgrav;
Vector2d particlePFvisc;
Vector2d particlePFpress;