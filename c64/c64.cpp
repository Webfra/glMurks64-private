#include "c64.h"

#include "hex.h"
//========================================================================
#include <iostream>
#include <sstream>

//========================================================================
void C64::init( uint8_t * basic, uint8_t *kernal, uint8_t *chargen)
{
    memset(&RAM[0], 0x00, 0x10000);
    memset(&ROM[0], 0x00, 0x10000);

    memcpy( &ROM[0xA000], basic, 0x2000 );
    memcpy( &ROM[0xE000], kernal, 0x2000 );
    memcpy( &ROM[0xD000], chargen, 0x1000 );
}

//========================================================================
void C64::loop()
{
    if(reset)
    {
        clockticks = 0;
        memset(&RAM[0], 0x00, 0x10000);
        reset6502(this);
        reset = false;
    }
    if( enable_debug_logs )
        std::cout << "X " << bin2hex4(pc) << std::endl;
    step(this);
    // Simulate VIC-II rasterline register
    if( 0==(clockticks & 63) )
    {
        RAM[0xD012]++;
    }
}

//==================================================================
// Run the C64 - executed in separate thread!
void C64::run()
{
    //------------------------------------------------------------
    auto irq_clock = std::chrono::high_resolution_clock::now();
    //------------------------------------------------------------
    while(!quit)
    {
        if(!paused)
        {
            //------------------------------------------------------------
            loop();
            //------------------------------------------------------------
            // Generate timer interrupt roughly 60 times per second.
            auto now = std::chrono::high_resolution_clock::now();
            auto delta = (now - irq_clock);
            auto micro = std::chrono::duration_cast<std::chrono::microseconds>(delta);
            if( micro.count() >= 16666 ) // 16.666ms = 60 fps
            {
                irq_clock = now;
                if( ( RAM[0xDC0E] & 1) == 1 )
                {
                    irq6502(this);
                }
            }
            //------------------------------------------------------------
        }
        else // prevent Timer IRQs from being generated when paused.
        {
            irq_clock = std::chrono::high_resolution_clock::now();
        }
    }
}

//==================================================================
void C64::stop()
{
    quit=true;
    runner.join();
}

//==================================================================
void C64::set_key_state(const Key & key, bool state) noexcept
{
    //------------------------------------------------------------------
    uint8_t & row = key_matrix[ key.row ];
    //------------------------------------------------------------------
    uint8_t mask = uint8_t(1 << key.col);
    //------------------------------------------------------------------
    if( state ) row &= ~mask;
    else        row |= mask;
    //------------------------------------------------------------------
}
//==================================================================
bool C64::get_key_state(const Key & key) const noexcept
{
    //------------------------------------------------------------------
    const uint8_t & row = key_matrix[ key.row ];
    //------------------------------------------------------------------
    uint8_t mask = uint8_t(1 << key.col);
    //------------------------------------------------------------------
    // Check if Esc is currently pressed.
    return (row & mask) == 0;
}

//==================================================================
uint8_t handle_key_matrix( C64 *p64 ) noexcept
{
    //------------------------------------------------------------------
    // Keys are active low in they matrix, start with all bits set.
    uint8_t result = 0xFF;
    //------------------------------------------------------------------
    // Go through all rows.
    for(int i=0; i<8; i++)
    {
        //------------------------------------------------------------------
        // If a row is selected by a cleared bit in $DC00, then
        if( 0 == (p64->RAM[0xDC00] & (1<<i)) )
        {
            //------------------------------------------------------------------
            // "OR" ("NAND") it to the result bits.
            result &= p64->key_matrix[i];
        }
    }
    //------------------------------------------------------------------
    // Write result to RAM. This is just for debugging.
    // (Any read of $DC01 would go trough this function again.)
    // But this way, $DC01 content can be seen in memory.
    p64->RAM[0xDC01] = result;
    //------------------------------------------------------------------
    // Overwrite the default result and return the currently active keyboard bits.
    return result;
    //------------------------------------------------------------------
}

//========================================================================
void mem_write(context_t *cpu, uint16_t address, uint8_t value)
{
    C64 *p64 = (C64*)cpu;
    if( p64->enable_debug_logs )
        std::cout << "W " << bin2hex4(address) << ": " << bin2hex2(value) << std::endl;
    p64->RAM[address] = value;
}

//========================================================================
uint8_t mem_read(context_t *cpu, uint16_t address)
{
    C64 *p64 = (C64*)cpu;

    uint8_t value;

    // Read from CIA1 port B: Read keyboard matrix.
    if( address == 0xDC01 )
    {
        return handle_key_matrix(p64);
    }
    else if( (address>=0xA000) && (address<=0xBFFF) )
    {
        value = p64->ROM[address];
    }
    /* Leave out char rom, as it usually is hidden behind I/O area. */
    else if( (address>=0xE000) && (address<=0xFFFF) )
    {
        value = p64->ROM[address];
    }
    else
    {
        value = p64->RAM[address];
    }
    if( p64->enable_debug_logs )
        std::cout << "R " << bin2hex4(address) << ": " << bin2hex2(value) << "  - " << bin2hex2(p64->ROM[0xEE8E]) << std::endl;
    return value;
}

//========================================================================
