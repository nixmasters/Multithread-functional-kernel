#include "../h/syscall_cpp.hpp"

void* operator new(size_t in_bytes) {
    return mem_alloc(in_bytes);
}
void operator delete(void* addr) {
    mem_free(addr);
}

Thread::Thread(void (*body)(void*), void* args) : myHandle(nullptr), body(body), args(args){}

Thread::~Thread() {
    delete myHandle;
    myHandle = nullptr;
}

int Thread::getThreadID() {
    return get_thread_id();
}

void Thread::join() {
    if (myHandle) {
        thread_join(&myHandle);
    }

}

int Thread::start() {
    if (!myHandle) thread_create(&myHandle, body, args);
    return 0;
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

Thread::Thread() : myHandle(nullptr), body(Thread::thread_wrapper), args(this){}

void Thread::thread_wrapper(void *args) {
    if(args){
        Thread* thread = (Thread*) args;
        thread->run();
    }
}

Semaphore::Semaphore(unsigned init) {
    sem_open(&handle, init);
}

Semaphore::~Semaphore() {
    sem_close(handle);
}

int Semaphore::wait() {
    return sem_wait(handle);
}

int Semaphore::signal() {
    return sem_signal(handle);
}

PeriodicThread::PeriodicThread(time_t period) : Thread(PeriodicThread::periodic_thread_wrapper, this){
    this->period = period;
}

void PeriodicThread::periodic_thread_wrapper(void* args) {
    PeriodicThread* thread = (PeriodicThread*) args;
    while (thread->period != -1UL) {
        thread->periodicActivation();
        Thread::sleep(thread->period);
    }
}

void PeriodicThread::terminate() {
    this->period = -1UL;
}

char Console::getc() {
    return ::getc();
}
void Console::putc(char c) {
    ::putc(c);
}