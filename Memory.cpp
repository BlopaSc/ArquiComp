#ifndef MEMORY_CPP
#define MEMORY_CPP
#define WORDS_PER_BLOCK 4
#define BLOCKS_INSTR 40
#define BLOCKS_DATA 88
// Clase encargada de la memoria del sistema
class Memory{
    public:
        // Punteros a memoria
        unsigned *ramInstructions,*ramData;
        // Constructor
        Memory(){
            ramInstructions = new unsigned[(BLOCKS_INSTR<<2)*WORDS_PER_BLOCK];
            ramData = new unsigned[BLOCKS_DATA*WORDS_PER_BLOCK];
        }
        // Destructor
        ~Memory(){
            delete[] ramInstructions;
            delete[] ramData;
        }
};
#endif
