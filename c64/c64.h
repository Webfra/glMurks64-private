#ifndef C64_H
#define C64_H

//========================================================================
#include <string>
#include <vector>
#include <cstring>

//========================================================================
#include <stdint.h>
extern "C" {
    #include "fake6502.h"
    void reset6502(context_t * c);
    void irq6502  (context_t * c);
    void nmi6502  (context_t * c);
};
void mem_write(context_t *, uint16_t address, uint8_t value);
uint8_t mem_read(context_t *, uint16_t address);

//========================================================================
typedef struct _c64_t {

    context_t cpu;

    bool enable_debug_logs { false };

    uint8_t RAM[0x10000];
    uint8_t ROM[0x10000];

    bool quit = false;
    bool reset = true;
    int old_clock = 0;

    void init( uint8_t * basic, uint8_t *kernal, uint8_t *chargen);
    void loop();
} C64;

//========================================================================
//extern C64 c64;

//========================================================================
#endif // C64_H
