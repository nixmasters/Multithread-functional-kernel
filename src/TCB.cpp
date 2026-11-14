#include "../h/Riscv.hpp"
#include "../h/Scheduler.hpp"
#include "../h/Console.hpp"
#include "../h/Sleeping.hpp"

TCB* TCB::running = nullptr;
int TCB::currID=0;

extern int thread_exit();

TCB::TCB(Fun fun, void *args, void* stack, uint64 timeSlice){
    this->fun = fun;
    this->args = args;
    this->stack = (uint64*) stack;
    this->timeSlice = timeSlice;
    context.ra = (uint64) thread_wrapper;
    context.sp = (uint64)((char*)stack + DEFAULT_STACK_SIZE);
    id = ++currID;
}

TCB::~TCB() {
    next = nullptr;
    nextTime = nullptr;
    MemoryAllocator::mem_free(stack);
    stack = nullptr;
}

void TCB::thread_wrapper() {
    Riscv::popSppSpie();
    running->fun(running->args);
    thread_exit();
    /*running->setFinished();
    running->setRunnable(false);
    _thread_dispatch(); */
}

void *TCB::operator new(size_t size) {
    return MemoryAllocator::mem_alloc(size);
}
void TCB::operator delete(void *addr) {
    MemoryAllocator::mem_free(addr);
}

void TCB::_thread_dispatch() {
    Riscv::resetTimeSliceCounter();
    TCB* old = running;
    if (old->isRunnable()) Scheduler::put(old);
    running = Scheduler::get();
    switchContexts(&old->context, &running->context);
}

void TCB::_thread_create() {
    void* stack = (void*) Riscv::r_a_index_stack(4);
    void* args = (void*) Riscv::r_a_index_stack(3);
    Fun fun = (Fun) Riscv::r_a_index_stack(2);
    TCB** threadHandle = (TCB**) Riscv::r_a_index_stack(1);
    (*threadHandle) = new TCB(fun, args, stack, DEFAULT_TIME_SLICE);
    if(*threadHandle){
        (*threadHandle)->setRunnable(true);
        Scheduler::put(*threadHandle);
        Riscv::w_a0_stack(0);
    }else Riscv::w_a0_stack(-1); // memory allocation failed
}

void TCB::_time_sleep() {
    time_t timeout = Riscv::r_a_index_stack(1);
    time_sleep(timeout);
    _thread_dispatch();
    Riscv::w_a0_stack(0);
}
void TCB::_get_thread_id() {
    int ret = TCB::running->id;
    _thread_dispatch();
    Riscv::w_a0_stack(ret);
}
void TCB::_join() {

    TCB** handle = (TCB**) Riscv::r_a_index_stack(1);
    while (!(*handle)->isFinished()) {
        _thread_dispatch();
    }
}


void TCB::time_sleep(time_t timeout) {
    running->setSleepTime(timeout + Riscv::getCurrentTime());
    Sleeping::put(running);
}

void TCB::_thread_exit() {
    running->setRunnable(false);
    running->setFinished();
    _thread_dispatch();
}
