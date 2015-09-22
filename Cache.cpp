#ifndef CACHE_CPP
#define CACHE_CPP
#include "Bus.cpp"
#define BLOCKS_PER_CACHE 8
class Cache{
    private:
        Bus* bus;
        unsigned* cache;
    public: 
        Cache(Bus* b,unsigned multiplier){
            cache = new unsigned[BLOCKS_PER_CACHE*WORDS_PER_BLOCK*multiplier];
            bus = b;
        }
        ~Cache(){
            delete[] cache;
        }
};
#endif
