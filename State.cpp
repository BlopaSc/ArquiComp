#ifndef STATE_CPP
#define STATE_CPP
#include <stdio.h>
#include <stdlib.h>
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
            registers = (int *) calloc (REGISTER_COUNT*sizeof(int),0);
            pc=0;
            rl=0;
        }
        // Destructor
        ~State(){
            free(registers);
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
