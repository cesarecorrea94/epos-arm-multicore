// EPOS ARMv7 MMU Mediator Initialization

#include <architecture/mmu.h>

extern "C" void * __data_start;
extern "C" void * _edata;
extern "C" void * __bss_start;
extern "C" void * _end;
char ttb_address[16*1024] __attribute__((aligned (16*1024)));

__BEGIN_SYS

void MMU::init()
{
    db<Init, MMU>(TRC) << "MMU::init()" << endl;

    db<Init, MMU>(INF) << "MMU::init::dat.b=" << &__data_start << ",dat.e=" << &_edata << ",bss.b=" << &__bss_start << ",bss.e=" << &_end << endl;

    ASM(
    // Disable MMU
    "\n     MRC p15, 0, r1, c1, c0, 0"  // Atribui-se ao R1 o valor do registrador 1 do
                                        // coprocessor 15
    "\n     BIC r1, r1, #0x1"           // Atribui-se ao bit 0 em R1 o valor 0, para
                                        // desligar a MMU
    "\n     MCR p15, 0, r1, c1, c0, 0"  // Escreve-se no reg 1 do coprocessor 15
                                        // o que ha em R1, desabilitando a MMU

    // Disable L1 Caches
    "\n     MRC p15, 0, r1, c1, c0, 0"  // Read Control Register configuration data
    "\n     BIC r1, r1, #(0x1 << 12)"   // Disable I Cache
    "\n     BIC r1, r1, #(0x1 << 2)"    // Disable D Cache
    "\n     MCR p15, 0, r1, c1, c0, 0"  // Write Control Register configuration data

    // Invalidate L1 Caches
    // Invalidate Instruction cache
    "\n     MOV r1, #0"
    "\n     MCR p15, 0, r1, c7, c5, 0"

    // Invalidate Data cache to make the code general purpose,
    // we calculate the cache size first and loop through each set + way
    "\n     MRC p15, 1, r0, c0, c0, 0"  // Read Cache Size ID
    "\n     LDR r3, =#0x1ff"
//  "\n     LDR r3, #0x1ff" /* PDF Cortex-A9 */
    "\n     AND r0, r3, r0, LSR #13"    // r0 = no. of sets - 1
    "\n     MOV r1, #0"                 // r1 = way counter way_loop
    "\n way_loop:"
    "\n     MOV r3, #0"                 // r3 = set counter set_loop
    "\n set_loop:"
    "\n     MOV r2, r1, LSL #30"
    "\n     ORR r2, r3, LSL #5"         // r2 = set/way cache operation format
    "\n     MCR p15, 0, r2, c7, c6, 2"  // Invalidate line described by r2
    "\n     ADD r3, r3, #1"             // Increment set counter
    "\n     CMP r0, r3"                 // Last set reached yet?
    "\n     BGT set_loop"               // if not, iterate set_loop
    "\n     ADD r1, r1, #1"             // else, next
    "\n     CMP r1, #4"                 // Last way reached yet?
    "\n     BNE way_loop"               // if not, iterate way_loop

    // Invalidate TLB
    "\n     MCR p15, 0, r1, c8, c7, 0"

    /* Branch Prediction Enable */  // Não tem no seminário

    /* Enable D-side Prefetch */    // Não tem no seminário

    // Aqui é criada uma L1 translation table na RAM que divide
    // todo o espaço de endereçamento de 4GB em seções de 1 MB,
    // todas com Full Access e Strongly Ordered
    "\n     LDR r0, =0xDE2"             // Atribui-se ao R0 parte do descriptor
//                  ^^^^^^ Binário no PDF (averiguar)
    "\n     LDR r1, =ttb_address"       // Atribui-se ao R1 endereço base
                                        // da L1 tranlastion table
//  "\n     LDR r1, ttb_address" /* PDF Cortex-A9 */
    "\n     LDR r3, = 4095"             // R3 se torna o contador para o loop

    "\n write_pte:"                     // Label do loop para escrita das
                                        // page table entry (PTE) da translation table
    "\n     ORR r2, r0, r3, LSL #20"    // Atribui-se ao R2 OR entre o endereço
                                        // e os bits padrão da PTE
    "\n     STR r2, [r1, r3, LSL #2]"   // Escreve-se a PTE na translation table
                                        // (endereço de escrita é o ttb_address somado
                                        // com contador e multiplicado por 4)
//  "\n     SUB r3, r3, #1"             // Decrementa-se contador do loop
//  "\n     CMP r3, #-1"                // Faz-se a comparação para verificar
                                        // se loop acabou
    "\n     SUBS r3, r3, #1" /* PDF Cortex-A9 */
    "\n     BNE write_pte"              // Caso o loop não tenha acabado,
                                        // escreve mais uma pte

    // Faz-se a primeira entrada da tranlastion table
    // cacheable, normal, write-back, write allocate
//                      vvvvv Todos estão binário no PDF (averiguar)
    "\n     BIC r0, r0, #0xC"           // Limpa-se CB bits
    "\n     ORR r0, r0, #0X4"           // Write-back, write allocate
    "\n     BIC r0, r0, #0x7000"        // Limpa-se TEX bits
    "\n     ORR r0, r0, #0x5000"        // Faz-se TEX write-back e write allocate
    "\n     ORR r0, r0, #0x10000"       // Torna compartilhável
    "\n     STR r0, [r1]"               // Escreve-se na primeira entrada

    // Inicializa a MMU
    "\n     MOV r1,#0x0"
    "\n     MCR p15, 0, r1, c2, c0, 2"  // Escrita do Translation Table Base Control Register
    "\n     LDR r1, =ttb_address"       // Atribui-se ao R1 endereço base
                                        // da L1 tranlastion table
//  "\n     LDR r1, ttb_address" /* PDF Cortex-A9 */
    "\n     MCR p15, 0, r1, c2, c0, 0"  // Escreve-se no reg 1 do coprocessor 15 o que ha
                                        // em r1 (endereco base da tranlastion table)

    // In this simple example, we don't use TRE or Normal Memory Remap Register.
    // Set all Domains to Client
    "\n     LDR r1, =0x55555555"
    "\n     MCR p15, 0, r1, c3, c0, 0"  // Write Domain Access Control Register

    // Enable MMU
    "\n     MRC p15, 0, r1, c1, c0, 0"  // Atribui-se ao R1 o valor do registrador 1 do
                                        // coprocessor 15
    "\n     ORR r1, r1, #0x1"           // Atribui-se ao bit 0 em R1 o valor 1, para
                                        // ligar a MMU
    "\n     MCR p15, 0, r1, c1, c0, 0"  // Escreve-se no reg 1 do coprocessor 15
                                        // o que há em R1, habilitando a MMU
    "\n");

    // For machines that do not feature a real MMU, frame size = 1 byte
    // TODO: The stack left at the top of the memory for INIT is freed at Thread::init()
    free(&_end, pages(Memory_Map::SYS_STACK - reinterpret_cast<unsigned int>(&_end)));
}

__END_SYS

