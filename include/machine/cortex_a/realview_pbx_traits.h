// EPOS RealView (ARM Cortex-A9) MCU Metainfo and Configuration

#ifndef __machine_traits_h
#define __machine_traits_h

#include <system/config.h>

__BEGIN_SYS

class Machine_Common;
template<> struct Traits<Machine_Common>: public Traits<void>
{
    static const bool debugged = Traits<void>::debugged;
};

template<> struct Traits<Machine>: public Traits<Machine_Common>
{
    static const unsigned int CPUS = Traits<Build>::CPUS;

    // Physical Memory  
    static const unsigned int MEM_BASE    = 0x00000000; // DDR SDRAM
    static const unsigned int MEM_TOP     = 0x0FFFFFFF;
    static const unsigned int BOOT_STACK  = 0x0FFFFFFC; // MEM_TOP - sizeof(int)

    // Logical Memory Map
    static const unsigned int APP_LOW     = MEM_BASE;
    static const unsigned int APP_CODE    = 0x00010000; // Address chosen by Qemu (http://www.informit.com/articles/article.aspx?p=2431417&seqNum=4)
    static const unsigned int APP_DATA    = MEM_BASE;
    static const unsigned int APP_HIGH    = MEM_TOP;

    static const unsigned int PHY_MEM     = MEM_BASE;
    static const unsigned int IO_BASE     = 0x10000000; // Is 'FPGA Peripherals' here?
    static const unsigned int IO_TOP      = 0xBFFFFFFF;

    static const unsigned int SYS         = 0x00000000; // What goes here??
    static const unsigned int SYS_CODE    = 0x00000000; // Library mode only => APP + SYS
    static const unsigned int SYS_DATA    = MEM_BASE; // Library mode only => APP + SYS

    // Default Sizes and Quantities
    static const unsigned int STACK_SIZE  = 512;
    static const unsigned int HEAP_SIZE   = 512;
    static const unsigned int MAX_THREADS = 5;
};

template<> struct Traits<IC>: public Traits<Machine_Common>
{
    static const bool debugged = hysterically_debugged;
};

template<> struct Traits<Timer>: public Traits<Machine_Common>
{
    static const bool debugged = hysterically_debugged;

    // Meaningful values for the timer frequency range from 100 to
    // 10000 Hz. The choice must respect the scheduler time-slice, i. e.,
    // it must be higher than the scheduler invocation frequency.
    static const int FREQUENCY = 1000; // Hz
};

template<> struct Traits<UART>: public Traits<Machine_Common>
{
    static const unsigned int UNITS = 2;

    static const unsigned int CLOCK = Traits<CPU>::CLOCK;

    static const unsigned int DEF_UNIT = 1;
    static const unsigned int DEF_BAUD_RATE = 115200;
    static const unsigned int DEF_DATA_BITS = 8;
    static const unsigned int DEF_PARITY = 0; // none
    static const unsigned int DEF_STOP_BITS = 1;
};

template<> struct Traits<USB>: public Traits<Machine_Common>
{
    static const bool enabled = false;
    static const unsigned int UNITS = 0;
    static const bool blocking = true;
};

template<> struct Traits<Scratchpad>: public Traits<Machine_Common>
{
    static const bool enabled = false;
};

__END_SYS

#endif