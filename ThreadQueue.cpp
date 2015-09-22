#ifndef THREAD_QUEUE_CPP
#define THREAD_QUEUE_CPP
#include "State.cpp"
// Clase que se encarga de manejar los hilos que se ejecutaran en los procesadores, guarda los contextos
class QueueNode(){
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
}
class ThreadQueue{
    public:
        QueueNode *current;
        unsigned size;
        ThreadQueue(){
            size=0;
        }
        ~ThreadQueue(){
            if(size){
                current->prev->next=0;
                delete current;
            }
        }
};
#endif
