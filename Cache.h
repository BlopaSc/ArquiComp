#ifndef CACHE_H
#define CACHE_H
#include <pthread.h>
class Bus;
class Cache{
    private:
        // Almacena el bus con el que se comunicara
        Bus* bus;
        // Guarda los tags de los caches y la data del cache
        unsigned* tag;
        char* status;
        unsigned** cache;
        // Contadores con fines estadisticos
        unsigned hitCounter,missCounter,multi;
        int idProcessor,blockInvalidate;
        // Se encarga de realizar un writeback de un bloque
        void writeback(unsigned block);
    public: 
        // Locks del cache
        pthread_mutex_t cacheLock;
        pthread_mutex_t noDeadLock; // Mutex que sirve para evitar deadlock en cache
        bool cacheTaken;
        // Constructor : multiplier se utiliza para la cache de instrucciones que la estamos trabajando como extendida
        Cache(Bus* b,unsigned multiplier,int id);
        // Destructor
        ~Cache();
        // Revisa si la data se encuentra disponible, de no ser asi la trae de memoria y la devuelve
        bool getData(int *data,int pos);
        // Se encarga de almacenar un dato en una posicion de memoria
        bool saveData(int data,int pos);
        // Se encarga de invalidar un bloque cuando recibe la notificacion del bus
        void invalidateBlock(unsigned blockNumber);
        // Se encarga de enviar cualquier señal de invalidacion que haga falta
        void signalInvalidate();
        // Recibe solicitud para ver si bloque esta modificado
        bool checkModified(unsigned blockNumber);
        // Recibe una solicitud de writeback
        void requestWriteback(unsigned blockNumber,int idCaller);
};
#endif
