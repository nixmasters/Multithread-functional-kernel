#include "../h/Riscv.hpp"
#include "../h/BoundedBuffer.hpp"
#include "../h/Console.hpp"
#include "../h/Riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/Scheduler.hpp"
#include "../h/Main.hpp"

BoundedBuffer* Console::inputBuffer = nullptr;
BoundedBuffer* Console::outputBuffer = nullptr;

void Console::consoleWrapper(void *_) {
    putCharacters();
}
void Console::getCharacters() {
    if (r_console_status() & CONSOLE_RX_STATUS_BIT) {
        char c = (char) r_console_data();
        inputBuffer->put(c);
    }
}

void Console::putCharacters() {
    while(!Main::user->isFinished() || !outputBuffer->isEmpty()){
        if((r_console_status() & CONSOLE_TX_STATUS_BIT) && !outputBuffer->isEmpty()){
            char c = outputBuffer->get();
            w_console_data(c);
        }else thread_dispatch();
    }
    Scheduler::put(Main::kernel);
    thread_exit();
}

void Console::_getc() {
    char c = inputBuffer->get();
    /*if (c != 0x1b && c != 0xd) // ESC = 0x1b, CR = 0xd, LF = 0xa
        outputBuffer->put(c);
    if (c == 0xd)
        outputBuffer->put(0xa); */
    Riscv::w_a0_stack((uint64) c);
}

void Console::_putc() {
    char c = (char) Riscv::r_a_index_stack(1);
    outputBuffer->put(c);
}

inline uint64 Console::r_console_status() {
    uint64 addr = CONSOLE_STATUS;
    __asm__ volatile("mv a0, %0"::"r"(addr));
    __asm__ volatile("lb a1, 0(a0)");
    uint64 status;
    __asm__ volatile("mv %0, a1" :  "=r"(status));
    return status;
}
inline uint64 Console::r_console_data() {
    uint64 addr = CONSOLE_TX_DATA;
    __asm__ volatile("mv a0, %0"::"r"(addr));
    __asm__ volatile("lb a1,0(a0)");
    char c;
    __asm__ volatile("mv %0, a1" :  "=r"(c));
    return c;
}
inline void Console::w_console_data(uint64 c) {
    uint64 addr = CONSOLE_RX_DATA;
    __asm__ volatile("mv a0, %0"::"r"(addr));
    __asm__ volatile("sb %0, 0(a0)" :  :"r"(c));
}

void Console::initializeConsole() {
    inputBuffer = new BoundedBuffer();
    outputBuffer = new BoundedBuffer();
}
