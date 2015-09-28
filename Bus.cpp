#ifndef BUS_CPP
#define BUS_CPP
#include "Memory.cpp"
#include<pthread.h>

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
};
#endif
