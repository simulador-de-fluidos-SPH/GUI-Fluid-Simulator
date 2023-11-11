#include "ui_functions.h"
#include "globalVariables.h"

void modUiData(){
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
