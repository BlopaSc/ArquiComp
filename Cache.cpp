#ifndef CACHE_CPP
#define CACHE_CPP
#include "Bus.cpp"
#define BLOCKS_PER_CACHE 8
extern pthread_barrier_t synchroBarrier;
extern int m;
extern int b;
extern bool verbose;
class Cache{
    private:
        // Almacena el bus con el que se comunicara
        Bus* bus;
        // Guarda los tags de los caches y la data del cache
        unsigned* tag;
        char* status;
        unsigned** cache;
        // Contadores con fines estadisticos
        unsigned hitCounter,missCounter,multi;
        int idProcessor;
    public: 
        // Locks del cache
        pthread_mutex_t cacheLock;
        pthread_mutex_t noDeadLock; // Mutex que sirve para evitar deadlock en cache
        bool cacheTaken;
        // Constructor : multiplier se utiliza para la cache de instrucciones que la estamos trabajando como extendida
        Cache(Bus* b,unsigned multiplier,int id){
            multi = multiplier;
            tag = new unsigned[BLOCKS_PER_CACHE];
            status = new char[BLOCKS_PER_CACHE];
            cache = new unsigned*[BLOCKS_PER_CACHE];
            for(int i=0;i<BLOCKS_PER_CACHE;i++){
                cache[i]=new unsigned[WORDS_PER_BLOCK*multiplier];
            }
            bus = b;
            hitCounter=0;
            missCounter=0;
            idProcessor = id;
            // Inicializa los tags en -1 para obligarlo a cargar las instrucciones correctas
            for(int i=0;i<BLOCKS_PER_CACHE;i++){tag[i]=-1; status[i]='I';}
            if (pthread_mutex_init(&cacheLock, NULL)){
                printf("\nAlgo salio mal creando el mutex del cache\n");
            }
            if (pthread_mutex_init(&noDeadLock, NULL)){
                printf("\nAlgo salio mal creando el deadlock-mutex del cache\n");
            }
            cacheTaken=false;
        }
        // Destructor
        ~Cache(){
            delete[] tag;
            delete[] status;
            for(int i=0;i<BLOCKS_PER_CACHE;i++){
                delete[] cache[i];
            }
            delete[] cache;
        }
        // Revisa si la data se encuentra disponible, de no ser asi la trae de memoria y la devuelve
        unsigned* getInstruction(unsigned pos){
            // Calcula su numero de bloque
            unsigned blockNumber = pos/(WORDS_PER_BLOCK*multi);
            unsigned *transfer,copy;
            int wait;
            if(blockNumber!=tag[blockNumber%BLOCKS_PER_CACHE]){
                  // Seguro que evita deadlock en agarre de bus
                  pthread_mutex_lock(&(bus->lockDeadlock));
                  while(bus->busTaken){
                        if(verbose){printf("Proc %i: Waiting for bus\n",idProcessor);}
                        pthread_barrier_wait (&synchroBarrier);
                        pthread_barrier_wait (&synchroBarrier);
                  }
                  pthread_mutex_lock(&(bus->lock));
                  // Toma el bus solo si esta desocupado
                  bus->busTaken=true;
                  pthread_mutex_unlock(&(bus->lockDeadlock));
                  // Tranfiere datos
                  transfer = bus->getData(blockNumber*multi*WORDS_PER_BLOCK);
                  for(copy=0;copy<multi*WORDS_PER_BLOCK;copy++){
                       cache[blockNumber%BLOCKS_PER_CACHE][(pos%WORDS_PER_BLOCK)+copy] = transfer[copy];
                  }
                  // Espera
                  wait = WORDS_PER_BLOCK*(b+m+b);
                  for(copy=0;copy<wait;copy++){
                        if(verbose){printf("Proc %i: Getting data to cache\n",idProcessor);}
                        pthread_barrier_wait (&synchroBarrier);
                        pthread_barrier_wait (&synchroBarrier);
                  }
                  bus->busTaken=false;
                  missCounter++;
                  tag[blockNumber%BLOCKS_PER_CACHE]=blockNumber;
                  // Libera el bus
                  pthread_mutex_unlock(&(bus->lock));
            }else{
                  hitCounter++;
            }
            return &cache[blockNumber%BLOCKS_PER_CACHE][pos%(WORDS_PER_BLOCK*multi)];
        }
        bool getData(unsigned &data){
            bool success = false;
            
            return success;
        }
};
#endif
