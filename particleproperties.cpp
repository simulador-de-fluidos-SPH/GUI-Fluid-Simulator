#include "particleproperties.h"

FluidProperties::FluidProperties() {
    // Inicializa las propiedades del fluido con valores predeterminados
    REST_DENS = 300.f;
    GAS_CONST = 2000.f;
    H = 16.f;
    HSQ = H * H;
    MASS = 2.5f;
    VISC = 200.f;
    DT = 0.0007f;

}

void FluidProperties::setWaterProperties() {
    REST_DENS = 300.f;
    GAS_CONST = 2000.f;
    H = 16.f;
    HSQ = H * H;
    MASS = 2.5f;
    VISC = 200.f;
    DT = 0.0007f;
}

void FluidProperties::setOilProperties() {
    //propiedades para simular aceite
    REST_DENS = 250.f;
    GAS_CONST = 1500.f;
    H = 15.f;
    HSQ = H * H;
    MASS = 3.f;
    VISC = 400.f;
    DT = 0.001f;
}

void FluidProperties::setResinProperties() {
    // propiedades para simular resina
    REST_DENS = 400.f;
    GAS_CONST = 1500.f;
    H = 18.f;
    HSQ = H * H;
    MASS = 3.f;
    VISC = 300.f;
    DT = 0.00048f;
}
void FluidProperties::setAlcoholProperties() {
    //propiedades para simular alcohol
    REST_DENS = 200.f;
    GAS_CONST = 1200.f;
    H = 15.f;
    HSQ = H * H;
    MASS = 1.5f;
    VISC = 200.f;
    DT = 0.00075f;
}
void FluidProperties::setMagmaProperties() {
    // Cambia las propiedades para simular magma
    REST_DENS = 800.f;
    GAS_CONST = 1500.f;
    H = 20.f;
    HSQ = H * H;
    MASS = 5.f;
    VISC = 1000.f;
    DT = 0.0001f;
}
void FluidProperties::setMilkProperties() {
    //propiedades para simular leche
    REST_DENS = 320.f;
    GAS_CONST = 1800.f;
    H = 19.f;
    HSQ = H * H;
    MASS = 2.5f;
    VISC = 150.f;
    DT = 0.00063f;
}
void FluidProperties::setGlueProperties() {
    //propiedades para simular pegamento
    REST_DENS = 310.f;
    GAS_CONST = 1600.f;
    H = 17.f;
    HSQ = H * H;
    MASS = 3.f;
    VISC = 250.f;
    DT = 0.00033f;
}
void FluidProperties::setQuickSandProperties() {
    //propiedades para simular arenas movedizas
    REST_DENS = 400.f;
    GAS_CONST = 1400.f;
    H = 22.f;
    HSQ = H * H;
    MASS = 4.f;
    VISC = 350.f;
    DT = 0.0002f;
}
void FluidProperties::setBloodProperties() {
    //propiedades para simular sangre
    REST_DENS = 330.f;
    GAS_CONST = 1700.f;
    H = 19.5f;
    HSQ = H * H;
    MASS = 2.5f;
    VISC = 180.f;
    DT = 0.0006f;
}
void FluidProperties::setHoneyProperties() {
    //propiedades para simular miel
    REST_DENS = 390.f;
    GAS_CONST = 1900.f;
    H = 21.f;
    HSQ = H * H;
    MASS = 3.f;
    VISC = 220.f;
    DT = 0.00036f;
}
