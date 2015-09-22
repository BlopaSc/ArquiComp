#ifndef STATE_CPP
#define STATE_CPP
#include <stdlib.h>
// Clase encargada de llevar los registros y el estado del pipeline del procesador 
class State{
    public:
        // Registros
        int * registers;
        int pc;
        int rl;
        // FALTA: Estados de pipeline?
        // Constructor
        State(){
            registers = (int *) calloc (32*sizeof(int),0);
            pc=0;
            rl=0;
        }
        // Destructor
        ~State(){
            free(registers);
        }
};
#endif
