#include "Processor.cpp"
#include "Bus.cpp"
#include "Memory.cpp"
int main(){
    
    Memory *mainMemory = new Memory;
    
    Bus *instrBus = new Bus(mainMemory),*dataBus = new Bus(mainMemory);
    
    // BEGIN MULTITHREAD
    Processor* proc = new Processor;
    
    delete proc;
    // END MULTITHREAD
    
    return 0;
}
