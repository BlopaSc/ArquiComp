#include <stdio.h>
#include <pthread.h>
#include "Processor.cpp"
#include "Bus.cpp"
#include "Memory.cpp"

// Definiciones editables
#define NUM_PROCS 2
#define QUANTUM 16

pthread_barrier_t  synchroBarrier;
Memory *mainMemory;
Bus *instrBus, *dataBus;

void *threadProcessor(void *paramPtr){
      Processor* proc = new Processor(instrBus,dataBus);
      int idThread = (int)paramPtr;

      printf("Procesador No�%i\n",idThread);

      /*while(!proc->getFin()){
        proc->execute();
        
        // FALTA MUCHO CODIGO
        
        pthread_barrier_wait (&synchroBarrier);
      }*/
      
      delete proc;
}

int main(){
    mainMemory = new Memory();
    instrBus = new Bus(mainMemory->ramInstructions);
    dataBus = new Bus(mainMemory->ramData);
    pthread_t *thread = new pthread_t[NUM_PROCS];
    
    pthread_barrier_init (&synchroBarrier, NULL, NUM_PROCS);
    printf("Hilo root ejecutandose\n");
    
    // BEGIN MULTITHREAD
    for(int i=0;i<NUM_PROCS;i++){
        data.idThread=i;
        pthread_create(&thread[i],NULL,threadProcessor,(void*)i);
    }
    // END MULTITHREAD
    for(int i=0;i<NUM_PROCS;i++){
        pthread_join(thread[i],NULL);
    }

    printf("Emulacion finalizada\n");
    
    delete[] thread;
    delete dataBus;
    delete instrBus;
    delete mainMemory;
    return 0;
}
