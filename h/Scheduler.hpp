#ifndef OS1_SCHEDULER_HPP
#define OS1_SCHEDULER_HPP

class TCB;
class Queue;

class Scheduler {
public:
    static void put(TCB* tcb);
    static TCB* get();

private:
    static Queue ready;
};

#endif