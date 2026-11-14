#ifndef OS1_MAIN_HPP
#define OS1_MAIN_HPP
#include "TCB.hpp"

class Main{
public:
    static TCB* kernel;
    static TCB* console;
    static TCB* user;
};

#endif