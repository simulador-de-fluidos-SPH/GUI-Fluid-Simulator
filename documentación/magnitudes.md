# Magnitudes #
En la simulación, las magnitudes tanto de velocidad como de fuerza, no usan un sistema estandar y son dispares con el sistema internacional
(kg, m, m/s...) por lo tanto, para ser más amigables con el usuario, intentamos convertir estas unidades a magnitudes mesurables.

## Conversión de magnitud de simulación a magnitud mesurable ##
Existe una relación entre N y v; N = kg*a y v = a*t. Sabemos que fg = m*g, esta es una fuerza presente en la simulación, pero se cálcula
como fg = m*g/rho, esto a causa de que __rho es densidad__ y la densidad es importante en el cálculo de la simulación. Sin embargo, si queremos
una magnitud más amigable podemos multiplicar __fg__ por el escalar __rho__ para obtener una magnitud de velocidad. Sabiendo esto, al ser rho
un escalar, podemos multiplicar todos los vectores tanto de fuerza como velocidad por rho, de esta manera obtenemos magnitudes mesurables.

## rho ##
rho (densidad) se calcula siempre que una particula entra en contacto con otra, sin embargo rho no es nula cuando la particula no está en contacto
con otras, lo que descarta la división y multiplicación por rho=0 en los cálculos donde se usa.

__rho = MASS * POLY6 * pow(HSQ, 3.f)__ cuando la particula no tiene contacto con vencinos. Sacado de "openglsimulation.cpp/ComputeDensityPressure()"
