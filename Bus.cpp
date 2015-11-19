#ifndef BUS_CPP
#define BUS_CPP
#include "Memory.cpp"
#include "Cache.h"
#include <pthread.h>
#include <stdio.h>
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
            pthread_mutex_destroy(&lockDeadlock);
            delete[] cache;
        }
        // Retorna la data de la posicion de memoria
        unsigned* getData(unsigned pos){
            return &mem[pos];
        }
        // Escribe la data en memoria
        void writeData(unsigned* data,unsigned pos,unsigned size){
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
                    cache[i]->invalidateBlock(blockNumber);
                }
            }
        }
        // Consulta si alguno de los caches contiene el bloque solicitado como modificado
        bool checkModified(unsigned blockNumber,int &idTargetProcessor){
            bool isModified=false;
            for(idTargetProcessor=0;idTargetProcessor<numProcs && !isModified;idTargetProcessor++){
                isModified = cache[idTargetProcessor]->checkModified(blockNumber);
            }
            return isModified;
        }
        // Bloquea el cache del cual se necesita un bloque
        void blockCache(int idProcessor){
            pthread_mutex_lock(&(cache[idProcessor]->noDeadLock));
            cache[idProcessor]->cacheTaken=true;
            pthread_mutex_unlock(&(cache[idProcessor]->noDeadLock));
        }
        // Ejecuta un writeback
        void orderWriteback(unsigned blockNumber,int idProcessor,int idProcessorCaller){
            pthread_mutex_lock(&(cache[idProcessor]->cacheLock));
            cache[idProcessor]->requestWriteback(blockNumber,cache[idProcessorCaller-1]->printCache);
            cache[idProcessor]->cacheTaken=false;
            pthread_mutex_unlock(&(cache[idProcessor]->cacheLock));
        }
};
#endif

        /*// Solicita un writeback   DEPRACATED
        bool requestWriteback(unsigned blockNumber,int idProcessor,int idProcessorCaller){
            bool success=false;
            pthread_mutex_lock(&(cache[idProcessor]->noDeadLock));
            if(cache[idProcessor]->cacheTaken){
                if(verbose){printf("Load failed, busy cache\n");}
                pthread_mutex_unlock(&(cache[idProcessor]->noDeadLock));
            }else{
                pthread_mutex_lock(&(cache[idProcessor]->cacheLock));
                cache[idProcessor]->cacheTaken=true;
                pthread_mutex_unlock(&(cache[idProcessor]->noDeadLock));
                cache[idProcessor]->requestWriteback(blockNumber,idProcessorCaller);
                cache[idProcessor]->cacheTaken=false;
                pthread_mutex_unlock(&(cache[idProcessor]->cacheLock));
            }
            return success;
        }*/
