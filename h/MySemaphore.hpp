#ifndef OS1_MYSEMAPHORE_HPP
#define OS1_MYSEMAPHORE_HPP

#include "TCB.hpp"
#include "Queue.hpp"

class MySemaphore {
public:
    MySemaphore(int init = 0);

    static void semaphoreTimerUpdate();
    int value() const{ return val; }

    enum SemaphoreConstants{
        SEMDEAD = -1UL,
        TIMEOUT = -2UL
    };

    uint64 wait();
    void signal();

    void* operator new(size_t size);
    void operator delete(void *addr) noexcept;

    ~MySemaphore();

    static void _sem_open();
    static void _sem_close();
    static void _sem_wait();
    static void _sem_signal();

private:
    void block();
    void unblock();

    int val;
    Queue blocked;
};

#endif