#include "Processor.cpp"
#include "Bus.cpp"
#include "Memory.cpp"

// Definiciones editables
#define WORDS_PER_BLOCK 4
#define BLOCKS_IN_MEMORY 88
#define QUANTUM 16


int main(){
    
    Memory *mainMemory = new Memory(WORDS_PER_BLOCK,BLOCKS_IN_MEMORY);
    
    Bus *instrBus = new Bus(mainMemory),*dataBus = new Bus(mainMemory);
    
    // BEGIN MULTITHREAD
    Processor* proc = new Processor;
    
    // FALTA: Cargar estado inicial
    
    while(!proc->getFin()){
        proc->execute();
        
        // FALTA MUCHO CODIGO
        
    }
    
    delete proc;
    // END MULTITHREAD
    
    return 0;
}
