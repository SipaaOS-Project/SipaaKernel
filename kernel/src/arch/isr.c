#include "isr.h"
#include "idt.h"
#include "8259_pic.h"
#include <libasg/libasg.h>
#include <res/res.h>

// For both exceptions and irq interrupt
ISR g_interrupt_handlers[NO_INTERRUPT_HANDLERS];

// for more details, see Intel manual -> Interrupt & Exception Handling
char *exception_messages[32] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "BOUND Range Exceeded",
    "Invalid Opcode",
    "Device Not Available (No Math Coprocessor)",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt (intel reserved)",
    "x87 FPU Floating-Point Error (Math Fault)",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

/**
 * register given handler to interrupt handlers at given num
 */
void isr_register_interrupt_handler(int num, ISR handler) {
    //printf("IRQ %d registered\n", num);
    if (num < NO_INTERRUPT_HANDLERS)
        g_interrupt_handlers[num] = handler;
}

/*
 * turn off current interrupt
*/
void isr_end_interrupt(int num) {
    pic8259_eoi(num);
}

/**
 * invoke isr routine and send eoi to pic,
 * being called in irq.asm
 */
void isr_irq_handler(REGISTERS *reg) {
    if (g_interrupt_handlers[reg->int_no] != 0) {
        ISR handler = g_interrupt_handlers[reg->int_no];
        handler(reg);
    }
    pic8259_eoi(reg->int_no);
}

/**
static void print_registers(REGISTERS *reg) {
    printf("REGISTERS:\n");
    printf("err_code=%d\n", reg->err_code);
    printf("eax=0x%x, ebx=0x%x, ecx=0x%x, edx=0x%x\n", reg->eax, reg->ebx, reg->ecx, reg->edx);
    printf("edi=0x%x, esi=0x%x, ebp=0x%x, esp=0x%x\n", reg->edi, reg->esi, reg->ebp, reg->esp);
    printf("eip=0x%x, cs=0x%x, ss=0x%x, eflags=0x%x, useresp=0x%x\n", reg->eip, reg->ss, reg->eflags, reg->useresp);
}
**/

/**
 * invoke exception routine,
 * being called in exception.asm
 */
void isr_exception_handler(REGISTERS reg) {
    if (reg.int_no < 32) {
        // Draw error to screen
        Color white = { 255, 255, 255, 255 };
        Color bg = { 255, 143, 0, 0 };
        Font f;
        f.charwidth = 8;
        f.charheight = 16;
        f.pixels = (uint8_t *)deffont;
        clear_screen(bg);
        Position pos1 = set_string(":'(\nSipaaKernel ran into a problem and needs to restart...\nStop message : ", 10, 10, f, white);
        Position pos2 = set_string(exception_messages[reg.int_no], pos1.x, pos1.y, f, white);
        Position pos3 = set_string("\n\n", pos2.x, pos2.y, f, white);
        Position pos4 = set_string("If this is the first time seeing this screen, reboot.\nElse, try to report the bug to the SipaaKernel Team (by Discord or GitHub).\n\n\nCPU halted.\n\n\n\n\n\n\n\n", 10, pos3.y, f, white);
        
        flush();

        //printf("EXCEPTION: %s\n", exception_messages[reg.int_no]);
        //print_registers(&reg);
        asm("hlt");
        for (;;)
            ;
    }
    if (g_interrupt_handlers[reg.int_no] != 0) {
        ISR handler = g_interrupt_handlers[reg.int_no];
        handler(&reg);
    }
}