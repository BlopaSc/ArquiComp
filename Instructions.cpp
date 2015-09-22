#ifndef INSTRUCTIONS_CPP
#define INSTRUCTIONS_CPP
#include "State.cpp"
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
        }
        
        //Metodo DADD
        void DADD(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] + state->registers[rz];
        }
        
        //DSUB
        void DSUB(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] - state->registers[rz];
        }
        
        //DMUL
        void DMUL(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] * state->registers[rz];
        }
        
        //DDIV
        void DDIV(State * state, int rx, int ry, int rz){
             state->registers[rx] = state->registers[ry] / state->registers[rz];
        }
        
        //BEQZ
        void BEQZ(State * state, int rx, int etiq){
             if(!state->registers[rx]){
                  state->pc=etiq;
             } 
        }
        
        //BNEZ
        void BNEZ(State * state, int rx, int etiq){
             if(state->registers[rx]){
                  state->pc=etiq;
             } 
        }
        
        //JAL
        void JAL(State * state, int n){
             state->registers[31]=state->pc;
             state->pc=state->pc+n;
        }
        
        
        //JR
        void JR(State * state, int rx){
             state->pc=state->registers[rx];
        }
        
};
#endif
