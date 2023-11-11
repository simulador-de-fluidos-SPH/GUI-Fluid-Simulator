#include "toolsfunctions.h"

// Función para seleccionar una particula y devuelve la dirección de memoria de la particula seleccionada como dirección void no Particle
void* selectParticle(QMouseEvent *e){

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
void deleteParticle(QMouseEvent *e){
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
void particlePointerSetter(QMouseEvent *e){

    Particle* localPointer = (Particle*)selectParticle(e);

    if(localPointer == nullptr){
        particleSelected = false;
    } else {
        particleSelected = true;
    }

    particlePointer = localPointer;
}
