# MSE_CO04_ISO

# Implementación de sistemas operativos

# Para la presentación del examen:
# Teclas funcionales TEC1 y TEC 2 según distribución de la edu-ciaa
# Resolución de tiempo 1ms

#Descripción:
# El sistema cuenta con dos tares, la primera encargada del barrido de las teclas mediante una máquina de estados,
# la captura de los tiempos de los flancos (para tal propósito se corre un contador de sistema actualizado en cada tick),
# también encargada de establecer la secuencia lógica de dos flancos de bajada y dos flancos de subida en orden estricto mediante
# una máquina de estados.
# En caso de no cumplirse la secuencia, se reinicia la máquina descartando cualquier dato recogido hasta el momento.

# En caso de encontrar una secuencia correcta se habilita una segunda tarea.

#La segunda tarea determina, según los tiempos recogidos, el correspondiente caso, el tiempo de encendido,el led a encender
# y el mensaje en consola.
#Terminada la secuencia se bloquea en espera de una nueva secuencia.

