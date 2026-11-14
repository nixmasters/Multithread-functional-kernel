#include "../h/TCB.hpp"
#include "../h/Riscv.hpp"
#include "../h/MySemaphore.hpp"
#include "../h/Console.hpp"
#include "../h/Scheduler.hpp"
#include "../h/Sleeping.hpp"

uint64 Riscv::systemTimer = 0;
uint64 Riscv::timeSliceCounter = 0;

void Riscv::popSppSpie() {
    if(TCB::running->getPrivilege() == TCB::SUPERVISOR) ms_sstatus(SSTATUS_SPP);
    else mc_sstatus(SSTATUS_SPP);
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleConsoleInterrupt() {
    int irq = plic_claim();
    if(irq == CONSOLE){
        Console::getCharacters();
    }
    plic_complete(irq);
}

void Riscv::handleTimerInterrupt() {
    uint64 volatile sepc = r_sepc();
    uint64 volatile sstatus = r_sstatus();
    mc_sip(SIP_SSIP);

    incCurrentTime();
    Sleeping::timerUpdate();
    timeSliceCounter++;
    if (timeSliceCounter >= TCB::running->getTimeSlice()) {
        TCB::_thread_dispatch();
    }

    w_sstatus(sstatus);
    w_sepc(sepc);
}

extern int thread_exit();
void Riscv::handleSupervisorTrap() {
    uint64 savedSP = TCB::running->oldSP;
    TCB::running->oldSP = r_sscratch();
    
    uint64 scause = r_scause();
    if(scause == TIMER_INT) handleTimerInterrupt();
    else if(scause == CONSOLE_INT) handleConsoleInterrupt();
    else if(scause & (1ul << 63)) Riscv::stopEmulator(); // unexpected external interrupt
    else handleInternalInterrupts(scause);

    TCB::running->oldSP = savedSP;

}

void Riscv::handleInternalInterrupts(uint64 scause) {
    if(scause == USER_INT || scause == SYSTEM_INT){
        uint64 volatile sstatus = r_sstatus();
        uint64 volatile sepc = r_sepc() + 4;
        uint64 syscall = r_a_index_stack(0);
        switch (syscall) {
            case 0x01: MemoryAllocator::_mem_alloc(); break;
            case 0x02: MemoryAllocator::_mem_free(); break;
            case 0x03: MemoryAllocator::_mem_get_free_space(); break;
            case 0x04: MemoryAllocator::_mem_get_largest_free_block(); break;
            case 0x11: TCB::_thread_create(); break;
            case 0x12: TCB::_thread_exit(); break;
            case 0x13: TCB::_thread_dispatch(); break;
            case 0x21: MySemaphore::_sem_open(); break;
            case 0x22: MySemaphore::_sem_close(); break;
            case 0x23: MySemaphore::_sem_wait(); break;
            case 0x24: MySemaphore::_sem_signal(); break;
            case 0x31: TCB::_time_sleep(); break;
            case 0x35: TCB::_get_thread_id(); break;
            case 0x36: TCB::_join(); break;
            case 0x41: Console::_getc(); break;
            case 0x42: Console::_putc(); break;
        }
        w_sstatus(sstatus);
        w_sepc(sepc);
    }else{
        thread_exit();
        //stopEmulator();
    }
}

void Riscv::w_a0_stack(uint64 a0) {
    uint64 addr = TCB::running->oldSP + 80;
    __asm__ volatile("sd %[a0], 0(%[addr])" : : [a0] "r"(a0), [addr] "r"(addr));
}

uint64 Riscv::r_a_index_stack(int index) {
    uint64 addr = TCB::running->oldSP + 80 + 8*index;
    uint64 ret;
    __asm__ volatile("ld %[ret], 0(%[addr])" : [ret]"=r"(ret) : [addr]"r"(addr));
    return ret;
}
