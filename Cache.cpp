#ifndef CACHE_CPP
#define CACHE_CPP
#include "Bus.cpp"
#define BLOCKS_PER_CACHE 8
extern pthread_barrier_t synchroBarrier;
class Cache{
    private:
        // Almacena el bus con el que se comunicara
        Bus* bus;
        // Guarda los tags de los caches y la data del cache
        unsigned* tag;
        unsigned** cache;
        // Contadores con fines estadisticos
        unsigned hitCounter,missCounter,multi;
        // Barrera de sincronizacion utilizada por los procesadores
        
    public: 
        // Constructor : multiplier se utiliza para la cache de instrucciones que la estamos trabajando como extendida
        Cache(Bus* b,unsigned multiplier){
            multi = multiplier;
            tag = new unsigned[BLOCKS_PER_CACHE];
            cache = new unsigned*[BLOCKS_PER_CACHE];
            for(int i=0;i<BLOCKS_PER_CACHE;i++){
                cache[i]=new unsigned[WORDS_PER_BLOCK*multiplier];
            }
            bus = b;
            hitCounter=0;
            missCounter=0;
            // Inicializa los tags en -1 para obligarlo a cargar las instrucciones correctas
            for(int i=0;i<BLOCKS_PER_CACHE;i++){tag[i]=-1;}
            
        }
        // Destructor
        ~Cache(){
            delete[] tag;
            for(int i=0;i<BLOCKS_PER_CACHE;i++){
                delete[] cache[i];
            }
            delete[] cache;
        }
        // Revisa si la data se encuentra disponible, de no ser asi la trae de memoria y la devuelve
        unsigned* getData(unsigned pos){
            // Calcula su numero de bloque
            unsigned blockNumber = pos/(WORDS_PER_BLOCK*multi);
            unsigned *transfer,copy;
            if(blockNumber!=tag[blockNumber%BLOCKS_PER_CACHE]){
                  // SI EL MUTEX ESTA OCUPADO, ENBUCLAR AQUI ---> IMPORTANTISILIMO PRIORITY LVL 1 <---
                  pthread_mutex_lock(&(bus->lock));
                  transfer = bus->getData(blockNumber*multi*WORDS_PER_BLOCK);
                  for(copy=0;copy<multi*WORDS_PER_BLOCK;copy++){
                       cache[blockNumber%BLOCKS_PER_CACHE][(pos%WORDS_PER_BLOCK)+copy] = transfer[copy];
                  }
                  // AGREGAR WAIT
                  pthread_mutex_unlock(&(bus->lock));
                  missCounter++;
                  tag[blockNumber%BLOCKS_PER_CACHE]=blockNumber;
            }else{
                  hitCounter++;
            }
            return &cache[blockNumber%BLOCKS_PER_CACHE][pos%(WORDS_PER_BLOCK*multi)];
        }
};
#endif
