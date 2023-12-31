#include "particleproperties.h"
#include "globalVariables.h"

/* properties:

    RED
    GREEN
    BLUE
    REST_DENS
    GAS_CONST
    MASS
    VISC
    DT
*/

Data props[] = {
    {0.30980f, 0.25882f, 0.70980f, 300.f, 2000.f, 2.5f, 200.f, 0.0007f},   // Default
    {0.83137f, 0.94510f, 0.97647f, 300.f, 2000.f, 2.5f, 200.f, 0.0007f},   // Water
    {0.85882f, 0.81176f, 0.36078f, 250.f, 1500.f, 3.f, 400.f, 0.001f},     // Oil
    {0.6f, 0.6f, 0.6f, 400.f, 1500.f, 3.f, 300.f, 0.00048f},               // Resin
    {0.58824f, 0.10588f, 0.09804f, 200.f, 1200.f, 1.5f, 200.f, 0.00075f},  // Alcohol
    {0.81176f, 0.06275f, 0.12549f, 800.f, 1500.f, 5.f, 1000.f, 0.0001f},   // Magma
    {0.69216f, 0.70000f, 0.66078f, 320.f, 1800.f, 2.5f, 150.f, 0.00063f},  // Milk
    {0.99608f, 0.98824f, 0.98039f, 310.f, 1600.f, 3.f, 250.f, 0.00033f},   // Glue
    {0.76078f, 0.69804f, 0.50196f, 400.f, 1400.f, 4.f, 350.f, 0.0002f},    // QuickSand
    {0.54118f, 0.01176f, 0.01176f, 330.f, 1700.f, 2.5f, 180.f, 0.0006f},   // Blood
    {0.92157f, 0.66275f, 0.21569f, 390.f, 1900.f, 3.f, 220.f, 0.00036f}    // Honey
};
