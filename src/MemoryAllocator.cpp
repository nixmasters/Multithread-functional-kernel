#include "../lib/hw.h"
#include "../h/MemoryAllocator.hpp"
#include "../h/Riscv.hpp"

MemoryBlock* MemoryAllocator::freeMemHead = nullptr;
MemoryBlock* MemoryAllocator::usedMemHead = nullptr;
size_t MemoryAllocator::sizeFree = 0;

void MemoryAllocator::initializeMemory() {
    /*
    MemoryAllocator::freeMemHead = (MemoryBlock*)HEAP_START_ADDR;
    freeMemHead = (MemoryBlock*)(((uint64)freeMemHead >> 4) << 4); // align 16
    freeMemHead->next = nullptr;
    freeMemHead->prev  = nullptr;
    freeMemHead->size = (char*)freeMemHead - (char*)HEAP_END_ADDR - sizeof(MemoryBlock);
    */
    MemoryAllocator::freeMemHead = (MemoryBlock*)HEAP_START_ADDR;
    freeMemHead->next = nullptr;
    freeMemHead->prev  = nullptr;
    freeMemHead->size = (char*)HEAP_START_ADDR - (char*)HEAP_END_ADDR - sizeof(MemoryBlock);
}

void *MemoryAllocator::mem_alloc(size_t size) {
    size = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    size *= MEM_BLOCK_SIZE;
    MemoryBlock* curr = freeMemHead;
    while(curr && curr->size < size) curr = curr->next;
    if(!curr) return nullptr;
    if(curr->size > size) {
        MemoryBlock* newBlock = (MemoryBlock*) ((char*)curr + size + sizeof(MemoryBlock));
        if(curr->prev) curr->prev->next = newBlock;
        else MemoryAllocator::freeMemHead = newBlock;
        if(curr->next) curr->next->prev = newBlock;
        newBlock->prev = curr->prev;
        newBlock->next = curr->next;
        newBlock->size = curr->size - size - sizeof(MemoryBlock);

        curr->next = usedMemHead;
        if(usedMemHead) usedMemHead->prev = curr;
        curr->prev = nullptr;
        curr->size = size;
        usedMemHead = curr;

    }
    else {
        if (curr->prev) curr->prev->next = curr->next;
        else freeMemHead = curr->next;
        if(curr->next) curr->next->prev = curr->prev;

        curr->next = usedMemHead;
        if(usedMemHead) usedMemHead->prev = curr;
        curr->prev = nullptr;
        usedMemHead = curr;

    }
    sizeFree -= size;
    return (char*)curr + sizeof(MemoryBlock);
}

int MemoryAllocator::mem_free(void* ptr) {
    if(usedMemHead == nullptr) return 0;
    if(ptr == nullptr || ptr < HEAP_START_ADDR || ptr > HEAP_END_ADDR) return -1;
    char* addr = (char*)ptr - sizeof(MemoryBlock);
    MemoryBlock* curr = nullptr;
    MemoryBlock* old = (MemoryBlock*) addr;

    if(old->prev) old->prev->next = old->next;
    else usedMemHead = old->next;
    if(old->next) old->next->prev = old->prev;

    size_t size = old->size;

    if(!freeMemHead || addr < (char*)freeMemHead) curr = nullptr;
    else{
        for(curr = freeMemHead; curr->next && addr>(char*)(curr->next); curr = curr->next);
    }
    if(curr && (char*)curr + curr->size + sizeof(MemoryBlock) == addr){
        curr->size += size + sizeof(MemoryBlock);
        if(curr->next && (char*)curr + curr->size + sizeof(MemoryBlock) == (char*)(curr->next)){
            curr->size += curr->next->size;
            curr->next = curr->next->next;
            if(curr->next) curr->next->prev = curr;
        }
    }else{
        MemoryBlock* nextSegment = curr ? curr->next : freeMemHead;
        if(nextSegment && addr + size + sizeof(MemoryBlock) == (char*)nextSegment){
            MemoryBlock* newSegment = (MemoryBlock*) addr;
            newSegment->size = nextSegment->size + size + sizeof(MemoryBlock);
            newSegment->prev = nextSegment->prev;
            newSegment->next = nextSegment->next;
            if(nextSegment->next) nextSegment->next->prev = newSegment;
            if(nextSegment->prev) nextSegment->prev->next = newSegment;
            else freeMemHead = newSegment;
        }else{
            MemoryBlock* newSegment = (MemoryBlock*) addr;
            newSegment->size = size;
            newSegment->prev = curr;
            if(curr) newSegment->next = curr->next;
            else newSegment->next = freeMemHead;
            if(newSegment->next) newSegment->next->prev = newSegment;
            if(curr) curr->next = newSegment;
            else freeMemHead = newSegment;
        }
    }
    sizeFree += size;
    return 0;
}

void MemoryAllocator::_mem_get_free_space(){
    Riscv::w_a0_stack((uint64)sizeFree);
}

void MemoryAllocator::_mem_get_largest_free_block(){
    if(!freeMemHead){
        Riscv::w_a0_stack(0);
        return;
    }
    size_t maxSize = freeMemHead->size;
    MemoryBlock* curr = freeMemHead;
    while(curr){
        if(maxSize < curr->size){
            maxSize = curr->size;
        }
        curr = curr->next;
    }

    Riscv::w_a0_stack(maxSize);
}

void MemoryAllocator::_mem_alloc() {
    size_t size = (size_t) Riscv::r_a_index_stack(1);
    void* addr = mem_alloc(size * MEM_BLOCK_SIZE);
    Riscv::w_a0_stack((uint64) addr);
}

void MemoryAllocator::_mem_free() {
    void* addr = (void*) Riscv::r_a_index_stack(1);
    uint64 ret = mem_free(addr);
    Riscv::w_a0_stack(ret);
}
