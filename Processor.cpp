#ifndef PROCESSOR_CPP
#define PROCESSOR_CPP

#include "State.cpp"
#include "Instructions.cpp"
// Clase que emula un procesador
class Processor{
    private:
        long long cycles;
        unsigned char flags;
    public:
           friend class State;
           friend class Instructions;
           State* state;
           Instructions* instr;
        // Constructor
        Processor(){
            cycles=0;
            flags=0x0;
            state = new State();
            instr = new Instructions();
        }
        // Destructor
        ~Processor(){
            delete state;
            delete instr;
        }
        // Retorna si el procesador se encuentra en un estado de fin
        inline unsigned char getFin(){return flags&0x1;}
        // Retorna el ciclo actual del procesador para fines del quantum
        inline long long getCycle(){return cycles;}
        // Retorna el estado actual del procesador para almacenarlo
        inline State* getState(){return state;}
        //Switch de las instrucciones MIPS
        void ejecutarMIPS(int codigo, int p1,int p2, int p3){
             switch(codigo){
               case 8:
                    instr->DADDI(state,p2,p1,p3);
                    break;
               case 32:
                    instr->DADD(state,p2,p3,p1);
                    break;
               case 34:
                    instr->DSUB(state,p2,p3,p1);
                    break;
               case 12:
                    instr->DMUL(state,p2,p3,p1);
                    break;
               case 14:
                    instr->DDIV(state,p2,p3,p1);
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
             }
        }
        
        
        // Es la señal que ocasiona la ejecucion de un nuevo ciclo de reloj
        void execute(){
        }  
        
};
#endif
