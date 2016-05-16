.extern	system_init
.extern __bss_start
.extern __bss_end
.extern vFreeRTOS_ISR
.extern vPortYieldProcessor
.extern DisableInterrupts
.extern main
	.section .init
	.globl _start
;; 
_start:
	;@ All the following instruction should be read as:
	;@ Load the address at symbol into the program counter.
	
	ldr	pc,reset_handler		;@ 	Processor Reset handler 		-- we will have to force this on the raspi!
	;@ Because this is the first instruction executed, of cause it causes an immediate branch into reset!
	
	ldr pc,undefined_handler	;@ 	Undefined instruction handler 	-- processors that don't have thumb can emulate thumb!
    ldr pc,swi_handler			;@ 	Software interrupt / TRAP (SVC) -- system SVC handler for switching to kernel mode.
    ldr pc,prefetch_handler		;@ 	Prefetch/abort handler.
    ldr pc,data_handler			;@ 	Data abort handler/
    ldr pc,unused_handler		;@ 	-- Historical from 26-bit addressing ARMs -- was invalid address handler.
    ldr pc,irq_handler			;@ 	IRQ handler
    ldr pc,fiq_handler			;@ 	Fast interrupt handler.

	;@ Here we create an exception address table! This means that reset/hang/irq can be absolute addresses
reset_handler:      .word reset
undefined_handler:  .word undefined_instruction
swi_handler:        .word vPortYieldProcessor
prefetch_handler:   .word prefetch_abort
data_handler:       .word data_abort
unused_handler:     .word unused
irq_handler:        .word vFreeRTOS_ISR
fiq_handler:        .word fiq

reset:
    // Pi bootloader for armv7 and greater kicks us off in Hypervisor mode
    // Detect that case and return to supervisor mode.
#if __ARM_ARCH >= 7 
    mrs r0,cpsr
    and r1,r0,#0x1F		@ mode bits
    cmp r1,#0x1A		@ HYP mode?
    bne modeok

    // leave HYP mode, returning to supervisor mode
    bic r0,r0,#0x1F		@ clear mode
    orr r0,r0,#0x13		@ set SVC
    msr spsr_cxsf,r0		@ prep for eret out of HYP mode
    add r0,pc,#4
    msr ELR_hyp,r0
    eret
modeok:
#endif

    // change vector base address (VBAR) to our table
    adr r0, _start
    mcr p15, 0, r0, c12, c0, 0

	;@	Set up the various STACK pointers for different CPU modes
    ;@ (PSR_IRQ_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
    mov r0,#0xD2
    msr cpsr_c,r0
    mov sp,#0x8000

    ;@ (PSR_FIQ_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
    mov r0,#0xD1
    msr cpsr_c,r0
    mov sp,#0x4000

    ;@ (PSR_SVC_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
    mov r0,#0xD3
    msr cpsr_c,r0
	mov sp,#0x8000000

	ldr r0, =__bss_start
	ldr r1, =__bss_end

	mov r2, #0

zero_loop:
	cmp 	r0,r1
	it		lt
	strlt	r2,[r0], #4
	blt		zero_loop

	bl 		DisableInterrupts
	
	
	;@ 	mov	sp,#0x1000000
	b main									;@ We're ready?? Lets start main execution!
	.section .text

undefined_instruction:
	b undefined_instruction

prefetch_abort:
	b prefetch_abort

data_abort:
	b data_abort

unused:
	b unused

fiq:
	b fiq
	
hang:
	b hang

