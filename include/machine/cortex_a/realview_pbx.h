#ifndef __realview_pbx_h
#define __realview_pbx_h

#include <architecture/cpu.h>
#include <architecture/tsc.h>
#include <machine/rtc.h>
#include <system.h>

__BEGIN_SYS

class Realview_PBX
{
    friend class TSC;

protected:
    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;

public:

protected:
    Realview_PBX() {}

    static void reboot() {
        // Reg32 val = scs(AIRCR) & (~((-1u / VECTKEY) * VECTKEY));
        // val |= 0x05fa * VECTKEY | SYSRESREQ;
        // scs(AIRCR) = val;
    }

    static void delay(const RTC::Microsecond & time) {
        // assert(Traits<TSC>::enabled);
        // unsigned long long ts = static_cast<unsigned long long>(time) * TSC::frequency() / 1000000;
        // tsc(GPTMTAILR) = ts;
        // tsc(GPTMTAPR) = ts >> 32;
        // tsc(GPTMCTL) |= TAEN;
        // while(!(tsc(GPTMRIS) & TATO_INT));
        // tsc(GPTMCTL) &= ~TAEN;
        // tsc(GPTMICR) |= TATO_INT;
    }

    // Device enabling
    static void enable_uart(unsigned int unit) {
        // assert(unit < UARTS);
        // power_uart(unit, FULL);
        // gpioa(AFSEL) |= 3 << (unit * 2);                // Pins A[1:0] are multiplexed between GPIO and UART 0. Select UART.
        // gpioa(DEN) |= 3 << (unit * 2);                  // Enable digital I/O on Pins A[1:0]
    }

    // Power Management
    static void power_uart(unsigned int unit, const Power_Mode & mode) {
        // assert(unit < UARTS);
        // switch(mode) {
        // case ENROLL:
        // 	break;
        // case DISMISS:
        // 	break;
        // case SAME:
        // 	break;
        // case FULL:
        // 	break;
        // case LIGHT:
        // 	break;
        // case SLEEP:
        //     scr(RCGC1) |= 1 << unit;                   // Activate UART "unit" clock
        //     scr(RCGC2) |= 1 << unit;                   // Activate port "unit" clock
        //     break;
        // case OFF:
        //     scr(RCGC1) &= ~(1 << unit);                // Deactivate UART "unit" clock
        //     scr(RCGC2) &= ~(1 << unit);                // Deactivate port "unit" clock
        //     break;
        // }
    }

    static void power_user_timer(unsigned int unit, const Power_Mode & mode) {
        // assert(unit < TIMERS);
        // switch(mode) {
        // case ENROLL:
        // 	break;
        // case DISMISS:
        // 	break;
        // case SAME:
        // 	break;
        // case FULL:
        // 	break;
        // case LIGHT:
        // 	break;
        // case SLEEP:
        //     scr(RCGC1) |= 1 << (unit + 16);             // Activate GPTM "unit" clock
        //     break;
        // case OFF:
        //     scr(RCGC1) &= ~(1 << (unit + 16));          // Deactivate GPTM "unit" clock
        //     break;
        // }
    }

public:
    // static volatile Reg32 & scr(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(SCR_BASE)[o / sizeof(Reg32)]; }
    // static volatile Reg32 & scs(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(IC0_BASE)[o / sizeof(Reg32)]; }

    // static volatile Reg32 & systick(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(TIMER4_BASE)[o / sizeof(Reg32)]; }
    // static volatile Reg32 & tsc(unsigned int o)     { return reinterpret_cast<volatile Reg32 *>(TIMER1_BASE)[o / sizeof(Reg32)]; }
    // static volatile Reg32 & timer0(unsigned int o)  { return reinterpret_cast<volatile Reg32 *>(TIMER0_BASE)[o / sizeof(Reg32)]; }
    // static volatile Reg32 & timer1(unsigned int o)  { return reinterpret_cast<volatile Reg32 *>(TIMER2_BASE)[o / sizeof(Reg32)]; }
    
protected:
    static void pre_init();
    static void init();
};

typedef Realview_PBX Machine_Model;

__END_SYS

#endif