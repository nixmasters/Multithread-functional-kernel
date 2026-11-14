#include "../h/syscall_c.hpp"

void* mem_alloc(size_t size) {
    uint64 sz = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    __asm__ volatile("mv a1, %0" : : "r" (sz));
    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x01));

    __asm__ volatile("ecall");

    uint64 addr;
    __asm__ volatile("mv %[addr], a0" : [addr] "=r" (addr));
    return (void*)addr;
}

int mem_free(void* addr) {
    __asm__ volatile("mv a1, %0" : : "r" ((uint64)addr));
    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x02));

    __asm__ volatile("ecall");

    uint64 ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return (int)ret;
}

size_t mem_get_free_space(){
    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x03));

    __asm__ volatile("ecall");

    size_t ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return (size_t)ret;
}

size_t mem_get_largest_free_block(){
    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x04));

    __asm__ volatile("ecall");

    size_t ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return (size_t)ret;
}

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg) {
    void* stack = mem_alloc(DEFAULT_STACK_SIZE);
    if (!stack) return -1;

    __asm__ volatile("mv a4, %0" : : "r" ((uint64)stack));
    __asm__ volatile("mv a3, %0" : : "r" ((uint64)arg));
    __asm__ volatile("mv a2, %0" : : "r" ((uint64)start_routine));
    __asm__ volatile("mv a1, %0" : : "r" ((uint64)handle));
    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x11));

    __asm__ volatile("ecall");

    uint64 ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return (int)ret;
}
void thread_join(thread_t* handle) {
    __asm__ volatile("mv a1, %0" : : "r" ((uint64)handle));
    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x36));

    __asm__ volatile("ecall");

}


int thread_exit() {
    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x12));

    __asm__ volatile("ecall");

    uint64 ret;
    __asm__ volatile("mv %0, a0" : [ret] "=r" (ret));
    return (int)ret;
}

void thread_dispatch() {
    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x13));

    __asm__ volatile("ecall");
}

int get_thread_id() {

    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x35));

    __asm__ volatile("ecall");

    uint64 ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return (int)ret;
}

int time_sleep(time_t time) {
    __asm__ volatile("mv a1, %0" : : "r" ((uint64)time));
    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x31));

    __asm__ volatile("ecall");

    uint64 ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return (int)ret;
}

int sem_open(sem_t* handle, unsigned init) {
    __asm__ volatile("mv a2, %0" : : "r" ((uint64)init));
    __asm__ volatile("mv a1, %0" : : "r" ((uint64)handle));
    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x21));

    __asm__ volatile("ecall");

    uint64 ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return (int)ret;
}

int sem_close(sem_t handle) {
    __asm__ volatile("mv a1, %0" : : "r" ((uint64)handle));
    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x22));

    __asm__ volatile("ecall");

    uint64 ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return (int)ret;
}

int sem_wait(sem_t id) {
    __asm__ volatile("mv a1, %0" : : "r" ((uint64)id));
    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x23));

    __asm__ volatile("ecall");

    uint64 ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return (int)ret;
}

int sem_signal(sem_t id) {
    __asm__ volatile("mv a1, %0" : : "r" ((uint64)id));
    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x24));

    __asm__ volatile("ecall");

    uint64 ret;
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return (int)ret;
}

char getc() {
    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x41));

    __asm__ volatile("ecall");

    uint64 c;
    __asm__ volatile("mv %[c], a0" : [c] "=r" (c));
    return (char)c;
}

void putc(char c) {
    __asm__ volatile("mv a1, %0" : : "r" ((uint64)c));
    __asm__ volatile("mv a0, %[syscall]" : : [syscall] "r" (0x42));

    __asm__ volatile("ecall");
}
