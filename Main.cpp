#include <pthread.h>
#include "Processor.cpp"
#include "Bus.cpp"
#include "Memory.cpp"

// Definiciones editables
#define NUM_PROCS 2
#define WORDS_PER_BLOCK 4
#define BLOCKS_IN_MEMORY 88
#define QUANTUM 16

pthread_barrier_t  synchroBarrier;
Memory *mainMemory;
Bus *instrBus;
struct threadData{
};
void* threadProcessor(void* paramPtr){
      Processor* proc = new Processor;
      
      while(!proc->getFin()){
        proc->execute();
        
        // FALTA MUCHO CODIGO
        
      }
      
      delete proc;
}

int main(){
    mainMemory = new Memory(WORDS_PER_BLOCK,BLOCKS_IN_MEMORY);
    instrBus = new Bus(mainMemory),*dataBus = new Bus(mainMemory);
    
    pthread_barrier_init (&barrier, NULL, NUM_PROCS);
    pthread_t *thread = new pthread_t[NUM_PROCS];
    // BEGIN MULTITHREAD
    for(int i=0;i<NUM_PROCS;i++){
        pthread_create(&thread[i],NULL,threadProcessor,NULL);
    }
    // END MULTITHREAD
    for(int i=0;i<NUM_PROCS;i++){
        pthread_join(thread[i],NULL);
    }
    
    delete[] thread;
    return 0;
}
