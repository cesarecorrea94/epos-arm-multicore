// EPOS REALVIEW_PBX (ARM Cortex) MCU Initialization

#include <machine/main.h>

#ifdef __mmod_realview_pbx__

__BEGIN_SYS

void REALVIEW_PBX::pre_init()
{
    db<Init, Machine>(TRC) << "REALVIEW_PBX::pre_init" << endl;
    //Pré inicializações do realview aqui se necessário!!!
    db<Init, Machine>(TRC) << "REALVIEW_PBX::pre_init" << endl;
}

void REALVIEW_PBX::init()
{
    db<Init, Machine>(TRC) << "REALVIEW_PBX::init" << endl;
    //Inicializações do realview aqui se necessário!!!
    db<Init, Machine>(TRC) << "REALVIEW_PBX::init" << endl;
}

__END_SYS

#endif
