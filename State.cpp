#ifndef STATE_CPP
#define STATE_CPP
#include <stdio.h>
#define REGISTER_COUNT 32
// Clase encargada de llevar los registros y el estado del pipeline del procesador 
class State{
    public:
        // Registros
        int * registers;
        int pc;
        int rl;
        // Constructor
        State(){
            registers = new int[REGISTER_COUNT];
	        for(int i=0;i<REGISTER_COUNT;i++){registers[i]=0;}
            pc=0;
            rl=0;
        }
        // Destructor
        ~State(){
            delete[] registers;
        }
        // Print del estado
        void printState(){
            printf("Registers: ");
            for(int i=0;i<REGISTER_COUNT;i++){
                if(registers[i]){
                    printf("R%i = %i, ",i,registers[i]);
                }
            }
            printf("\n");
        }
};
#endif
