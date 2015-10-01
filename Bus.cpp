#ifndef BUS_CPP
#define BUS_CPP
#include "Memory.cpp"
#include <pthread.h>
#include <stdio.h>
// Clase intermediaria utilizada por los diferentes caches para extraer informacion de la memoria
// Contiene un lock ya que solo puede ser utilizada por un cache a la vez
class Bus{
    private:
        unsigned* mem;
    public:
        pthread_mutex_t lock;
        // Constructor
        Bus(unsigned* m){
            mem=m;
            if (pthread_mutex_init(&lock, NULL)){
                printf("\nAlgo salio mal creando el mutex del bus\n");
            }
        }
        // Destructor
        ~Bus(){
            pthread_mutex_destroy(&lock);
        }
        // Retorna la data de la posicion de memoria
        unsigned* getData(unsigned pos){
            return &mem[pos];
        }
        
};
#endif
