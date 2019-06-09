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
    typedef CPU::Reg32 Count;

public:
    enum TimerXBase {
        Timer1Base      = 0x00,
        Timer2Base      = 0x20,
    };
    // Timer 1'n'2 registers
    enum Timer_Register_offsets {       // Description                  Type    Value after reset
        Load            = 0x00,         // Load (Interval)              r/w     0x00000000
        Value           = 0x04,         // Value (Counter)              ro      0xFFFFFFFF
        Control         = 0x08,         // Control                      r/w     0x20
        IntClr          = 0x0C,         // Interrupt Clear              wo      -
        RIS             = 0x10,         // Raw Interrupt Status         ro      0x0
        MIS             = 0x14,         // Masked Interrupt Status      ro      0x0
        BGLoad          = 0x18,         // Background Load              r/w     0x00000000
    };
    // Dual-Timer Module registers
    enum Module_Register_offsets {      // Description                  Type    Value after reset
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
    enum Control_bits {                 // Function                     Type    Default value
        TimerEn         = 1 <<  7,      // Enable bit                   r/w     0
        TimerMode       = 1 <<  6,      // Mode bit                     r/w     0   (0: free-running; 1: periodic)
        IntEnable       = 1 <<  5,      // Interrupt Enable             r/w     1
        TimerPre        = 3 <<  2,      // Prescale bits                r/w     0
        TimerSize       = 1 <<  1,      // Bit counter operation        r/w     0   (0: 16-bit; 1: 32-bit)
        OneShot         = 1 <<  0,      // Counter mode                 r/w     0   (0: wrapping; 1: one-shot)
    };
    enum CounterMode { ModeOS, ModeFR, ModePe };
    enum TimerPre_bits {
        Prescale0   = 0 <<  2,  // 0 stages of prescale, clock is divided by 1 (default)
        Prescale4   = 1 <<  2,  // 4 stages of prescale, clock is divided by 16
        Prescale8   = 2 <<  2,  // 8 stages of prescale, clock is divided by 256
    };

protected:
    SP804(Log_Addr *        base,
        const TimerXBase    txb,
        const Count         count,
        const CounterMode   mode = ModePe,
        const bool          interrupt = true,
        const TimerPre_bits prescale = Prescale0,
        const bool          size32bits = true)
    :   _base(base), _txb(txb) {
        config(count, mode, interrupt, prescale, size32bits);
    }

public:
    ~SP804() { reset(); }

protected:
    void config(
        const Count         count,
        const CounterMode   mode = ModeFR,
        const bool          interrupt = true,
        const TimerPre_bits prescale = Prescale0,
        const bool          size32bits = false)
    {
        reset();
        int_clear();
        switch (mode) {
        case ModeOS:
            reg8(Control) |= OneShot;
            reg(Load) = count;
            break;
        case ModePe:
            reg8(Control) |= TimerMode;
            reg(Load) = count;
            break;
        case ModeFR: //default on reset
        default: break;
        }
        if(!interrupt)
            int_disable();
        if(size32bits)
            reg8(Control) |= TimerSize;
        reg8(Control) |= prescale;
        enable();
    }

    Count read()        { return reg(Value); }
    void enable()       { reg8(Control) |=  TimerEn; }
    void disable()      { reg8(Control) &= ~TimerEn; }
    void int_clear()    { reg(IntClr)    = -1; }
    void int_enable()   { reg8(Control) |=  IntEnable; }
    void int_disable()  { reg8(Control) &= ~IntEnable; }
    void reset() {
        int_clear();
        reg8(Control) = 0x20;
        reg(Load) = reg(BGLoad) = 0;
    }

private:
    volatile Reg8 & reg8(Timer_Register_offsets o) {
        return reinterpret_cast<volatile Reg8*>(_base)[(_txb + o) / sizeof(Reg8)];
    }
    volatile Reg32 & reg(Timer_Register_offsets o) {
        return reinterpret_cast<volatile Reg32*>(_base)[(_txb + o) / sizeof(Reg32)];
    }
    volatile Reg32 & reg(Module_Register_offsets o) {
        return reinterpret_cast<volatile Reg32*>(_base)[o / sizeof(Reg32)];
    }

private:
    volatile Log_Addr * _base;
    const TimerXBase _txb;
};

__END_SYS

#endif
