#ifndef THREAD_QUEUE_CPP
#define THREAD_QUEUE_CPP
#include "State.cpp"
// Clase que se encarga de manejar los hilos que se ejecutaran en los procesadores, guarda los contextos
class QueueNode{
    public:
        QueueNode *next,*prev;
        State *state;
        QueueNode(QueueNode *n,QueueNode *p,State *s){
            next=n;
            prev=p;
            state=s;
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
    public:
        ThreadQueue(){
            size=0;
            current=0;
        }
        ~ThreadQueue(){
            if(current){
                current->prev->next=0;
                delete current;
            }
        }
        inline unsigned getSize(){return size;}
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
        }
        State* getNext(){
            current = current->next;
            return current->prev->state;
        }
        void remove(State *state){
            QueueNode *tmp = current;
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
                delete tmp;
                size--;
                if(!size){
                    current=0;
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
        printf("PC: %i\n",test->pc);
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
