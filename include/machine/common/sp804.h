// EPOS SP804 Dual-Timer Module Mediator Declarations

#ifndef __sp804_h__
#define __sp804_h__

#include <system/config.h>

__BEGIN_SYS

// ARM Dual-Timer Module (SP804)
class SP804: protected Machine_Model
{
private:
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg32 Reg32;
/* 
    static const unsigned int UNITS = Traits<UART>::UNITS;
    static const unsigned int CLOCK = Traits<UART>::CLOCK / 16;
 */
public:
    enum _TimerXBase {
        None            = 0,
        Timer1Base      = 0x00,
        Timer2Base      = 0x20,
    };
    enum _Register_offsets {             // Description                  Type    Value after reset
        // Timer 1'n'2 registers
        Load            = 0x00,         // Load                         r/w     0x00000000
        Value           = 0x04,         // Value                        ro      0xFFFFFFFF
        Control         = 0x08,         // Control                      r/w     0x20
        IntClr          = 0x0C,         // Interrupt Clear              wo      -
        RIS             = 0x10,         // Raw Interrupt Status         ro      0x0
        MIS             = 0x14,         // Masked Interrupt Status      ro      0x0
        BGLoad          = 0x18,         // Background Load              r/w     0x00000000
        // Dual-Timer Module registers
        TimerITCR       = 0xF00,        // Integration Test Control     r/w     0x0
        TimerITOP       = 0xF04,        // Integration Test Output Set  wo      0x0
        PeriphID0       = 0xFE0,        // Peripheral Identification 0  ro      0x04
        PeriphID1       = 0xFE4,        // Peripheral Identification 1  ro      0x18
        PeriphID2       = 0xFE8,        // Peripheral Identification 2  ro      0x14
        PeriphID3       = 0xFEC,        // Peripheral Identification 3  ro      0x00
        PCellID0        = 0xFF0,        // PrimeCell Identification 0   ro      0x0D
        PCellID1        = 0xFF4,        // PrimeCell Identification 1   ro      0xF0
        PCellID2        = 0xFF8,        // PrimeCell Identification 2   ro      0x05
        PCellID3        = 0xFFC         // PrimeCell Identification 3   ro      0xB1
    };

    // Useful Bits in the Control Register
    enum _Control {                     // Function                     Type    Default value
        TimerEn         = 1 <<  7,      // Enable bit                   r/w     0
        TimerMode       = 1 <<  6,      // Mode bit                     r/w     0   (0: free-running; 1: periodic)
        IntEnable       = 1 <<  5,      // Interrupt Enable             r/w     1
        TimerPre        = 3 <<  2,      // Prescale bits                r/w     0
        TimerPre0       = 0 <<  2,      // 0 stages of prescale         --      -   Clock is divided by 1 (default)
        TimerPre4       = 1 <<  2,      // 4 stages of prescale         --      -   Clock is divided by 16
        TimerPre8       = 2 <<  2,      // 8 stages of prescale         --      -   Clock is divided by 256
        TimerSize       = 1 <<  1,      // Bit counter operation        r/w     0   (0: 16-bit; 1: 32-bit)
        OneShot         = 1 <<  0,      // Counter mode                 r/w     0   (0: wrapping; 1: one-shot)
    };

public:
/*
    PL011(unsigned int unit, unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits)
    : _base(reinterpret_cast<Log_Addr *>(unit ? UART1_BASE : UART0_BASE)) {
        assert(unit < UNITS);
        config(baud_rate, data_bits, parity, stop_bits);
    }

    void config(unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits) {
        Reg32 lcrh = data_bits == 8 ? WLEN8 : data_bits == 7 ? WLEN7 : data_bits = 6 ? WLEN6 : WLEN5; // config data bits
        lcrh |= FEN; // always use FIFO
        lcrh |= stop_bits == 2 ? STP2 : 0; // config stop bits
        lcrh |= (parity == 2) ? (EPS | PEN) : (parity == 1) ? PEN : 0; // config and enable even/odd parity

        reg(UCR) &= ~UEN;                       // Disable UART for configuration
        reg(ICR) = ~0;                          // Clear all interrupts
        reg(UIM) = UIMALL;                      // Disable all interrupts
        Reg32 br = CLOCK / (baud_rate / 300);   // Factor by the minimum BR to preserve meaningful bits of FBRD
        reg(IBRD) = br / 300;                   // IBRD = int(CLOCK / baud_rate)
        reg(FBRD) = br / 1000;                  // FBRD = int(0.1267 * 64 + 0.5) = 8
        reg(LCRH) = lcrh;                       // Write the serial parameters configuration
        reg(UIM) = UIMTX | UIMRX;               // Mask TX and RX interrupts for polling operation
        reg(UCR) |= UEN | TXE | RXE;            // Enable UART
    }

    void config(unsigned int * baud_rate, unsigned int * data_bits, unsigned int * parity, unsigned int * stop_bits) {
        Reg32 lcrh = reg(LCRH);
        *data_bits = 5 + (lcrh & WLEN8);
        *parity = (lcrh & PEN) ? (1 + (lcrh & EPS)) : 0;
        *baud_rate = (CLOCK * 300) / (reg(FBRD) * 1000 + reg(IBRD) * 300);
        *stop_bits = (reg(LCRH) & STP2) ? 2 : 1;
    }

    Reg8 rxd() { return reg(DR); }
    void txd(Reg8 c) { reg(DR) = c; }

    void int_enable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        reg(UIM) &= ~((receive ? UIMRX : 0) | (send ? UIMTX : 0));
    }
    void int_disable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        reg(UCR) |= (receive ? UIMRX : 0) | (send ? UIMTX : 0);
    }

    void reset() {
        unsigned int b, db, p, sb;
        config(&b, &db, &p, &sb);
        config(b, db, p, sb);
    }

    void loopback(bool flag) {
        if(flag)
            reg(UCR) |= int(LBE);
        else
            reg(UCR) &= ~LBE;
    }

    bool rxd_ok() { return !(reg(FR) & RXFE); }
    bool txd_ok() { return !(reg(FR) & TXFF); }

    bool rxd_full() { return (reg(FR) & RXFF); }
    bool txd_empty() { return (reg(FR) & TXFE) && !(reg(FR) & BUSY); }

    bool busy() { return (reg(FR) & BUSY); }
 */
private:
    volatile Reg32 & reg(_Register_offsets o, _TimerXBase txb = None) { return reinterpret_cast<volatile Reg32*>(_base)[(txb+o) / sizeof(Reg32)]; }

private:
    volatile Log_Addr * _base;
};

__END_SYS

#endif
