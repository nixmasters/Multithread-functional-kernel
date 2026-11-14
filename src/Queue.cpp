#include "../h/Queue.hpp"

Queue::Queue() {
    head = nullptr;
    tail = nullptr;
}

bool Queue::isEmpty(){
    return head == tail;
}

TCB* Queue::peek(){
    return head;
}

void Queue::insert(TCB *tcb) {
    tcb->next = nullptr;
    if (!head) head = tcb;
    else tail->next = tcb;
    tail = tcb;
}

TCB *Queue::remove() {
    if (!head) return nullptr;
    TCB* ret = head;

    head = head->next;
    if (!head) tail = nullptr;
    ret->next = nullptr;
    return ret;
}

void Queue::insertSleep(TCB *tcb) {
    TCB* prev = nullptr;
    TCB* curr = head;
    while (curr && tcb->getSleepTime() > curr->getSleepTime()) {
        prev = curr;
        curr = curr->next;
    }
    tcb->next = curr;
    if(!tcb->next) tail = tcb;
    if (!prev) head = tcb;
    else prev->next = tcb;
}


