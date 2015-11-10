#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_PROCS 2

pthread_barrier_t  synchroBarrier;
pthread_t *thread;
pthread_mutex_t lockQueue;
int instructionsProcessed,quantum,m,b,clockCounter;
bool modoLento,verbose;

#include "ThreadQueue.cpp"
#include "Processor.cpp"
#include "Bus.cpp"
#include "Memory.cpp"

Memory *mainMemory;
Bus *instrBus, *dataBus;
ThreadQueue *threadManager;
State** results;


void *threadProcessor(void *paramPtr){
      int idThread = (int)paramPtr;
      Processor* proc = new Processor(instrBus,dataBus,idThread);
      if(idThread){
          printf("Procesador No.%i\n",idThread);
          pthread_mutex_lock(&lockQueue);
          if(idThread<=threadManager->getSize()){
              proc->setState(threadManager->getNext());
          }
          pthread_mutex_unlock(&lockQueue);
      }
      // Se sincroniza para empezar
      pthread_barrier_wait (&synchroBarrier);
      // Mientras quede algun hilillo por ejecutar
      while(threadManager->getSize()){
         pthread_barrier_wait (&synchroBarrier);
         if(idThread){
             // Si se excede el quantum o llega al fin del hilillo, y quedan mas hilos disponibles
             if(proc->getFin()){
                 // Acabo hilo
                 pthread_mutex_lock(&lockQueue);
                 results[proc->getState()->id-1] = proc->getState();
                 threadManager->remove(proc->getState());
                 proc->finishState();
                 if(threadManager->getLeftUntaken()){
                        proc->setState(threadManager->getNext());
                 }
                 pthread_mutex_unlock(&lockQueue);
             }else{
                    pthread_mutex_lock(&lockQueue);
                    if(!(proc->getCycle()%quantum) && threadManager->getLeftUntaken()){
                        threadManager->returnThread(proc->getState());
                        proc->setState(threadManager->getNext());
                    }
                    pthread_mutex_unlock(&lockQueue);
             }
         }else{
             clockCounter++;
             if(verbose){
                 printf("Ciclo -- %i",clockCounter);
             }
             if(modoLento){
                char c[2];
                scanf("%c",c);
             }
         }
         pthread_barrier_wait (&synchroBarrier);
         if(idThread){proc->execute();}
      }
      if(idThread){
          printf("Fin Processor No.%i\n",idThread);
      }else{
          printf("Ciclos realizados: %i\n",clockCounter);
      }
      delete proc;
}

void run(){
    printf("Emulacion iniciando\n");
    // BEGIN MULTITHREAD
    for(int i=0;i<NUM_PROCS+1;i++){
        pthread_create(&thread[i],NULL,threadProcessor,(void*)i);
    }
    // END MULTITHREAD
    for(int i=0;i<NUM_PROCS+1;i++){
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
	            newState->id = threadManager->getSize();
		        printf("Position in memory: %i -- Thread no: %i\n",newState->pc,threadManager->getSize());
	        }
	        if(verbose){printf("Read: %d %d %d %d \n",codigo, p1, p2, p3);}
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
         if((i&0xF) == 15){
            printf("\n");
         }else{
            if((i&0x3) == 3){printf("\t");}
         }
     }
     printf("\n");
}


int main(int argc,char *argv[]){
    mainMemory = new Memory();
    instrBus = new Bus(mainMemory->ramInstructions);
    dataBus = new Bus(mainMemory->ramData);
    threadManager = new ThreadQueue();
    thread = new pthread_t[NUM_PROCS+1];
    if (pthread_mutex_init(&lockQueue, NULL)){
        printf("\nAlgo salio mal creando el mutex del queue\n");
    }
    instructionsProcessed = 0;
    clockCounter=0;
    verbose=false;
    
    if(argc == 1){
        char* input = new char[0x10000];
        printf("Desea activar el modo lento? (s/n): ");
        scanf("%s",input);
        modoLento = input[0]=='s';
        verbose = modoLento;
        printf("De que tamano desea el quantum?: ");
        scanf("%i",&quantum);
        printf("De que tamano desea el m?: ");
        scanf("%i",&m);
        printf("De que tamano desea el b?: ");
        scanf("%i",&b);
        printf("Escriba los nombres de los archivos a ejecutar\nPara finalizar introduzca solo 'e':\n");
        do{
            scanf("%s",input);
            if(input[0]!='e' && input[1]!='\0'){
                loadFile(input);
            }
        }while(input[0]!='e' && input[1]!='\0');
        delete[] input;
    }else{
        // Cargar de argumentos
        modoLento = argv[1][0]=='t';
        if(argv[1][1]!='\0'){
           verbose = argv[1][1]=='r';
        }else{
           verbose = modoLento;
        }
        quantum = atoi(argv[2]);
        m = atoi(argv[3]);
        b = atoi(argv[4]);
        for(int i=5;i<argc;i++){
            loadFile(argv[i]);
        }
    }
    quantum++;
    displayMemory();
    int resultSize=threadManager->getSize();
    results = new State*[resultSize];
    
    pthread_barrier_init (&synchroBarrier, NULL, NUM_PROCS+1);
    printf("Hilo root ejecutandose\n");
    
    run();
    
    printf("Resultados de los hilos:\n");
    for(int i=0;i<resultSize;i++){
        results[i]->printState();
        delete results[i];
    }
    
    delete[] results;
    delete[] thread;
    delete dataBus;
    delete instrBus;
    delete mainMemory;
    return 0;
}
