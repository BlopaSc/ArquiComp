#include <stdio.h>
#include <pthread.h>
#include "ThreadQueue.cpp"
#include "Processor.cpp"
#include "Bus.cpp"
#include "Memory.cpp"

// Definiciones editables
#define NUM_PROCS 2
#define QUANTUM 16

using namespace std;
pthread_barrier_t  synchroBarrier;
Memory *mainMemory;
Bus *instrBus, *dataBus;
ThreadQueue *threadManager;
pthread_t *thread;
int instructionsProcessed;


void *threadProcessor(void *paramPtr){
      Processor* proc = new Processor(instrBus,dataBus);
      int idThread = (int)paramPtr;

      printf("Procesador Noº%i\n",idThread);

      /*while(!proc->getFin()){
        proc->execute();
        
        // FALTA MUCHO CODIGO
        
        pthread_barrier_wait (&synchroBarrier);
      }*/
      
      delete proc;
}

void run(){
    printf("Emulacion iniciando\n");
    // BEGIN MULTITHREAD
    for(int i=0;i<NUM_PROCS;i++){
        pthread_create(&thread[i],NULL,threadProcessor,(void*)i);
    }
    // END MULTITHREAD
    for(int i=0;i<NUM_PROCS;i++){
        pthread_join(thread[i],NULL);
    }
    printf("Emulacion finalizada\n");
}

void loadFile(char * filename){
    FILE *ptrFile;
    int codigo,p1,p2,p3;
    printf("Opening file: %s\n",filename);
    ptrFile = fopen(filename,"r");
    while(fscanf(ptrFile,"%i %i %i %i\n",&codigo,&p1,&p2,&p3) != EOF){
        printf("Read: %d %d %d %d \n",codigo, p1, p2, p3);
        mainMemory->ramInstructions[instructionsProcessed]=codigo;
        instructionsProcessed++;
        mainMemory->ramInstructions[instructionsProcessed]=p1;
        instructionsProcessed++;
        mainMemory->ramInstructions[instructionsProcessed]=p2;
        instructionsProcessed++;
        mainMemory->ramInstructions[instructionsProcessed]=p3;
        instructionsProcessed++;
    }
    fclose(ptrFile);
}

void displayMemory(){
     printf("Despliegue de memoria: \n");
     for(int i=0;i<instructionsProcessed;i++){
         printf("%d ",mainMemory->ramInstructions[i]);            
     }
     printf("\n");
}


int main(){
    mainMemory = new Memory();
    instrBus = new Bus(mainMemory->ramInstructions);
    dataBus = new Bus(mainMemory->ramData);
    threadManager = new ThreadQueue();
    thread = new pthread_t[NUM_PROCS];
    instructionsProcessed = 0;
    
    loadFile((char *)"1.txt");
    loadFile((char *)"2.txt");
    displayMemory();
    
    
    pthread_barrier_init (&synchroBarrier, NULL, NUM_PROCS);
    printf("Hilo root ejecutandose\n");
    
    run();
    
    delete[] thread;
    delete dataBus;
    delete instrBus;
    delete mainMemory;
    return 0;
}
