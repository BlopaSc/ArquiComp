#ifndef MEMORY_CPP
#define MEMORY_CPP
// Clase encargada de la memoria del sistema
class Memory{
    private:
        // Tamaño de la memoria, puntero a memoria
        unsigned wordsPerBlock,blocks,words;
        unsigned* ram;
    public:
        // Constructor
        Memory(unsigned wpb,unsigned b){
            wordsPerBlock = wpb;
            blocks = b;
            words = wpb*b;
            if(words){
                ram = new unsigned[words];
            }else{
                ram = 0;
            }
        }
        // Destructor
        ~Memory(){
            if(ram){
                delete[] ram;
            }
        }
};
#endif
