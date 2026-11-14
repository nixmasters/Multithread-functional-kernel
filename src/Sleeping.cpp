#include "../h/Sleeping.hpp"
#include "../h/Scheduler.hpp"
#include "../h/Riscv.hpp"

Queue Sleeping::sleepingQueue;

void Sleeping::put(TCB *tcb) {
    tcb->setRunnable(false);
    sleepingQueue.insertSleep(tcb);
}

void Sleeping::timerUpdate() {
    while (sleepingQueue.peek() && Riscv::getCurrentTime() >= sleepingQueue.peek()->getSleepTime()) {
        TCB* tcb = sleepingQueue.remove();
        tcb->setSleepTime(0);
        Scheduler::put(tcb);
    }
}
