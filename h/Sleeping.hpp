#ifndef OS1_SLEEPING_HPP
#define OS1_SLEEPING_HPP
#include "Queue.hpp"

class Sleeping{
public:
    static void put(TCB* tcb);
    static void timerUpdate();
private:
    static Queue sleepingQueue;
};

#endif
