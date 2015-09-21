#ifndef BUS_CPP
#define BUS_CPP
#include "Memory.cpp"
class Bus{
    private:
        Memory* mem;
    public:
        // Constructor
        Bus(Memory* m){
            mem=m;
        }
        // Destructor
        ~Bus(){
        }
};
#endif
