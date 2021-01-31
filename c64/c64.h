#ifndef C64_H
#define C64_H

//========================================================================
#include <string>
#include <vector>
#include <cstring>

#include <thread>
#include <iostream>
#include <chrono>

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
struct C64 : public context_t
{
    //===================================================================================
    C64() : runner(&run, this) {}
    ~C64() { stop(); }
    std::thread runner;
    void run(); // runner thread!
    void stop(); // stop and join the thread

    //===================================================================================
    uint8_t RAM[0x10000];
    uint8_t ROM[0x10000];

    //===================================================================================
    bool quit = false;
    bool paused = true; // start paused until memory is initialized.
    bool reset = true;
    bool enable_debug_logs { false };
    //===================================================================================
    void init( uint8_t * basic, uint8_t *kernal, uint8_t *chargen);
    void loop();

    //===================================================================================
    // The 8bit by 8bit key matrix holds the state of all keys
    // 1=not pressed, 0=pressed
    uint8_t key_matrix[8] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };
    struct Key { int row; int col; };
    void set_key_state(const Key & key, bool state) noexcept;
    bool get_key_state(const Key & key) const  noexcept;
};

//========================================================================
//extern C64 c64;

//========================================================================
#endif // C64_H
