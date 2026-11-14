#ifndef OS1_RISCV_HPP
#define OS1_RISCV_HPP

#include "../lib/hw.h"

class Riscv {
public:
    static const int CONSOLE = 10;
    static const unsigned long SIP_SSIP = (1ul << 1);

    enum SCAUSE {
        TIMER_INT = (1ul << 63) | 1,
        CONSOLE_INT = (1ul << 63) | 9,
        USER_INT = 8,
        SYSTEM_INT = 9,
    };

    enum SSTATUS{
        SSTATUS_SIE = (1ul << 1),
        SSTATUS_SPIE = (1ul << 5),
        SSTATUS_SPP = (1ul << 8)
    };

    static void resetTimeSliceCounter() { timeSliceCounter = 0; }

    static void incTimeSliceCounter(){ timeSliceCounter++; }

    static time_t getCurrentTime();
    static void incCurrentTime() { systemTimer++; }

    static void popSppSpie();

    static void supervisorTrap();
    static void handleSupervisorTrap();

    static void handleInternalInterrupts(uint64 scause);
    static void handleTimerInterrupt();
    static void handleConsoleInterrupt();

    static void stopEmulator();

    static uint64 r_scause();
    static void w_stvec(uint64);
    static uint64 r_sscratch();

    static uint64 r_sepc();
    static void w_sepc(uint64);

    static uint64 r_sstatus();
    static void w_sstatus(uint64);

    static void ms_sstatus(uint64);
    static void mc_sstatus(uint64);

    static void ms_sip(uint64);
    static void mc_sip(uint64);

    static void w_a0_stack(uint64);
    static uint64 r_a_index_stack(int index);

private:
    static uint64 systemTimer;
    static uint64 timeSliceCounter;

};


inline uint64 Riscv::r_scause() {
    uint64 volatile scause;
    __asm__ volatile("csrr %0, scause" : "=r"(scause));
    return scause;
}
inline void Riscv::w_stvec(uint64 stvec) {
    __asm__ volatile("csrw stvec, %0" : : "r"(stvec));
}
inline uint64 Riscv::r_sscratch() {
    uint64 volatile sscratch;
    __asm__ volatile("csrr %0, sscratch" : "=r"(sscratch));
    return sscratch;
}

inline uint64 Riscv::r_sepc() {
    uint64 volatile sepc;
    __asm__ volatile("csrr %0, sepc" : "=r"(sepc));
    return sepc;
}
inline void Riscv::w_sepc(uint64 sepc) {
    __asm__ volatile("csrw sepc, %0" : : "r"(sepc));
}

inline uint64 Riscv::r_sstatus() {
    uint64 volatile sstatus;
    __asm__ volatile("csrr %0, sstatus" : "=r"(sstatus));
    return sstatus;
}
inline void Riscv::w_sstatus(uint64 sstatus) {
    __asm__ volatile("csrw sstatus, %0" : : "r"(sstatus));
}

inline void Riscv::ms_sstatus(uint64 mask) {
    __asm__ volatile("csrs sstatus, %0" : : "r"(mask));
}
inline void Riscv::mc_sstatus(uint64 mask) {
    __asm__ volatile("csrc sstatus, %0" : : "r"(mask));
}

inline void Riscv::ms_sip(uint64 mask) {
    __asm__ volatile("csrs sip, %0" : : "r"(mask));
}
inline void Riscv::mc_sip(uint64 mask) {
    __asm__ volatile("csrc sip, %0" : : "r"(mask));
}

inline time_t Riscv::getCurrentTime() {
    return systemTimer;
}

inline void Riscv::stopEmulator() {
    __asm__ volatile("li t0, 0x5555");
    __asm__ volatile("li t1, 0x100000");
    __asm__ volatile("sw t0, 0(t1)");
}

#endif