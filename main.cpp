#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_DEPARTAMENTOS 3
#define NUM_IMPRESORAS 2
#define NUM_ESCANERES 1
#define NUM_PLOTTERS 1

// Recursos disponibles
int impresoras = NUM_IMPRESORAS;
int escaneres = NUM_ESCANERES;
int plotters = NUM_PLOTTERS;

// Mutex y variables de condición para controlar los recursos
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// Función que simula una tarea del departamento
void* tarea_departamento(void* arg) {
    int dept_id = *(int*)arg;
    delete (int*)arg;  

    // Intento de adquirir recursos (impresora y otro recurso)
    pthread_mutex_lock(&mutex);

    printf("Departamento %d intentando adquirir recursos...\n", dept_id);

    // Esperar hasta que haya una impresora y otro recurso disponible
    while (impresoras == 0 || escaneres == 0 || plotters == 0) {
        pthread_cond_wait(&cond, &mutex); // Esperar hasta que se liberen recursos
    }

    // Adquirir recursos
    impresoras--;
    if (dept_id % 2 == 0) {
        escaneres--;
        printf("Departamento %d adquirió impresora y escáner.\n", dept_id);
    } else {
        plotters--;
        printf("Departamento %d adquirió impresora y plotter.\n", dept_id);
    }

    pthread_mutex_unlock(&mutex);

    // Simular tarea de impresión
    sleep(2);

    // Liberar recursos
    pthread_mutex_lock(&mutex);
    impresoras++;
    if (dept_id % 2 == 0) {
        escaneres++;
    } else {
        plotters++;
    }

    printf("Departamento %d liberó los recursos.\n", dept_id);

    pthread_cond_broadcast(&cond); 
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pthread_t departamentos[NUM_DEPARTAMENTOS];

    // Crear hilos que representan a los departamentos
    for (int i = 0; i < NUM_DEPARTAMENTOS; i++) {
        int* dept_id = new int;  
        *dept_id = i + 1; 
        pthread_create(&departamentos[i], NULL, tarea_departamento, dept_id);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < NUM_DEPARTAMENTOS; i++) {
        pthread_join(departamentos[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
