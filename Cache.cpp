#ifndef CACHE_CPP
#define CACHE_CPP
#include "Bus.cpp"
#define BLOCKS_PER_CACHE 8
class Cache{
    private:
        // Almacena el bus con el que se comunicara
        Bus* bus;
        // Guarda los tags de los caches y la data del cache
        unsigned* tag;
        unsigned* cache;
        // Contadores con fines estadisticos
        unsigned hitCounter,missCounter,multi;
    public: 
        // Constructor : multiplier se utiliza para la cache de instrucciones que la estamos trabajando como extendida
        Cache(Bus* b,unsigned multiplier){
            multi = multiplier;
            tag = new unsigned[BLOCKS_PER_CACHE];
            cache = new unsigned[BLOCKS_PER_CACHE*WORDS_PER_BLOCK*multiplier];
            bus = b;
            hitCounter=0;
            missCounter=0;
            // Inicializa los tags en -1 para obligarlo a cargar las instrucciones correctas
            for(int i=0;i<BLOCKS_PER_CACHE;i++){tag[i]=-1;}
            
        }
        // Destructor
        ~Cache(){
            delete[] tag;
            delete[] cache;
        }
        
        // CHECK THIS CODE DUDE, 100% LIKELY IT HAS A BUG
        
        // Revisa si la data se encuentra disponible, de no ser asi la trae de memoria y la devuelve
    unsigned* getData(unsigned pos){
        unsigned tagNumber = pos/(WORDS_PER_BLOCK*multi);
        if(tagNumber!=tag[tagNumber%BLOCKS_PER_CACHE]){
            pthread_mutex_lock(&(bus->lock));
            // IMPLEMENTAR LLAMADO
            pthread_mutex_unlock(&(bus->lock));
            missCounter++;
        }else{
            hitCounter++;
        }
        return &cache[(pos%WORDS_PER_BLOCK)+((tagNumber%BLOCKS_PER_CACHE)*WORDS_PER_BLOCK)];
    }
        
};
#endif
