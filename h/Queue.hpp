#ifndef OS1_QUEUE_HPP
#define OS1_QUEUE_HPP

#include "MemoryAllocator.hpp"
#include "TCB.hpp"

class Queue {
public:
    Queue();

    void insert(TCB* tcb);
    TCB* remove();

    bool isEmpty();
    TCB* peek();

    void insertSleep(TCB* tcb);
    TCB* removeSleep();

private:
    TCB* head;
    TCB* tail;
};

#endif