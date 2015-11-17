#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

/**
 * P7: implemente un monitor (c++), que permite agregar/quitar elementos de una cola.
 *
 * Compilar con:
 *     g++ -o p7 -lpthread p7.cpp
 */

#define MAX_ELEMENTOS 100

/**
 * Cola cuyo push y pop pueden ser llamados desde diferentes threads. Ademas se
 * puede implementar con semaforos, para hacer bloqueos al momento de insertar
 * y no haya espacio, o al remover y no hayan elementos.
 */
class ColaMonitor {
private:
    int elementos[MAX_ELEMENTOS];
    int inicio, cantidad;

    pthread_mutex_t lock; // proteje a: elementos, inicio y cantidad

public:
    ColaMonitor() {
        inicio = 0;
        cantidad = 0;
        // Constructor inicializa el mutex
        if (pthread_mutex_init(&lock, NULL) != 0) {
            exit(1);
        }
    }

    ~ColaMonitor() {
        // Destructor libera el mutex
        pthread_mutex_destroy(&lock);
    }

    bool push(int elemento) {
        bool pudoAgregar = false;
        pthread_mutex_lock(&lock);
        // ------ inicio de seccion critica ------
        if (cantidad < MAX_ELEMENTOS) {
            elementos[(inicio + cantidad) % MAX_ELEMENTOS] = elemento;
            cantidad++;
            pudoAgregar = true;
        }
        // ------ fin de seccion critica ------
        pthread_mutex_unlock(&lock);

        return pudoAgregar;
    }

    bool pop(int &elemento) {
        bool pudoRemover = false;
        pthread_mutex_lock(&lock);
        // ------ inicio de seccion critica ------
        if (cantidad > 0) {
            elemento = elementos[inicio];
            cantidad--;
            inicio = (inicio + 1) % MAX_ELEMENTOS;
            pudoRemover = true;
        }
        // ------ fin de seccion critica ------
        pthread_mutex_unlock(&lock);
        return pudoRemover;
    }
};

int main() {

    ColaMonitor cola; // Constructor automaticamente al declarar cola.

    cola.push(67);
    cola.push(78);
    cola.push(92);
    cola.push(13); // cola.push esta sincronizado, por lo tanto es thread-safe.

    int x;        // x debe ser variable local al thread que llama a .pop
    if (cola.pop(x)) printf("x: %d\n", x);
    // x contiene ahora el siguiente valor de la cola

    if (cola.pop(x)) printf("x: %d\n", x);

    if (cola.pop(x)) printf("x: %d\n", x);

    if (cola.pop(x)) printf("x: %d\n", x);

    cola.push(56);

    if (cola.pop(x)) printf("x: %d\n", x);

    return 0; // Destructor llamado automaticamente al salir del scope.
}
