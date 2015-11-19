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
                if(verbose){printf("%sSaving data to memory\n",printCache);}
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
                for(int j=0;j<WORDS_PER_BLOCK*multiplier;j++){cache[i][j]=0;}
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
            if(verbose){printCache = new char[0x100];printData = new char[0x400];}
            cacheTaken=false;
}
// Destructor
Cache::~Cache(){
            for(int i=0;i<BLOCKS_PER_CACHE;i++){
                // Si hay data modificada, guardela
                if(status[i]=='M'){
                    bus->writeData(cache[i],tag[i]*WORDS_PER_BLOCK*multi,WORDS_PER_BLOCK*multi);
                    status[i]='C';
                }
            }
            delete[] tag;
            delete[] status;
            for(int i=0;i<BLOCKS_PER_CACHE;i++){
                delete[] cache[i];
            }
            delete[] cache;
            pthread_mutex_destroy(&cacheLock);
            pthread_mutex_destroy(&noDeadLock);
            if(verbose){delete[] printCache;delete[] printData;}
}
// Revisa si la data se encuentra disponible, de no ser asi la trae de memoria y la devuelve
bool Cache::getData(int *data,int pos){
            int idProcMod;
            bool success;
            // Calcula su numero de bloque
            unsigned blockNumber = pos/(WORDS_PER_BLOCK*multi);
            unsigned *transfer,copy;
            int wait;
            if(success = (blockNumber==tag[blockNumber%BLOCKS_PER_CACHE] && status[blockNumber%BLOCKS_PER_CACHE]!='I')){
                // Si esta en cache y es valido
                hitCounter++;
                for(copy=0;copy<multi;copy++){
                    data[copy] = cache[blockNumber%BLOCKS_PER_CACHE][(pos%(WORDS_PER_BLOCK*multi))+copy];
                }
            }else{
                // Seguro que evita deadlock en agarre de bus
                pthread_mutex_lock(&(bus->lockDeadlock));
                if(bus->busTaken){
                    pthread_mutex_unlock(&(bus->lockDeadlock));
                }else{
                    // Toma el bus
                    pthread_mutex_lock(&(bus->lock));
                    bus->busTaken=true;
                    pthread_mutex_unlock(&(bus->lockDeadlock));
                    // Si el valor de los datos previos ha sido modificado, los guarda
                    if(status[blockNumber%BLOCKS_PER_CACHE]=='M'){
                        writeback(tag[blockNumber%BLOCKS_PER_CACHE]);
                    }
                    // Espera a fin de ciclo para revisar si el bloque se encuentra modificado en otros lugares
                    if(verbose){printf("%sWaiting for cycle end to check caches - load\n",printCache);}
                    pthread_barrier_wait (&synchroBarrier);
                    if(success = bus->checkModified(blockNumber,idProcMod)){
                        // Tomar cache
                        bus->blockCache(idProcMod);
                    }
                    pthread_barrier_wait (&synchroBarrier);
                    // Si se encuentra modificado en algun otro lugar
                    if(success){
                        // Solicita writeback
                        bus->orderWriteback(blockNumber,idProcMod,idProcessor);
                        // Tranfiere datos
                        transfer = bus->getData(blockNumber*multi*WORDS_PER_BLOCK);
                        for(copy=0;copy<multi*WORDS_PER_BLOCK;copy++){
                            cache[blockNumber%BLOCKS_PER_CACHE][copy] = transfer[copy];
                        }
                        // Retorna los datos obtenidos y actualiza su informacion
                        missCounter++;
                        tag[blockNumber%BLOCKS_PER_CACHE]=blockNumber;
                        status[blockNumber%BLOCKS_PER_CACHE]='C';
                        for(copy=0;copy<multi;copy++){
                            data[copy] = cache[blockNumber%BLOCKS_PER_CACHE][(pos%(WORDS_PER_BLOCK*multi))+copy];
                        }
                    }else{
                        // Sino, tranfiere datos desde memoria
                        transfer = bus->getData(blockNumber*multi*WORDS_PER_BLOCK);
                        for(copy=0;copy<multi*WORDS_PER_BLOCK;copy++){
                            cache[blockNumber%BLOCKS_PER_CACHE][copy] = transfer[copy];
                        }
                        // Espera
                        wait = WORDS_PER_BLOCK*(b+m+b);
                        for(copy=0;copy<wait;copy++){
                            if(verbose){printf("%sGetting data to cache\n",printCache);}
                            pthread_barrier_wait (&synchroBarrier);
                            pthread_barrier_wait (&synchroBarrier);
                        }
                        // Retorna los datos obtenidos y actualiza su informacion
                        missCounter++;
                        tag[blockNumber%BLOCKS_PER_CACHE]=blockNumber;
                        status[blockNumber%BLOCKS_PER_CACHE]='C';
                        for(copy=0;copy<multi;copy++){
                            data[copy] = cache[blockNumber%BLOCKS_PER_CACHE][(pos%(WORDS_PER_BLOCK*multi))+copy];
                        }
                        success=true;
                    }
                    // Libera el bus
                    bus->busTaken=false;
                    pthread_mutex_unlock(&(bus->lock));
                }
            }
            return success;
}
// Se encarga de almacenar un dato en una posicion de memoria
bool Cache::saveData(int data,int pos){
            // Calcula su numero de bloque
            int blockNumber = pos/(WORDS_PER_BLOCK*multi);
            int idProcMod,wait;
            unsigned *transfer,copy;
            bool success=false;
            // Intenta tomar el bus
            pthread_mutex_lock(&(bus->lockDeadlock));
            if(bus->busTaken){
                pthread_mutex_unlock(&(bus->lockDeadlock));
            }else{
                // Toma el bus
                pthread_mutex_lock(&(bus->lock));
                bus->busTaken=true;
                pthread_mutex_unlock(&(bus->lockDeadlock));
                // Revisa si contiene el bloque y no esta invalido
                success = (blockNumber==tag[blockNumber%BLOCKS_PER_CACHE] && status[blockNumber%BLOCKS_PER_CACHE]!='I');
                if(success){
                    // Ya se tiene la data
                    cache[blockNumber%BLOCKS_PER_CACHE][pos%(WORDS_PER_BLOCK*multi)] = data;
                    status[blockNumber%BLOCKS_PER_CACHE]='M';
                    blockInvalidate = blockNumber;
                    hitCounter++;
                }else{
                    // Si no lo tiene o esta invalido
                    // Si el valor de los datos previos ha sido modificado, los guarda
                    if(status[blockNumber%BLOCKS_PER_CACHE]=='M'){
                        writeback(tag[blockNumber%BLOCKS_PER_CACHE]);
                    }
                    // Espera a fin de ciclo para revisar si el bloque se encuentra modificado en otros lugares
                    if(verbose){printf("%sWaiting for cycle end to check caches - save\n",printCache);}
                    pthread_barrier_wait (&synchroBarrier);
                    if(bus->checkModified(blockNumber,idProcMod)){
                        // Tomar cache
                        success = bus->blockCache(idProcMod);
                        pthread_barrier_wait (&synchroBarrier);
                        // Si se encuentra modificado en algun otro lugar
                        if(success){
                            // Solicita writeback
                            bus->orderWriteback(blockNumber,idProcMod,idProcessor);
                            // Tranfiere datos
                            transfer = bus->getData(blockNumber*multi*WORDS_PER_BLOCK);
                            for(copy=0;copy<multi*WORDS_PER_BLOCK;copy++){
                                cache[blockNumber%BLOCKS_PER_CACHE][copy] = transfer[copy];
                            }
                            // Actualiza la informacion
                            missCounter++;
                            tag[blockNumber%BLOCKS_PER_CACHE]=blockNumber;
                            cache[blockNumber%BLOCKS_PER_CACHE][pos%(WORDS_PER_BLOCK*multi)] = data;
                            status[blockNumber%BLOCKS_PER_CACHE]='M';
                            blockInvalidate = blockNumber;
                        }
                    }else{
                        pthread_barrier_wait (&synchroBarrier);
                        // Sino, tranfiere datos desde memoria
                        transfer = bus->getData(blockNumber*multi*WORDS_PER_BLOCK);
                        for(copy=0;copy<multi*WORDS_PER_BLOCK;copy++){
                            cache[blockNumber%BLOCKS_PER_CACHE][copy] = transfer[copy];
                        }
                        // Espera
                        wait = WORDS_PER_BLOCK*(b+m+b);
                        for(copy=0;copy<wait;copy++){
                            if(verbose){printf("%sGetting data to cache\n",printCache);}
                            pthread_barrier_wait (&synchroBarrier);
                            pthread_barrier_wait (&synchroBarrier);
                        }
                        // Actualiza la informacion
                        missCounter++;
                        tag[blockNumber%BLOCKS_PER_CACHE]=blockNumber;
                        cache[blockNumber%BLOCKS_PER_CACHE][pos%(WORDS_PER_BLOCK*multi)] = data;
                        status[blockNumber%BLOCKS_PER_CACHE]='M';
                        blockInvalidate = blockNumber;
                        success=true;
                    }
                }
                if(!success){
                    bus->busTaken=false;
                    pthread_mutex_unlock(&(bus->lock));
                }
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
            // Espera a final de ciclo
            pthread_barrier_wait (&synchroBarrier);
            bus->invalidateBlock(blockInvalidate,idProcessor);
            // Libera el bus
            bus->busTaken=false;
            pthread_mutex_unlock(&(bus->lock));
}
// Recibe solicitud para ver si bloque esta modificado
bool Cache::checkModified(unsigned blockNumber){
    return (tag[blockNumber%BLOCKS_PER_CACHE]==blockNumber)&&(status[blockNumber%BLOCKS_PER_CACHE]=='M');
}
// Recibe una solicitud de writeback
void Cache::requestWriteback(unsigned blockNumber,char* printInfo){
    if(blockNumber == tag[blockNumber%BLOCKS_PER_CACHE]){
        int wait = WORDS_PER_BLOCK*(b+m+b);
        for(int copy = 0;copy<wait;copy++){
            if(verbose){printf("%sSaving data to memory\n",printInfo);}
            pthread_barrier_wait (&synchroBarrier);
            pthread_barrier_wait (&synchroBarrier);
        }
        bus->writeData(cache[blockNumber%BLOCKS_PER_CACHE],blockNumber*WORDS_PER_BLOCK*multi,WORDS_PER_BLOCK*multi);
        status[blockNumber%BLOCKS_PER_CACHE]='C';
    }
}
// Retorna la data del cache a modo de string
char* Cache::getDataPrint(){
    sprintf(printData,"Cache:");
    for(int i=0;i<BLOCKS_PER_CACHE;i++){
        sprintf(nullPos()," %i,%c: %i,%i,%i,%i",tag[i],status[i],cache[i][0],cache[i][1],cache[i][2],cache[i][3]);
    }
    sprintf(nullPos(),"\n");
    return printData;
}
// Retorna la posicion del '\0' en la cadena printData
char* Cache::nullPos(){
    char* tmp = printData;
    while(*tmp){tmp++;}
    return tmp;
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
