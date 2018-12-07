#include <hardware/idt.h>
#include <hardware/port.h>
#include <boot/print.h>

#define IDT_INTERRUPT_GATE	0xE
#define IDT_DESC_PRESENT	0x80

#define PIC1		        0x20		/* IO base address for master PIC */
#define PIC2		        0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	    PIC1
#define PIC1_DATA	        (PIC1+1)
#define PIC2_COMMAND	    PIC2
#define PIC2_DATA	        (PIC2+1)
#define PIC_EOI		        0x20		/* End-of-interrupt command code */



idt_t g_idt;

//in assembly
static void interrupt_ignore();
static void irq0x00();
static void irq0x01();
static void irq0x02();
static void irq0x03();
static void irq0x04();
static void irq0x05();
static void irq0x06();
static void irq0x07();
static void irq0x08();
static void irq0x09();
static void irq0x0A();
static void irq0x0B();
static void irq0x0C();
static void irq0x0D();
static void irq0x0E();
static void irq0x0F();
static void irq0x31();

static void irq0x80();

static void isr0x00();
static void isr0x01();
static void isr0x02();
static void isr0x03();
static void isr0x04();
static void isr0x05();
static void isr0x06();
static void isr0x07();
static void isr0x08();
static void isr0x09();
static void isr0x0A();
static void isr0x0B();
static void isr0x0C();
static void isr0x0D();
static void isr0x0E();
static void isr0x0F();
static void isr0x10();
static void isr0x11();
static void isr0x12();
static void isr0x13();

static void __set_idt_entry(uint8_t interrupt, uint16_t code_segment, void (*handler)(), uint8_t level, uint8_t type) {
    // address of pointer to code segment (relative to global descriptor table)
    // and address of the handler (relative to segment)
    g_idt.idts[interrupt].offset_1 = ((uint32_t) handler) & 0xFFFF;
    g_idt.idts[interrupt].offset_2 = (((uint32_t) handler) >> 16) & 0xFFFF;
    g_idt.idts[interrupt].selector = code_segment;

    g_idt.idts[interrupt].type_attr = IDT_DESC_PRESENT | ((level & 3) << 5) | type;
    g_idt.idts[interrupt].zero = 0;
}


void idt_init(uint16_t hardware_interrupt_offset, gdt_t *gdt) {
    g_idt.hardware_interrupt_offset = hardware_interrupt_offset;

	uint32_t code_segment =  gdt_code_segment_selector(gdt);
    for (uint8_t i = 255; i > 0; --i) {
        __set_idt_entry(i, code_segment, &interrupt_ignore, 0, IDT_INTERRUPT_GATE);
        g_idt.handlers[i] = 0;
    }
    __set_idt_entry(0, code_segment, &interrupt_ignore, 0, IDT_INTERRUPT_GATE);
    g_idt.handlers[0] = 0;
    uint16_t offset = g_idt.hardware_interrupt_offset;

#define SET_ISR(interrupt) \
    __set_idt_entry(interrupt, code_segment, &isr##interrupt, 0, IDT_INTERRUPT_GATE);
#define SET_IRQ(interrupt) \
    __set_idt_entry(offset + interrupt, code_segment, &irq##interrupt, 0, IDT_INTERRUPT_GATE);

    SET_ISR(0x00);
    SET_ISR(0x01);
    SET_ISR(0x02);
    SET_ISR(0x03);
    SET_ISR(0x04);
    SET_ISR(0x05);
    SET_ISR(0x06);
    SET_ISR(0x07);
    SET_ISR(0x08);
    SET_ISR(0x09);
    SET_ISR(0x0A);
    SET_ISR(0x0B);
    SET_ISR(0x0C);
    SET_ISR(0x0D);
    SET_ISR(0x0E);
    SET_ISR(0x0F);
    SET_ISR(0x10);
    SET_ISR(0x11);
    SET_ISR(0x12);
    SET_ISR(0x13);

    SET_IRQ(0x00);
    SET_IRQ(0x01);
    SET_IRQ(0x02);
    SET_IRQ(0x03);
    SET_IRQ(0x04);
    SET_IRQ(0x05);
    SET_IRQ(0x06);
    SET_IRQ(0x07);
    SET_IRQ(0x08);
    SET_IRQ(0x09);
    SET_IRQ(0x0A);
    SET_IRQ(0x0B);
    SET_IRQ(0x0C);
    SET_IRQ(0x0D);
    SET_IRQ(0x0E);
    SET_IRQ(0x0F);
    __set_idt_entry(0x80, code_segment, &irq0x80, 0, IDT_INTERRUPT_GATE);
#undef SET_ISR
#undef SET_IRQ

    /* remapping the PIC */
    port_write8_slow(PIC1_COMMAND, 0x11);
    port_write8_slow(PIC2_COMMAND, 0x11);
//
    port_write8_slow(PIC1_DATA, offset);
    port_write8_slow(PIC2_DATA, offset + 8);

    port_write8_slow(PIC1_DATA, 0x04);
    port_write8_slow(PIC2_DATA, 0x02);

    port_write8_slow(PIC1_DATA, 0x01);
    port_write8_slow(PIC2_DATA, 0x01);

    port_write8_slow(PIC1_DATA, 0x00);
    port_write8_slow(PIC2_DATA, 0x00);

    idt_pointer_t idt_pointer;
    idt_pointer.size  = 256 * sizeof(idt_descr_t) - 1;
    idt_pointer.base  = (uint32_t)g_idt.idts;
    __asm__ volatile("lidt %0" : : "m" (idt_pointer));
    printf("idt_init\n");
}

void idt_active() {
    if (g_idt.activated) {
        idt_deactive();
    }
    g_idt.activated = true;
    __asm__("sti");
    printf("idt_active\n");
}

void idt_deactive() {
    if (g_idt.activated) {
        g_idt.activated = false;
        __asm__("cli");
        printf("idt_deactive\n");
    }
}

void idt_register_handler(interrupt_handler_t *handler) {
    if (g_idt.handlers[handler->interrutpt_number] != 0) {
        printf("handler already exist");
    }
//
    g_idt.handlers[handler->interrutpt_number] = handler;
}

uint32_t idt_handle(uint8_t interrupt, uint32_t esp) {
    if(g_idt.handlers[interrupt] != 0) {
        esp = g_idt.handlers[interrupt]->cb_handler(esp, g_idt.handlers[interrupt]);
    } else if(interrupt != g_idt.hardware_interrupt_offset) {
        printf("UNHANDLED INTERRUPT 0x");
        printf_hex(interrupt);
    }

    // hardware interrupts must be acknowledged
    if(g_idt.hardware_interrupt_offset <= interrupt && interrupt < g_idt.hardware_interrupt_offset+16) {
        port_write8_slow(PIC1_COMMAND, PIC_EOI);

        if(g_idt.hardware_interrupt_offset + 8 <= interrupt) {
            port_write8_slow(PIC2_COMMAND, PIC_EOI);
        }
    }

    return esp;
}