#include "mbed.h"
#include <chrono>
#include <cstdlib> // Para la función rand()

// Pines y puertos
DigitalOut Ds(D7);     // Pin de datos (Data)
DigitalOut STCP(D8);   // Pin de latch (Latch)
DigitalOut SHCP(D9);   // Pin de reloj de desplazamiento (Shift Clock)

// Constantes
#define TIEMPO_BARRIDO 100ms // Tiempo de retardo entre barridos
#define TIEMPO_PARPADEO 100ms // Tiempo entre cambios de estado de LEDs en el patrón de ruido blanco
#define TIEMPO_PAUSA 200ms // Tiempo de pausa antes de repetir el patrón

// Prototipo de función
void enviar_datos(void);

// Crear hilo para enviar datos
Thread hilo_enviar_datos;

// Función principal
int main() {
    // Inicializar pines
    Ds = 0;
    STCP = 0;
    SHCP = 0;

    // Inicializar la función rand() para generar números aleatorios
    std::srand(std::time(nullptr));

    // Iniciar el hilo para enviar datos
    hilo_enviar_datos.start(enviar_datos);

    // Mantener el programa en ejecución
    while (true) {
        ThisThread::sleep_for(1s); 
    }
}

// Función para enviar datos a los registros de desplazamiento
void enviar_datos(void) {
    while (true) {
        // Barrido de izquierda a derecha
        for (uint8_t i = 0; i < 8; i++) {
            uint8_t data = 1 << i; // Desplazar un bit hacia la derecha
            for (uint8_t j = 0; j < 8; j++) { // Enviar bits del dato
                SHCP = 0; // Preparar el reloj de desplazamiento
                Ds = (data >> j) & 0x01; // Enviar el bit correspondiente
                SHCP = 1; // Enviar el bit
            }
            STCP = 1; // Latch los datos en los registros de desplazamiento
            STCP = 0;
            ThisThread::sleep_for(TIEMPO_BARRIDO); // Espera entre actualizaciones
        }

        // Barrido de derecha a izquierda
        for (int8_t i = 7; i >= 0; i--) {
            uint8_t data = 1 << i; // Desplazar un bit hacia la izquierda
            for (uint8_t j = 0; j < 8; j++) { // Enviar bits del dato
                SHCP = 0; // Preparar el reloj de desplazamiento
                Ds = (data >> j) & 0x01; // Enviar el bit correspondiente
                SHCP = 1; // Enviar el bit
            }
            STCP = 1; // Latch los datos en los registros de desplazamiento
            STCP = 0;
            ThisThread::sleep_for(TIEMPO_BARRIDO); // Espera entre actualizaciones
        }

        // Crear un patrón blanco
        for (int i = 0; i < 10; i++) { // Repetir el patrón 10 veces
            uint8_t data = 0;
            for (uint8_t j = 0; j < 8; j++) {
                // Generar un valor aleatorio para cada LED (encendido o apagado)
                data |= (std::rand() % 2) << j;
            }
            for (uint8_t k = 0; k < 8; k++) { // Enviar bits del dato
                SHCP = 0; // Preparar el reloj de desplazamiento
                Ds = (data >> k) & 0x01; // Enviar el bit correspondiente
                SHCP = 1; // Enviar el bit
            }
            STCP = 1; // Latch los datos en los registros de desplazamiento
            STCP = 0;
            ThisThread::sleep_for(TIEMPO_PARPADEO); // Tiempo entre cambios de estado de LEDs
        }

        // Pausa antes de repetir el patrón
        ThisThread::sleep_for(TIEMPO_PAUSA);
    }
}
