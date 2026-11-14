
#include "../h/syscall_c.hpp"
#include "Threads_C_API_test.hpp"

#include "printing.hpp"

static volatile bool finishedA = false;
static volatile bool finishedB = false;
static volatile bool finishedC = false;


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
    thread_t* threadA = (thread_t*) arg;
    thread_join(threadA);
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
    //thread_t* threadB = (thread_t*) arg;

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



void joinTest() {
    thread_t threads[4];
    thread_create(&threads[0], workerBodyA, nullptr);
    printString("ThreadA created\n");

    thread_create(&threads[1], workerBodyB, &threads[0]);
    printString("ThreadB created\n");

    thread_create(&threads[2], workerBodyC, &threads[1]);
    printString("ThreadC created\n");



    while (!(finishedA && finishedB && finishedC)) {
        thread_dispatch();
    }

}
