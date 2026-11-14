
#include "../h/syscall_c.hpp"
#include "Threads_C_API_test.hpp"

#include "printing.hpp"

static volatile bool finishedA = false;
static volatile bool finishedB = false;
static volatile bool finishedC = false;
static volatile bool finishedD = false;

static uint64 fibonacci(uint64 n) {
    if (n == 0 || n == 1) { return n; }
    if (n % 10 == 0) { thread_dispatch(); }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

static void workerBodyA(void* arg) {
    int id ;
    for (int i=0;i<10;i++) {
        id = get_thread_id();
        printString("A ID =");
        printInt(id);
        printString("\n");
        time_sleep(5);
    }
    printString("A finished!\n");
    finishedA = true;
    thread_dispatch();
}

static void workerBodyB(void* arg) {
    int id ;
    for (int i=0;i<10;i++) {
        id = get_thread_id();
        printString("B ID =");
        printInt(id);
        printString("\n");
        time_sleep(5);
    }
    printString("B finished!\n");
    finishedB = true;
    thread_dispatch();
}

static void workerBodyC(void* arg) {
    int id ;
    for (int i=0;i<10;i++) {
        id = get_thread_id();
        printString("C ID =");
        printInt(id);
        printString("\n");
        time_sleep(5);
    }
    printString("C finished!\n");
    finishedC = true;
    thread_dispatch();
}



void GetThreadIDTest() {
    thread_t threads[4];
    thread_create(&threads[0], workerBodyA, nullptr);
    printString("ThreadA created\n");

    thread_create(&threads[1], workerBodyB, nullptr);
    printString("ThreadB created\n");

    thread_create(&threads[2], workerBodyC, nullptr);
    printString("ThreadC created\n");



    while (!(finishedA && finishedB && finishedC)) {
        thread_dispatch();
    }

}
