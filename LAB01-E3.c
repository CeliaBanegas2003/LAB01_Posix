#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define VECTOR_SIZE 20    // Definimos el tamaño del vector como una constante
#define NUM_THREADS 4     // Definimos el número de hilos que vamos a usar

int vector[VECTOR_SIZE] = {6, 2, 6, 7, 6, 9, 3, 4, 9, 6, 0, 6, 7, 9, 6, 0, 6, 2, 5, 2}; 
// Este es el vector que contiene los números donde se buscarán las ocurrencias del número objetivo

int contador = 0;    // Esta variable almacenará el total de ocurrencias del número objetivo
pthread_mutex_t mutex;   // Declaramos un mutex para proteger el acceso concurrente a la variable "contador"

// Definimos una estructura que servirá para pasar los datos a cada hilo
struct thread_data {
    int inicio;   // Índice de inicio de la porción del vector que procesará cada hilo
    int final;     // Índice de final de la porción del vector que procesará cada hilo
    int num;  // El número objetivo que estamos buscando en el vector
};

// Esta es la función que ejecutará cada hilo
void *count_occurrences(void *threadarg) {
    struct thread_data *data = (struct thread_data *) threadarg;   // Convertimos el argumento de tipo void* a nuestra estructura
    int local_count = 0;   // Variable local para contar las ocurrencias en la porción del vector que procesará el hilo

    // Recorremos la parte del vector asignada al hilo
    for (int i = data->inicio; i < data->final; i++) {
        if (vector[i] == data->num) {
            local_count++;  // Si encontramos el número objetivo, incrementamos el contador local
        }
    }

    // Sección crítica: bloqueamos el acceso al contador global para actualizarlo
    pthread_mutex_lock(&mutex);
    contador += local_count;  // Sumamos las ocurrencias locales al contador global
    pthread_mutex_unlock(&mutex);
    // Fin de la sección crítica

    pthread_exit(NULL);   // Terminamos la ejecución del hilo
}

int main() {
    pthread_t threads[NUM_THREADS];   // Vector para almacenar los identificadores de los hilos
    struct thread_data thread_data_array[NUM_THREADS];  // Vector para los datos de cada hilo
    int segment_size = VECTOR_SIZE / NUM_THREADS;   // Calculamos el tamaño de la porción de vector que procesará cada hilo
    int num_obj = 6;  // El número objetivo que vamos a buscar en el vector

    // Inicializamos el mutex
    pthread_mutex_init(&mutex, NULL);

    // Creamos los hilos, asignando a cada uno una porción del vector
    for (int t = 0; t < NUM_THREADS; t++) {
        thread_data_array[t].inicio = t * segment_size;  // Definimos el índice de comienzo de la sección asignada al hilo.
        thread_data_array[t].final = (t == NUM_THREADS - 1) ? VECTOR_SIZE : (t + 1) * segment_size; 
        // El último hilo puede tener más elementos si el vector no es divisible exactamente
        thread_data_array[t].num = num_obj;  // Pasamos el número objetivo

        // Creamos el hilo y le pasamos los datos de su porción del vector
        pthread_create(&threads[t], NULL, count_occurrences, (void *)&thread_data_array[t]); 
        
    }

    // Esperamos a que todos los hilos terminen
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);   // Nos aseguramos de que cada hilo haya completado su trabajo
    }

    // Destruimos el mutex ya que no lo necesitamos más
    pthread_mutex_destroy(&mutex);

    // Imprimimos el resultado final
    printf("Total occurrences of %d: %d\n", num_obj, contador);

    return 0;
}
