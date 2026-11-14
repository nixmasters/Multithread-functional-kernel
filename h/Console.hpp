#ifndef OS1_CONSOLE_HPP
#define OS1_CONSOLE_HPP
#include "BoundedBuffer.hpp"

class Console {
public:
    static void consoleWrapper(void* _);

    static void getCharacters();
    static void putCharacters();

    static void _putc();
    static void _getc();

    static uint64 r_console_status();
    static uint64 r_console_data();
    static void w_console_data(uint64 c);

    static void initializeConsole();

private:
    static BoundedBuffer* inputBuffer;
    static BoundedBuffer* outputBuffer;

};

#endif