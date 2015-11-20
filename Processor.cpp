#ifndef PROCESSOR_CPP
#define PROCESSOR_CPP
#include "State.cpp"
#include "Instructions.cpp"
#include "Cache.cpp"
extern bool verbose;
extern pthread_barrier_t synchroBarrier;
// Clase que emula un procesador
class Processor{
    private:
        long long cycles;
        unsigned char flags;
        unsigned* instruction;
        int idProcessor;
        bool success;
    public:
        bool endOfCycle;
        State* state;
        Instructions* instr;
        Cache *cacheData,*cacheInstr;
        // Constructor
        Processor(Bus *instrBus,Bus *dataBus,int id){
            endOfCycle=true;
            cycles=1;
            flags=0x0;
            state = 0;
            idProcessor=id;
            cacheInstr = new Cache(instrBus,0x4,id);
            instrBus->setCacheLink(cacheInstr,idProcessor);
            cacheData = new Cache(dataBus,0x1,id);
            dataBus->setCacheLink(cacheData,idProcessor);
            instr = new Instructions(cacheData);
            instruction = new unsigned[4];
        }
        // Destructor
        ~Processor(){
            delete instr;
            delete cacheData;
            delete cacheInstr;
            if(state){
                delete state;
            }
            delete[] instruction;
        }
        // Retorna si el procesador se encuentra en un estado de fin
        inline unsigned char getFin(){return flags&0x1;}
        // Retorna el ciclo actual del procesador para fines del quantum
        inline long long getCycle(){return cycles;}
        // Retorna el estado actual del procesador para almacenarlo
        inline State* getState(){return state;}
        inline void setState(State* s){
            state=s;
            state->rl=-1;
            cacheData->currentState = state;
            sprintf(cacheData->printCache,"Proc %i Thread %i: ",idProcessor,state->id);
            sprintf(cacheInstr->printCache,"Proc %i Thread %i: ",idProcessor,state->id);
        }
        //Switch de las instrucciones MIPS
        void ejecutarMIPS(int codigo, int p1,int p2, int p3){
             switch(codigo){
               case 8:
                    instr->DADDI(state,p2,p1,p3);
                    break;
               case 32:
                    instr->DADD(state,p3,p1,p2);
                    break;
               case 34:
                    instr->DSUB(state,p3,p1,p2);
                    break;
               case 12:
                    instr->DMUL(state,p3,p1,p2);
                    break;
               case 14:
                    instr->DDIV(state,p3,p1,p2);
                    break;
               case 4:
                    instr->BEQZ(state,p1,p3);
                    break;
               case 5:
                    instr->BNEZ(state,p1,p3);
                    break;
               case 3:
                    instr->JAL(state,p3);
                    break;
               case 2:
                    instr->JR(state,p1);
                    break;
               case 63:
                    if(verbose){printf("%sTHREAD ENDED\n",instr->printCache);}
                    flags|=0x1;
                    break;
               case 35:
                    instr->LW(state,p2,p3,p1);
                    break;
               case 43:
                    // Si se realiza con exito el SW, ya paso el final de ciclo: endOfCycle = false
                    endOfCycle = !instr->SW(state,p2,p3,p1);
                    break;
               case 50:
                    instr->LL(state,p2,p3,p1);
                    break;
               case 51:
                    endOfCycle = !instr->SC(state,p2,p3,p1);
                    break;
             }
        }
        
        // Es la señal que ocasiona la ejecucion de un nuevo ciclo de reloj
        void execute(){
             if(state){
                    // Si se tiene cargado un estado ejecuta
                    // Revisa que el cache no se encuentre ocupado
                    pthread_mutex_lock(&(cacheInstr->noDeadLock));
                    if(cacheInstr->cacheTaken){
                        if(verbose){printf("Proc %i Thread %i: Cache busy, couldn't get instruction\n",idProcessor,state->id);}
                        pthread_mutex_unlock(&(cacheInstr->noDeadLock));
                    }else{
                        // Si esta libre intenta traer la instruccion
                        pthread_mutex_lock(&(cacheInstr->cacheLock));
                        cacheInstr->cacheTaken=true;
                        pthread_mutex_unlock(&(cacheInstr->noDeadLock));
                        success = cacheInstr->getData(instruction,state->pc);
                        cacheInstr->cacheTaken=false;
                        pthread_mutex_unlock(&(cacheInstr->cacheLock));
                        if(success){
                            // Si logra traerla ejecuta la instruccion
                            if(verbose){sprintf(instr->printCache,"Proc %i Thread %i: PC: %i, Instr: %i %i %i %i \t",idProcessor,state->id,state->pc,instruction[0],instruction[1],instruction[2],instruction[3]);}
                            state->pc += 0x4;
                            state->counter++;
                            ejecutarMIPS(instruction[0],instruction[1],instruction[2],instruction[3]);
                            cycles++;
                        }else{
                            if(verbose){printf("Proc %i Thread %i: Waiting for bus\n",idProcessor,state->id);}
                        }
                    }
             }else{
                    if(verbose){printf("Proc %i: No-op\n",idProcessor);}
             }
        }
        
        // Es llamado cuando un hilo ha acabado y debe ser eliminado y las banderas reiniciadas
        void finishState(){
             cycles=1;
             state=0;
             flags=0x0;
        }
};
#endif
