#ifndef PROCESSOR_CPP
#define PROCESSOR_CPP

#include <stdlib.h>
// Clase encargada de llevar los registros y el estado del pipeline del procesador 
class State{
    public:
        friend class Processor;
        // Registros
        int * registers;
        // Estados de pipeline
        // Constructor
        State(){
                registers = (int *) calloc (32*sizeof(int),0);
        }
        // Destructor
        ~State(){
                 free(registers);
        }
};

// Clase que emula un procesador
class Processor{
    private:
        long long cycles;
        unsigned char flags;
    public:
           friend class State;
           State* state;
        // Constructor
        Processor(){
            cycles=0;
            flags=0x0;
            state = new State();
        }
        // Destructor
        ~Processor(){
            delete state;
        }
        // Retorna si el procesador se encuentra en un estado de fin
        inline unsigned char getFin(){return flags&0x1;}
        // Retorna el ciclo actual del procesador para fines del quantum
        inline long long getCycle(){return cycles;}
        // Retorna el estado actual del procesador para almacenarlo
        inline State* getState(){return state;}
        // Es la señal que ocasiona la ejecucion de un nuevo ciclo de reloj
        void execute(){
        }
};
#endif
