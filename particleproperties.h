#ifndef PARTICLEPROPERTIES_H
#define PARTICLEPROPERTIES_H

#include <Eigen/Dense>

class FluidProperties {
public:
    float REST_DENS;
    float GAS_CONST;
    float H;
    float HSQ;
    float MASS;
    float VISC;
    float DT;

    void setWaterProperties();
    void setOilProperties();
    void setResinProperties();
    void setAlcoholProperties();
    void setMagmaProperties();
    void setMilkProperties();
    void setGlueProperties();
    void setQuickSandProperties();
    void setBloodProperties();
    void setHoneyProperties();
    // Constructor
    FluidProperties();
};

#endif // PARTICLEPROPERTIES_H
