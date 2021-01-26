#include "c64.h"

#include "hex.h"
//========================================================================
#include <iostream>
#include <sstream>

//========================================================================
//C64 c64;

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
    //cpu.clockticks = 0;
    //while(!quit)
    {
        if(reset)
        {
            cpu.clockticks = 0;
            memset(&RAM[0], 0x00, 0x10000);
            reset6502(&cpu);
            reset = false;
        }
        if( enable_debug_logs )
            std::cout << "X " << bin2hex4(cpu.pc) << std::endl;
        step(&cpu);
        // Simulate VIC-II rasterline register
        if( 0==(cpu.clockticks & 63) )
        {
           RAM[0xD012]++;
        }
    }
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
    if( (address>=0xA000) && (address<=0xBFFF) )
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
