#ifndef INSTRUCTIONS_CPP
#define INSTRUCTIONS_CPP
// Clase encargada de las instrucciones MIPS

#include "Processor.cpp"

class Instructions{
    private:
        //
    public:
        // Constructor
        Instructions(){
            
        }
        // Destructor
        ~Instructions(){
        }
        
        //Metodo DADDI
        void DADDI(Processor * proc, int rx, int ry, int n){
             proc->state->registers[rx] = proc->state->registers[ry] + n;
        }      
};
#endif
