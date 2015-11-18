#ifndef MEMORY_CPP
#define MEMORY_CPP
#define WORDS_PER_BLOCK 4
#define BLOCKS_INSTR 40
#define BLOCKS_DATA 88
#define DEFAULT_MEMORY_START 1
// Clase encargada de la memoria del sistema
class Memory{
    public:
        // Punteros a memoria
        unsigned *ramInstructions,*ramData;
        // Constructor
        Memory(){
            int i,size=(BLOCKS_INSTR<<2)*WORDS_PER_BLOCK;
            // Crea la ram de instrucciones
            ramInstructions = new unsigned[size];
            for(int i=0;i<size;i++){ramInstructions[i] = DEFAULT_MEMORY_START;}
            // Crea la ram de datos
            size = BLOCKS_DATA*WORDS_PER_BLOCK;
            ramData = new unsigned[size];
            for(int i=0;i<size;i++){ramData[i] = DEFAULT_MEMORY_START;}
        }
        // Destructor
        ~Memory(){
            delete[] ramInstructions;
            delete[] ramData;
        }
        // Revisa si hay algun dato modificado (no default) en la memoria de datos
        bool modifiedData(){
            bool notModified=true;
            for(int i=0;i<BLOCKS_DATA*WORDS_PER_BLOCK && notModified;i++){
                notModified = ramData[i]==DEFAULT_MEMORY_START;
            }
            return !notModified;
        }
};
#endif
