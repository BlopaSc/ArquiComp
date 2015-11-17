#ifndef INSTRUCTIONS_CPP
#define INSTRUCTIONS_CPP
#define DATA_OFFSET 640
#define WORD_SIZE 4
#include "State.cpp"
#include "Cache.cpp"
#include <stdio.h>
extern bool verbose;
// Clase encargada de las instrucciones MIPS
class Instructions{
    public:
        char* printCache;
        // Constructor
        Instructions(){
            if(verbose){printCache = new char[0x100];}
        }
        // Destructor
        ~Instructions(){
            if(verbose){delete[] printCache;}
        }
        
        //Metodo DADDI
        void DADDI(State * state, int rx, int ry, int n){
             state->registers[rx] = state->registers[ry] + n;
             if(verbose){printf("%sR%i = R%i+%i = %i\n",printCache,rx,ry,n,state->registers[rx]);}
        }
        
        //Metodo DADD
        void DADD(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] + state->registers[rz];
             if(verbose){printf("%sR%i = R%i+R%i = %i\n",printCache,rx,ry,rz,state->registers[rx]);}
        }
        
        //DSUB
        void DSUB(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] - state->registers[rz];
             if(verbose){printf("%sR%i = R%i-R%i = %i\n",printCache,rx,ry,rz,state->registers[rx]);}
        }
        
        //DMUL
        void DMUL(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] * state->registers[rz];
             if(verbose){printf("%sR%i = R%i*R%i = %i\n",printCache,rx,ry,rz,state->registers[rx]);}
        }
        
        //DDIV
        void DDIV(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] / state->registers[rz];
             if(verbose){printf("%sR%i = R%i/R%i = %i\n",printCache,rx,ry,rz,state->registers[rx]);}
        }
        
        //BEQZ
        void BEQZ(State * state, int rx, int etiq){
             if(!state->registers[rx]){
                  state->pc+=0x4*etiq;
             }
             if(verbose){printf("%sif R%i = 0 JMP %i\n",printCache,rx,etiq);}
        }
        
        //BNEZ
        void BNEZ(State * state, int rx, int etiq){
             if(state->registers[rx]){
                  state->pc+=0x4*etiq;
             }
             if(verbose){printf("%sif R%i != 0 JMP %i\n",printCache,rx,etiq);}
        }
        
        //JAL
        void JAL(State * state, int n){
             state->registers[31]=state->pc;
             state->pc=state->pc+n;
             if(verbose){printf("%sR31 = PC ; PC += %i\n",printCache,n);}
        }
        
        //JR
        void JR(State * state, int rx){
             state->pc=state->registers[rx];
             if(verbose){printf("%sJMP R%i\n",printCache,rx);}
        }
        
        // LW
        void LW(State * state,Cache * cacheData,int rx,int n,int ry){
            pthread_mutex_lock(&(cacheData->noDeadLock));
            if(cacheData->cacheTaken){
                if(verbose){printf("%sLoad failed, busy cache\n",printCache);}
                pthread_mutex_unlock(&(cacheData->noDeadLock));
                state->pc -= 0x4;
                state->counter--;
            }else{
                pthread_mutex_lock(&(cacheData->cacheLock));
                cacheData->cacheTaken=true;
                pthread_mutex_unlock(&(cacheData->noDeadLock));
                bool success = cacheData->getData(&state->registers[rx],(state->registers[ry]+n-DATA_OFFSET)/WORD_SIZE);
                if(success){
                    if(verbose){printf("%sR%i <- M(%i+R%i) = %i\n",printCache,rx,n,ry,state->registers[rx]);}
                }else{
                    if(verbose){printf("%sLoad failed, busy bus\n",printCache);}
                    state->pc -= 0x4; state->counter--;
                }
                cacheData->cacheTaken=false;
                pthread_mutex_unlock(&(cacheData->cacheLock));
            }
        }
        
        // SW
        void SW(State * state,Cache * cacheData,int rx,int n,int ry){
            int data;
            pthread_mutex_lock(&(cacheData->noDeadLock));
            if(cacheData->cacheTaken){
                if(verbose){printf("%sLoad failed, busy cache\n",printCache);}
                state->pc -= 0x4;
                state->counter--;
                pthread_mutex_unlock(&(cacheData->noDeadLock));
            }else{
                pthread_mutex_lock(&(cacheData->cacheLock));
                cacheData->cacheTaken=true;
                pthread_mutex_unlock(&(cacheData->noDeadLock));
                // Revisa si el bloque esta en memoria o lo carga si no lo esta
                bool success = cacheData->getData(&data,(state->registers[ry]+n-DATA_OFFSET)/WORD_SIZE);
                if(success){
                    success = cacheData->saveData(state->registers[rx],(state->registers[ry]+n-DATA_OFFSET)/WORD_SIZE);
                    if(success){
                        if(verbose){printf("%sM(%i+R%i) <- R%i = %i\n",printCache,n,ry,rx,state->registers[rx]);}
                    }else{
                        if(verbose){printf("%sSave failed, wadafak\n",printCache);}
                        state->pc -= 0x4; state->counter--;
                    }
                }else{
                    if(verbose){printf("%sSave failed, busy bus\n",printCache);}
                    state->pc -= 0x4; state->counter--;
                }
                cacheData->cacheTaken=false;
                pthread_mutex_unlock(&(cacheData->cacheLock));
            }
        }
        
        // LL : Igual a LW pero ademas RL =  n + (Ry) 
        void LL(State * state,Cache * cacheData,int rx,int n,int ry){
             pthread_mutex_lock(&(cacheData->noDeadLock));
            if(cacheData->cacheTaken){
                if(verbose){printf("%sLoad failed, busy cache\n",printCache);}
                pthread_mutex_unlock(&(cacheData->noDeadLock));
                state->pc -= 0x4;
                state->counter--;
            }else{
                pthread_mutex_lock(&(cacheData->cacheLock));
                cacheData->cacheTaken=true;
                pthread_mutex_unlock(&(cacheData->noDeadLock));
                bool success = cacheData->getData(&state->registers[rx],(state->registers[ry]+n-DATA_OFFSET)/WORD_SIZE);
                if(success){
                    state->rl = n + state->registers[ry];
                    if(verbose){printf("%sR%i <- M(%i+R%i) = %i, RL = %i\n",printCache,rx,n,ry,state->registers[rx],state->rl);}
                }else{
                    if(verbose){printf("%sLoad failed, busy bus\n",printCache);}
                    state->pc -= 0x4; state->counter--;
                }
                cacheData->cacheTaken=false;
                pthread_mutex_unlock(&(cacheData->cacheLock));
            }
        }
        
        // SC : Igual a SW pero solo ejecuta el save si RL == n+(Ry), sino Rx = 0
        void SC(State * state,Cache * cacheData,int rx,int n,int ry){
             if(state->rl==(n + state->registers[ry])){
                 SW(state,cacheData,rx,n,ry);
             }else{
                 state->registers[rx]=0;
             }
        }
};
#endif
