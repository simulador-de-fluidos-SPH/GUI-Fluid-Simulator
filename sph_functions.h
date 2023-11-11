#ifndef SPH_FUNCTIONS_H
#define SPH_FUNCTIONS_H

#include <globalVariables.h>

#include <QDebug>
#include <Eigen/Dense>
#include <vector>
#include <random>

Vector2d norMagnitude(Vector2d f, float rho);
void InitSPH();
void Integrate(void);
void ComputeDensityPressure(void);
void ComputeForces(void);

#endif // SPH_FUNCTIONS_H
