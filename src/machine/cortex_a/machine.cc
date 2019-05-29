// EPOS Cortex-A Mediator Implementation

#include <machine/cortex_a/machine.h>
#include <machine/display.h>

__BEGIN_SYS

void Machine::panic()
{
    CPU::int_disable();
    if(Traits<Display>::enabled)
        Display::puts("PANIC!\n");
    if(Traits<System>::reboot)
        reboot();
    else
        CPU::halt();
}

void Machine::reboot()
{
    db<Machine>(WRN) << "Machine::reboot()" << endl;
    Machine_Model::reboot();
}

unsigned int Machine::n_cpus()
{
    db<Machine>(TRC) << "Machine::n_cpus()" << endl;
    return Traits<Build>::CPUS;
}

unsigned int Machine::cpu_id()
{
    db<Machine>(TRC) << "Machine::cpu_id()" << endl;
    unsigned int ret = 0;
    ASM("MRC p15, 0, r1, c0, c0, 5  \n"
        "AND r1, r1, #0x3           \n"
        "//MOV =ret, r1               \n"); // Is it right?
    return ret; // How to return the cpu_id?
}

__END_SYS
