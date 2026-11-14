#include "../h/Riscv.hpp"
#include "../h/Scheduler.hpp"
#include "../h/MySemaphore.hpp"

MySemaphore::~MySemaphore() {
    TCB* tcb;
    while ((tcb = blocked.remove())) {
        tcb->setClosedSemaphore(true);
        Scheduler::put(tcb);
    }
}

uint64 MySemaphore::wait() {
    if (--val < 0) block();
    if(TCB::running->getClosedSemaphore()) return -1;
    else return 0;
}

void MySemaphore::block() {
    TCB::running->setRunnable(false);
    blocked.insert(TCB::running);
    TCB::_thread_dispatch();
}

void MySemaphore::unblock() {
    TCB* tcb = blocked.remove();
    if (tcb) {
        tcb->setClosedSemaphore(false);
        Scheduler::put(tcb);
    }
}

void MySemaphore::signal() {
    if (++val <= 0) unblock();
}

void* MySemaphore::operator new(size_t size) {
    return MemoryAllocator::mem_alloc(size);
}
void MySemaphore::operator delete(void* addr) noexcept{
    MemoryAllocator::mem_free(addr);
}

// ----- SYSTEM CALLS -----

void MySemaphore::_sem_open() {
    MySemaphore** handle = (MySemaphore**) Riscv::r_a_index_stack(1);
    uint64 val = Riscv::r_a_index_stack(2);
    (*handle) = new MySemaphore((int)val);

    if(!(*handle)){
        Riscv::w_a0_stack(-1);
        return;
    }

    Riscv::w_a0_stack(0);
}

void MySemaphore::_sem_close() {
    MySemaphore* handle = (MySemaphore*) Riscv::r_a_index_stack(1);
    if(!handle){
        Riscv::w_a0_stack(-1);
        return;
    }

    delete handle;
    Riscv::w_a0_stack(0);
}

void MySemaphore::_sem_wait() {
    MySemaphore* handle = (MySemaphore*) Riscv::r_a_index_stack(1);
    if(!handle){
        Riscv::w_a0_stack(-1);
        return;
    }
    uint64 ret = handle->wait();
    Riscv::w_a0_stack(ret);
}

void MySemaphore::_sem_signal() {
    MySemaphore* handle = (MySemaphore*) Riscv::r_a_index_stack(1);
    if (!handle) {
        Riscv::w_a0_stack(-1);
        return;
    }
    handle->signal();
    Riscv::w_a0_stack(0);
}

MySemaphore::MySemaphore(int init) {
    val = init;
}
