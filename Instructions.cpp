#ifndef INSTRUCTIONS_CPP
#define INSTRUCTIONS_CPP
#include "State.cpp"
#include "Cache.cpp"
#include <stdio.h>
extern bool verbose;
// Clase encargada de las instrucciones MIPS
class Instructions{
    public:
        // Constructor
        Instructions(){
        }
        // Destructor
        ~Instructions(){
        }
        
        //Metodo DADDI
        void DADDI(State * state, int rx, int ry, int n){
             state->registers[rx] = state->registers[ry] + n;
             if(verbose){printf("R%i = R%i+%i = %i\n",rx,ry,n,state->registers[rx]);}
        }
        
        //Metodo DADD
        void DADD(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] + state->registers[rz];
             if(verbose){printf("R%i = R%i+R%i = %i\n",rx,ry,rz,state->registers[rx]);}
        }
        
        //DSUB
        void DSUB(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] - state->registers[rz];
             if(verbose){printf("R%i = R%i-R%i = %i\n",rx,ry,rz,state->registers[rx]);}
        }
        
        //DMUL
        void DMUL(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] * state->registers[rz];
             if(verbose){printf("R%i = R%i*R%i = %i\n",rx,ry,rz,state->registers[rx]);}
        }
        
        //DDIV
        void DDIV(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] / state->registers[rz];
             if(verbose){printf("R%i = R%i/R%i = %i\n",rx,ry,rz,state->registers[rx]);}
        }
        
        //BEQZ
        void BEQZ(State * state, int rx, int etiq){
             if(!state->registers[rx]){
                  state->pc+=0x4*etiq;
             }
             if(verbose){printf("if R%i = 0 JMP %i\n",rx,etiq);}
        }
        
        //BNEZ
        void BNEZ(State * state, int rx, int etiq){
             if(state->registers[rx]){
                  state->pc+=0x4*etiq;
             }
             if(verbose){printf("if R%i != 0 JMP %i\n",rx,etiq);}
        }
        
        //JAL
        void JAL(State * state, int n){
             state->registers[31]=state->pc;
             state->pc=state->pc+n;
             if(verbose){printf("R31 = PC ; PC += %i\n",n);}
        }
        
        //JR
        void JR(State * state, int rx){
             state->pc=state->registers[rx];
             if(verbose){printf("JMP R%i\n",rx);}
        }
        
        // LW
        void LW(State * state,Cache * cacheData,int rx,int n,int ry){
            pthread_mutex_lock(&(cacheData->noDeadLock));
            if(cacheData->cacheTaken){
                if(verbose){printf("Load failed, busy cache\n");}
                pthread_mutex_unlock(&(cacheData->noDeadLock));
                state->pc -= 0x4;
                state->counter--;
            }else{
                pthread_mutex_lock(&(cacheData->cacheLock));
                cacheData->cacheTaken=true;
                pthread_mutex_unlock(&(cacheData->noDeadLock));
                bool success = cacheData->getData(&state->registers[rx],(state->registers[ry]+n));
                if(success){
                    if(verbose){printf("R%i <- M(%i+R%i) = %i\n",rx,n,ry,state->registers[rx]);}
                }else{
                    if(verbose){printf("Load failed, busy bus\n");}
                    state->pc -= 0x4; state->counter--;
                }
                cacheData->cacheTaken=false;
                pthread_mutex_unlock(&(cacheData->cacheLock));
            }
        }
        
        // SW
        void SW(State * state,Cache * cacheData,int rx,int n,int ry){
            pthread_mutex_lock(&(cacheData->noDeadLock));
            int data;
            if(cacheData->cacheTaken){
                if(verbose){printf("Load failed, busy cache\n");}
                pthread_mutex_unlock(&(cacheData->noDeadLock));
                state->pc -= 0x4;
                state->counter--;
            }else{
                pthread_mutex_lock(&(cacheData->cacheLock));
                cacheData->cacheTaken=true;
                pthread_mutex_unlock(&(cacheData->noDeadLock));
                // Revisa si el bloque esta en memoria o lo carga si no lo esta
                bool success = cacheData->getData(&data,(state->registers[ry]+n));
                if(success){
                    success = cacheData->saveData(state->registers[rx],(state->registers[ry]+n));
                    if(success){
                        if(verbose){printf("M(%i+R%i) <- R%i = %i\n",n,ry,rx,state->registers[rx]);}
                    }else{
                        if(verbose){printf("Save failed, wadafak\n");}
                        state->pc -= 0x4; state->counter--;
                    }
                }else{
                    if(verbose){printf("Save failed, busy bus\n");}
                    state->pc -= 0x4; state->counter--;
                }
                cacheData->cacheTaken=false;
                pthread_mutex_unlock(&(cacheData->cacheLock));
            }
        }
};
#endif
