#ifndef OS1_MEMORYALLOCATOR_HPP
#define OS1_MEMORYALLOCATOR_HPP
#include "../lib/hw.h"

struct MemoryBlock {
    MemoryBlock* next;
    MemoryBlock* prev;
    size_t size;
    size_t dummy;
};

class MemoryAllocator {
public:
    //static MemoryAllocator* getInstance()
    //{
    //    static MemoryAllocator instance;
    //    return &instance;
    //}

    static size_t sizeFree;
    static void _mem_get_free_space();
    static void _mem_get_largest_free_block();


    static void* mem_alloc(size_t size);
    static int mem_free (void* ptr);

    static void initializeMemory();

    static void _mem_alloc();
    static void _mem_free();

private:
    static MemoryBlock* freeMemHead;
    static MemoryBlock* usedMemHead;

};

#endif