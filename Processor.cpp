#ifndef PROCESSOR.CPP
#define PROCESSOR_CPP
// Clase encargada de llevar los registros y el estado del pipeline del procesador 
class State{
    public:
        // Registros
        // Estados de pipeline
        // Constructor
        State(){
        }
        // Destructor
        ~State(){
        }
};

// Clase que emula un procesador
class Processor{
    private:
        State* state;
        long long cycles;
        unsigned char flags;
    public:
        // Constructor
        Processor(){
            cycles=0;
            flags=0x0;
        }
        // Destructor
        ~Processor(){
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
