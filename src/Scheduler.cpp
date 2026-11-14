#include "../h/Riscv.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/Queue.hpp"
#include "../h/Scheduler.hpp"

Queue Scheduler::ready;

void Scheduler::put(TCB *tcb) {
    ready.insert(tcb);
}

TCB* Scheduler::get() {
    TCB* tcb = ready.remove();
    tcb->setRunnable(true);
    return tcb;
}
