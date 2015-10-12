#ifndef INSTRUCTIONS_CPP
#define INSTRUCTIONS_CPP
#include "State.cpp"
#include <stdio.h>
// Clase encargada de las instrucciones MIPS
class Instructions{
    private:
        bool verbose;
    public:
        // Constructor
        Instructions(){
            verbose=true;
        }
        // Destructor
        ~Instructions(){
        }
        
        //Metodo DADDI
        void DADDI(State * state, int rx, int ry, int n){
             state->registers[rx] = state->registers[ry] + n;
             if(verbose){printf("R%i = R%i+%i",rx,ry,n);}
        }
        
        //Metodo DADD
        void DADD(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] + state->registers[rz];
             if(verbose){printf("R%i = R%i+R%i",rx,ry,rz);}
        }
        
        //DSUB
        void DSUB(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] - state->registers[rz];
             if(verbose){printf("R%i = R%i-R%i",rx,ry,rz);}
        }
        
        //DMUL
        void DMUL(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] * state->registers[rz];
             if(verbose){printf("R%i = R%i*R%i",rx,ry,rz);}
        }
        
        //DDIV
        void DDIV(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] / state->registers[rz];
             if(verbose){printf("R%i = R%i/R%i",rx,ry,rz);}
        }
        
        //BEQZ
        void BEQZ(State * state, int rx, int etiq){
             if(!state->registers[rx]){
                  state->pc=etiq;
             }
             if(verbose){printf("if R%i = 0 JMP %i",rx,etiq);}
        }
        
        //BNEZ
        void BNEZ(State * state, int rx, int etiq){
             if(state->registers[rx]){
                  state->pc=etiq;
             }
             if(verbose){printf("if R%i != 0 JMP %i",rx,etiq);}
        }
        
        //JAL
        void JAL(State * state, int n){
             state->registers[31]=state->pc;
             state->pc=state->pc+n;
             if(verbose){printf("R31 = PC ; PC += %i",n);}
        }
        
        
        //JR
        void JR(State * state, int rx){
             state->pc=state->registers[rx];
             if(verbose){printf("JMP R%i",rx);}
        }
        
};
#endif
