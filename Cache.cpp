#ifndef CACHE_CPP
#define CACHE_CPP
#include "Cache.h"
#include "Bus.cpp"
#define BLOCKS_PER_CACHE 8
extern pthread_barrier_t synchroBarrier;
extern int m;
extern int b;
extern bool verbose;
// Se encarga de realizar un writeback de un bloque
void Cache::writeback(unsigned block){
            int wait = WORDS_PER_BLOCK*(b+m+b);
            for(int copy = 0;copy<wait;copy++){
                if(verbose){printf("Proc %i: Saving data to memory\n",idProcessor);}
                pthread_barrier_wait (&synchroBarrier);
                pthread_barrier_wait (&synchroBarrier);
            }
            bus->writeData(cache[block%BLOCKS_PER_CACHE],block*WORDS_PER_BLOCK*multi,WORDS_PER_BLOCK*multi);
            status[block%BLOCKS_PER_CACHE]='C';
}
// Constructor : multiplier se utiliza para la cache de instrucciones que la estamos trabajando como extendida
Cache::Cache(Bus* b,unsigned multiplier,int id){
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
            idProcessor=id;
            blockInvalidate=-1;
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
Cache::~Cache(){
            delete[] tag;
            delete[] status;
            for(int i=0;i<BLOCKS_PER_CACHE;i++){
                delete[] cache[i];
            }
            delete[] cache;
}
// Revisa si la data se encuentra disponible, de no ser asi la trae de memoria y la devuelve
bool Cache::getData(int *data,int pos){
            int idProcMod;
            bool success = false;
            bool isModified;
            // Calcula su numero de bloque
            unsigned blockNumber = pos/(WORDS_PER_BLOCK*multi);
            unsigned *transfer,copy;
            int wait;
            if(blockNumber==tag[blockNumber%BLOCKS_PER_CACHE] && status[blockNumber%BLOCKS_PER_CACHE]!='I'){
                // Si esta en cache y es valido
                hitCounter++;
                for(copy=0;copy<multi;copy++){
                    data[copy] = cache[blockNumber%BLOCKS_PER_CACHE][(pos%(WORDS_PER_BLOCK*multi))+copy];
                }
                success=true;
            }else{
                // Seguro que evita deadlock en agarre de bus
                pthread_mutex_lock(&(bus->lockDeadlock));
                if(bus->busTaken){
                    pthread_mutex_unlock(&(bus->lockDeadlock));
                }else{
                    pthread_mutex_lock(&(bus->lock));
                    bus->busTaken=true;
                    pthread_mutex_unlock(&(bus->lockDeadlock));
                    // Si el valor de los datos previos ha sido modificado, los guarda
                    if(status[blockNumber%BLOCKS_PER_CACHE]=='M'){
                        writeback(tag[blockNumber%BLOCKS_PER_CACHE]);
                    }
                    // Revisa si se encuentra modificado en algun otro lugar
                    if(bus->checkModified(blockNumber,idProcMod)){
                        // Solicita writeback
                        success = bus->requestWriteback(blockNumber,idProcMod,idProcessor);
                        if(success){
                            // Tranfiere datos
                            transfer = bus->getData(blockNumber*multi*WORDS_PER_BLOCK);
                            for(copy=0;copy<multi*WORDS_PER_BLOCK;copy++){
                                cache[blockNumber%BLOCKS_PER_CACHE][(pos%WORDS_PER_BLOCK)+copy] = transfer[copy];
                            }
                        }
                    }else{
                        // Sino, tranfiere datos desde memoria
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
                        status[blockNumber%BLOCKS_PER_CACHE]='C';
                        // Libera el bus
                        pthread_mutex_unlock(&(bus->lock));
                        for(copy=0;copy<multi;copy++){
                            data[copy] = cache[blockNumber%BLOCKS_PER_CACHE][(pos%(WORDS_PER_BLOCK*multi))+copy];
                        }
                        success=true;
                    }
                }
            }
            return success;
}
// Se encarga de almacenar un dato en una posicion de memoria
bool Cache::saveData(int data,int pos){
            bool success=false;
            // Calcula su numero de bloque
            int blockNumber = pos/(WORDS_PER_BLOCK*multi);
            if(blockNumber==tag[blockNumber%BLOCKS_PER_CACHE] && status[blockNumber%BLOCKS_PER_CACHE]!='I'){
                cache[blockNumber%BLOCKS_PER_CACHE][pos%(WORDS_PER_BLOCK*multi)] = data;
                status[blockNumber%BLOCKS_PER_CACHE]='M';
                blockInvalidate=blockNumber;
                success = true;
            }
            return success;
}
// Se encarga de invalidar un bloque cuando recibe la notificacion del bus
void Cache::invalidateBlock(unsigned blockNumber){
            if(blockNumber == tag[blockNumber%BLOCKS_PER_CACHE]){
                status[blockNumber%BLOCKS_PER_CACHE]='I';
            }
}
// Se encarga de enviar cualquier señal de invalidacion que haga falta
void Cache::signalInvalidate(){
            if(blockInvalidate!=-1){
                bus->invalidateBlock(blockInvalidate,idProcessor);
                blockInvalidate=-1;
            }
}
// Recibe solicitud para ver si bloque esta modificado
bool Cache::checkModified(unsigned blockNumber){
    return (tag[blockNumber%BLOCKS_PER_CACHE]==blockNumber)&&(status[blockNumber%BLOCKS_PER_CACHE]=='M');
}
// Recibe una solicitud de writeback
void Cache::requestWriteback(unsigned blockNumber,int idCaller){
    if(blockNumber == tag[blockNumber%BLOCKS_PER_CACHE]){
        int wait = WORDS_PER_BLOCK*(b+m+b);
        for(int copy = 0;copy<wait;copy++){
            if(verbose){printf("Proc %i: Saving data to memory\n",idCaller);}
            pthread_barrier_wait (&synchroBarrier);
            pthread_barrier_wait (&synchroBarrier);
        }
        bus->writeData(cache[blockNumber%BLOCKS_PER_CACHE],blockNumber*WORDS_PER_BLOCK*multi,WORDS_PER_BLOCK*multi);
        status[blockNumber%BLOCKS_PER_CACHE]='C';
    }
}
#endif

        /* DEPRECATED :: AHORA TODO SE HACE DESDE GETDATA
        // Revisa si la instruccion se encuentra disponible, de no ser asi la trae de memoria y la devuelve
        unsigned* getInstruction(unsigned pos){
            // Calcula su numero de bloque
            unsigned blockNumber = pos/(WORDS_PER_BLOCK*multi);
            unsigned *transfer,copy;
            int wait;
            if(blockNumber==tag[blockNumber%BLOCKS_PER_CACHE] && status[blockNumber%BLOCKS_PER_CACHE]!='I'){
                hitCounter++;
            }else{
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
                  if(status[blockNumber%BLOCKS_PER_CACHE]=='M'){
                        writeback(blockNumber);
                  }
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
                  status[blockNumber%BLOCKS_PER_CACHE]='C';
                  // Libera el bus
                  pthread_mutex_unlock(&(bus->lock));
            }
            return &cache[blockNumber%BLOCKS_PER_CACHE][pos%(WORDS_PER_BLOCK*multi)];
        }*/
