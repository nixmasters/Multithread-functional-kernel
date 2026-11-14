#include "../h/Main.hpp"
#include "../h/Riscv.hpp"
#include "../h/Console.hpp"
#include "../h/Scheduler.hpp"

TCB* Main::kernel = nullptr;
TCB* Main::console = nullptr;
TCB* Main::user = nullptr;

extern void userMain();

void mainWrapper(void* _){
    userMain();
}

int main(){
    Riscv::w_stvec((uint64)Riscv::supervisorTrap);
    MemoryAllocator::initializeMemory();
    Console::initializeConsole();

    Main::kernel = new TCB(nullptr, nullptr, nullptr, 0);
    Main::kernel->setPrivilege(TCB::SUPERVISOR);
    Main::kernel->setRunnable(false);

    Main::console = new TCB(Console::consoleWrapper, nullptr);
    Main::console->setPrivilege(TCB::SUPERVISOR);
    Main::user = new TCB(mainWrapper, nullptr);

    TCB::running = Main::kernel;
    Scheduler::put(Main::console);
    Scheduler::put(Main::user);

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
    
    TCB::_thread_dispatch();

    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);
    Riscv::stopEmulator();
    return 0;
}