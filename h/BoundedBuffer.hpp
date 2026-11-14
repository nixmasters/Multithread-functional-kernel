#ifndef OS1_BOUNDEDBUFFER_HPP
#define OS1_BOUNDEDBUFFER_HPP

#include "MemoryAllocator.hpp"
#include "MySemaphore.hpp"
class MySemaphore;

class BoundedBuffer {
public:
    BoundedBuffer(){
        //mutex = MySemaphore(1);
        itemAvailable =  MySemaphore(bufferSize);
        itemUsed = MySemaphore(0);
    }

    void put(char c){
        itemAvailable.wait();
        //mutex.wait();
        buffer[tail] = c;
        tail = (tail + 1) % bufferSize;
        //mutex.signal();
        itemUsed.signal();
    }
    char get(){
        itemUsed.wait();
        //mutex.wait();
        char c = buffer[head];
        head = (head + 1) % bufferSize;
        //mutex.signal();
        itemAvailable.signal();
        return c;
    }

    bool isEmpty(){
        return head == tail;
    }

    // memory allocation
    void* operator new(size_t size) {
        return MemoryAllocator::mem_alloc(size);
    }
    void operator delete(void* addr) noexcept{
        MemoryAllocator::mem_free(addr);
    }
/*    void* operator new[](size_t size) {
        return MemoryAllocator::mem_alloc(size);
    }
    void operator delete[](void* addr) noexcept{
        MemoryAllocator::mem_free(addr);
    }
*/
private:
    static const int bufferSize = 1024;
    char buffer[bufferSize];
    //MySemaphore mutex;
    MySemaphore itemAvailable;
    MySemaphore itemUsed;
    int head = 0;
    int tail = 0;
};

#endif