#ifndef OS1_TCB_HPP
#define OS1_TCB_HPP

#include "MemoryAllocator.hpp"
#include "syscall_c.hpp"

using Fun = void (*)(void*);

class TCB {
private:
    struct Context{
        uint64 ra;
        uint64 sp;
    };
public:

    TCB(Fun fun, void* args, void* stack = MemoryAllocator::mem_alloc(DEFAULT_STACK_SIZE), uint64 timeSlice = DEFAULT_TIME_SLICE);

    static TCB* running;

    enum Privilege{
        USER, SUPERVISOR, MACHINE
    };
    static int currID;
    int id;

    void setPrivilege(Privilege privilege) { this->privilege = privilege; }
    Privilege getPrivilege(){ return privilege; }

    uint64 getTimeSlice() const{ return timeSlice; }

    void setRunnable(bool runnable){ this->runnable = runnable; }
    bool isRunnable() const{ return this->runnable; }

    void setFinished(){ finished = true; }
    bool isFinished() const{ return finished; }

    void setSleepTime(time_t time){ sleepTime = time; }
    time_t getSleepTime() const{ return sleepTime; }

    void setWaitTime(time_t time){ waitTime = time; }
    time_t getWaitTime() const{ return waitTime; }

    void setSemTimeout(bool val){ semTimeout = val; }
    bool getSemTimeout() const{ return semTimeout; }

    void setClosedSemaphore(bool val){ closedSemaphore = val; }
    bool getClosedSemaphore() const{ return closedSemaphore; }

    uint64 oldSP = 0;

    ~TCB();

    static void time_sleep(time_t timeout);

    static void _thread_dispatch();
    static void _thread_create();
    static void _thread_exit();
    static void _time_sleep();
    static void _get_thread_id();
    static void _join();

    static void switchContexts(TCB::Context* oldContext, TCB::Context* newContext);

    void* operator new(size_t size);
    void operator delete(void *ptr);

    friend class Queue;

private:
    static void thread_wrapper();

    TCB* next = nullptr;
    TCB* nextTime = nullptr;

    bool runnable = true;
    bool finished = false;
    time_t sleepTime = 0;
    time_t waitTime = 0;
    uint64 timeSlice = DEFAULT_TIME_SLICE;

    Fun fun;
    uint64* stack;
    void* args;
    Context context;

    Privilege privilege = USER;
    bool semTimeout = false;
    bool closedSemaphore = false;
};

#endif