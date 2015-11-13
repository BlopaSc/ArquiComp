#ifndef BUS_CPP
#define BUS_CPP
#include "Memory.cpp"
#include <pthread.h>
#include <stdio.h>
// Mencion a clase cache que sera utilizada
class Cache;
// Clase intermediaria utilizada por los diferentes caches para extraer informacion de la memoria
// Contiene un lock ya que solo puede ser utilizada por un cache a la vez
class Bus{
    private:
        unsigned* mem;
        Cache** cache;
        unsigned numProcs;
    public:
        pthread_mutex_t lock;
        pthread_mutex_t lockDeadlock;
        bool busTaken;
        // Constructor
        Bus(unsigned* m,unsigned processors){
            mem=m;
            numProcs = processors;
            cache = new Cache*[numProcs];
            if (pthread_mutex_init(&lock, NULL)){
                printf("\nAlgo salio mal creando el mutex del bus\n");
            }
            if (pthread_mutex_init(&lockDeadlock, NULL)){
                printf("\nAlgo salio mal creando el mutex para evitar el deadlock en bus\n");
            }
            busTaken=false;
        }
        // Destructor
        ~Bus(){
            pthread_mutex_destroy(&lock);
            delete[] cache;
        }
        // Retorna la data de la posicion de memoria
        unsigned* getData(unsigned pos){
            return &mem[pos];
        }
        // Escribe la data en memoria
        unsigned *writeData(unsigned* data,unsigned pos,unsigned size){
            for(unsigned i=0;i<size;i++){
                mem[pos+i]=data[i];
            }
        }
        // Vincula cache con el bus
        void setCacheLink(Cache* procCache,int idProcessor){
            idProcessor--;
            cache[idProcessor]=procCache;
        }
        // Invalida el bloque de memoria en los demas caches
        void invalidateBlock(unsigned blockNumber,int idProcessor){
            idProcessor--;
            for(int i=0;i<numProcs;i++){
                if(i!=idProcessor){
                    //cache[i]->invalidateBlock(blockNumber);
                }
            }
        }
};
#endif
