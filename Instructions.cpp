#ifndef INSTRUCTIONS_CPP
#define INSTRUCTIONS_CPP
#include "State.cpp"
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
        
};
#endif
