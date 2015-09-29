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
pthread_mutex_t lockQueue;
int instructionsProcessed;


void *threadProcessor(void *paramPtr){
      Processor* proc = new Processor(instrBus,dataBus);
      int idThread = (int)paramPtr;

      printf("Procesador Noº%i\n",idThread);

      while(threadManager->getSize()){
         if((!(proc->getCycle()%QUANTUM) || proc->getFin()) && threadManager->getSize()>=NUM_PROCS){
              pthread_mutex_lock(&lockQueue);
              if(proc->getState()){
                   if(proc->getFin()){
                       // Es porque acabo hilillo
                       threadManager->remove(proc->getState());
                       proc->finishState();
                   }else{
                       // Excedio QUANTUM
                       threadManager->returnThread(proc->getState());
                   }
              }
              proc->setState(threadManager->getNext());
              pthread_mutex_unlock(&lockQueue);
         }
         proc->execute();   
         pthread_barrier_wait (&synchroBarrier);
      }
      
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
    State *newState;
    int codigo,p1,p2,p3;
    bool startState=true;
    printf("Opening file: %s\n",filename);
    ptrFile = fopen(filename,"r");
    if(ptrFile!=NULL){
	    while(fscanf(ptrFile,"%i %i %i %i\n",&codigo,&p1,&p2,&p3) != EOF){
		if(startState){
	            startState=false;
	            newState = new State();
	            newState->pc = instructionsProcessed;
	            threadManager->add(newState);
		        printf("Position in memory: %i -- Thread noº: %i\n",newState->pc,threadManager->getSize());
	        }
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
    if (pthread_mutex_init(&lockQueue, NULL)){
        printf("\nAlgo salio mal creando el mutex del queue\n");
    }
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
