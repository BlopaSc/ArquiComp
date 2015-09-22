#ifndef BUS_CPP
#define BUS_CPP
#include "Memory.cpp"
class Bus{
    private:
        unsigned* mem;
    public:
        // Constructor
        Bus(unsigned* m){
            mem=m;
        }
        // Destructor
        ~Bus(){
        }
};
#endif
