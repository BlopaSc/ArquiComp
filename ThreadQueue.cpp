#ifndef THREAD_QUEUE_CPP
#define THREAD_QUEUE_CPP
#include "State.cpp"
// Clase que se encarga de manejar los hilos que se ejecutaran en los procesadores, guarda los contextos
class QueueNode{
    public:
        QueueNode *next,*prev;
        State *state;
        bool taken;
        QueueNode(QueueNode *n,QueueNode *p,State *s){
            next=n;
            prev=p;
            state=s;
            taken=false;
        }
        ~QueueNode(){
            if(next){
                delete next;
            }
        }
};
class ThreadQueue{
    private:
        QueueNode *current;
        unsigned size;
        unsigned leftUntaken;
    public:
        ThreadQueue(){
            size=0;
            leftUntaken=0;
            current=0;
        }
        ~ThreadQueue(){
            if(current){
                current->prev->next=0;
                delete current;
            }
        }
        inline unsigned getSize(){return size;}
        inline unsigned getLeftUntaken(){return leftUntaken;}
        // Agrega un nuevo hilo al manejador de hilos
        void add(State *state){
            if(current){
                current = new QueueNode(current,current->prev,state);
                current->prev->next = current;
                current->next->prev = current;
            }else{
                current = new QueueNode(0,0,state);
                current->next = current;
                current->prev = current;
            }
            size++;
            leftUntaken++;
        }
        // Todas estas funciones necesitan usarse en seccion critica
        // Retorna el siguiente hilo disponible al procesador
        State* getNext(){
            QueueNode *tmp = current;
            State *returnState=0;
            if(current){
                while(tmp->taken && tmp->next!=current){
                    tmp = tmp->next;
                }
                if(!tmp->taken){
                    current = tmp;
                    current->taken=true;
                    leftUntaken--;
                    current = current->next;
                    returnState = current->prev->state;
                }
            }
            return returnState;
        }
        // Marca un hilo como disponible cuando se libera para tomar otro y este no ha acabado
        void returnThread(State *state){
            QueueNode *tmp = current;
            while(tmp->next!=current && tmp->state!=state){
                tmp = tmp->next;
            }
            if(tmp->state==state){
                tmp->taken=false;
                leftUntaken++;
            }
        }
        // Se utiliza para remover un hilo finalizado de la lista
        void remove(State *state){
            QueueNode *tmp = current;
            if(current){
                do{
                    tmp = tmp->next;
                }while(tmp!=current && tmp->state!=state);
                if(tmp->state==state){
                    if(tmp==current){
                        current = tmp->next;
                    }
                    tmp->prev->next = tmp->next;
                    tmp->next->prev = tmp->prev;
                    tmp->next=0;
                    if(!tmp->taken){
                        leftUntaken--;
                    }
                    delete tmp;
                    size--;
                    if(!size){
                        current=0;
                    }
                }
            }
        }
};
#endif


// Test de la clase


/*
#include <stdio.h>
int main(){
    printf("Empezando test: \n");
    ThreadQueue* hilos = new ThreadQueue();
    printf("Size %i\n",hilos->getSize());
    State* test;
    for(int i=0;i<5;i++){
        test = new State();
        test->pc = i+1;
        hilos->add(test);
        printf("Size %i\n",hilos->getSize());
    }
    printf("\n");
    for(int j=0;j<11;j++){
        test = hilos->getNext();
        hilos->returnThread(test);
        if(test){
            printf("PC: %i\n",test->pc);
        }else{
            printf("State 0\n");
        }
    }
    printf("\n");
    for(int i=0;i<5;i++){
        hilos->remove(test);
        if(hilos->getSize()){
            test = hilos->getNext();
        }
        printf("Size %i\n",hilos->getSize());
    }
    delete hilos;
    
    char c[2];
    printf("Finalizando test ");
    scanf("%c",c);
    return 0;
}
*/
