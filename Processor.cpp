#ifndef PROCESSOR_CPP
#define PROCESSOR_CPP
#include "State.cpp"
#include "Instructions.cpp"
#include "Cache.cpp"
extern bool verbose;
// Clase que emula un procesador
class Processor{
    private:
        long long cycles;
        unsigned char flags;
        unsigned* instruction;
        int idProcessor;
        bool success;
    public:
           State* state;
           Instructions* instr;
           Cache *cacheData,*cacheInstr;
        // Constructor
        Processor(Bus *instrBus,Bus *dataBus,int id){
            cycles=1;
            flags=0x0;
            state = 0;
            instr = new Instructions();
            cacheInstr = new Cache(instrBus,0x4,id);
            cacheData = new Cache(dataBus,0x1,id);
            idProcessor=id;
            instruction = new unsigned[4];
        }
        // Destructor
        ~Processor(){
            delete instr;
            delete cacheData;
            delete cacheInstr;
            if(state){
                delete state;
            }
            delete[] instruction;
        }
        // Retorna si el procesador se encuentra en un estado de fin
        inline unsigned char getFin(){return flags&0x1;}
        // Retorna el ciclo actual del procesador para fines del quantum
        inline long long getCycle(){return cycles;}
        // Retorna el estado actual del procesador para almacenarlo
        inline State* getState(){return state;}
        inline void setState(State* s){state=s;}
        //Switch de las instrucciones MIPS
        void ejecutarMIPS(int codigo, int p1,int p2, int p3){
             switch(codigo){
               case 8:
                    instr->DADDI(state,p2,p1,p3);
                    break;
               case 32:
                    instr->DADD(state,p3,p1,p2);
                    break;
               case 34:
                    instr->DSUB(state,p3,p1,p2);
                    break;
               case 12:
                    instr->DMUL(state,p3,p1,p2);
                    break;
               case 14:
                    instr->DDIV(state,p3,p1,p2);
                    break;
               case 4:
                    instr->BEQZ(state,p1,p3);
                    break;
               case 5:
                    instr->BNEZ(state,p1,p3);
                    break;
               case 3:
                    instr->JAL(state,p3);
                    break;
               case 2:
                    instr->JR(state,p1);
                    break;
               case 63:
                    if(verbose){printf("\n");}
                    flags|=0x1;
                    break;
               case 35:
                    instr->LW(state,cacheData,p2,p3,p1);
                    break;
               case 43:
                    instr->SW(state,cacheData,p2,p3,p1);
                    break;
               case 50: // LL
                    
                    break;
               case 51: // SC
                    
                    break;
             }
        }
        
        
        // Es la señal que ocasiona la ejecucion de un nuevo ciclo de reloj
        void execute(){
             if(state){
                    // Si se tiene cargado un estado ejecuta
                    success = cacheInstr->getData(instruction,state->pc);
                    if(success){
                        if(verbose){printf("Proc %i, PC: %i, Instr: %i %i %i %i \t",idProcessor,state->pc,instruction[0],instruction[1],instruction[2],instruction[3]);}
                        state->pc += 0x4;
                        state->counter++;
                        ejecutarMIPS(instruction[0],instruction[1],instruction[2],instruction[3]);
                        cycles++;
                    }else{
                        if(verbose){printf("Proc %i: Waiting for bus\n",idProcessor);}
                    }
             }else{
                    if(verbose){printf("No-op\n");}
             }
        }
        
        // Es llamado cuando un hilo ha acabado y debe ser eliminado y las banderas reiniciadas
        void finishState(){
             cycles=1;
             state=0;
             flags=0x0;
        }
        
};
#endif
